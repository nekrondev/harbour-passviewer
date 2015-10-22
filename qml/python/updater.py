#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''Provides routines for pass updates.'''

import pyotherside

from threading import Thread
from multiprocessing import Process, Queue
from zipfile import ZipFile
import json
import urllib.request
import email.utils
import time
import io
import sys

from passdb import PassDB

import zipreader

def update_proc(path, signal):
    '''Handle the update. (worker process)'''
    oldraw = None
    oldpass = None
    try:
        with open(path, 'rb') as passfile:
            oldraw = passfile.read()
            passfile.seek(0)
            oldpass = get_pass_data(passfile)
        base_URL = oldpass['webServiceURL']
        passid = oldpass['passTypeIdentifier']
        serial = oldpass['serialNumber']
        auth = oldpass['authenticationToken']
    except Exception:
        signal.put('not updateable')
        return
    request = urllib.request.Request('/'.join((base_URL, 'v1', 'passes', passid, serial)))
    request.add_header('Authorization', 'ApplePass ' + auth)
    db = None
    passinfo = None
    try:
        db = PassDB()
        passinfo = db.get_pass('/'.join((passid, serial)))
        if passinfo['updated'] is not None:
            request.add_header('If-Modified-Since', email.utils.formatdate(passinfo['updated'], usegmt=True))
    except:
        pass
    newraw = None
    newpass = None
    try:
        with urllib.request.urlopen(request, cadefault=True) as response:
            newraw = response.read()
        newpass = get_pass_data(io.BytesIO(newraw))
        with open(path, 'wb') as passfile:
            passfile.write(newraw)
    except urllib.error.HTTPError:
        if str(sys.exc_info()[1]).startswith('HTTP Error 304:'):
            signal.put('no new version')
        else:
            signal.put('update failed')
        return
    except Exception:
        signal.put('update failed')
        return
    if oldraw == newraw:
        signal.put('no new version')
    else:
        signal.put('ok')
    if passinfo is not None:
        try:
            passinfo['updated'] = time.time()
            passinfo['changes'] = compare_passes(oldpass, newpass)
            db.set_pass(passinfo)
        except:
            pass

def watcher_thread(path):
    '''Start and watch the update. (worker thread)'''
    signal = Queue()
    proc = Process(target = update_proc, args = (path, signal))
    proc.start()
    try:
        pyotherside.send('update', signal.get(timeout = 300))
    except Exception:
        pass
    proc.join()

def update(path):
    '''Update the pass indicated by the path.'''
    thread = Thread(target = watcher_thread, args = (path, ))
    thread.start()

def get_pass_data(file):
    '''Get and interpret the JSON data from a pass.'''
    pass_data = None
    try:
        zip = ZipFile(file)
        with zip.open('pass.json') as jsonfile:
            data = zipreader.decode_binary(jsonfile.read())
            pass_data = json.loads(data)
    except Exception:
        pass_data = None
    return pass_data

def compare_passes(oldpass, newpass):
    '''Compare the fields of two passes, and return a list of changed fields.'''
    changed = []
    oldfields = get_fields(oldpass)
    newfields = get_fields(newpass)
    for newkey in newfields.keys():
        if newkey not in oldfields or newfields[newkey] != oldfields[newkey]:
            changed.append(newkey)
    return changed

def get_fields(thispass):
    '''Get the fields of a pass.'''
    fields = {}
    for passtype in ('boardingPass', 'coupon', 'eventTicket', 'storeCard', 'generic'):
        try:
            for fieldtype in ('headerFields', 'primaryFields', 'secondaryFields', 'auxiliaryFields'):
                try:
                    for field in thispass[passtype][fieldtype]:
                        fields[field['key']] = field['value']
                except KeyError:
                    pass
        except KeyError:
            pass
    return fields

def get_pass_changes(thispass):
    '''Gets and clears the last changes of a pass.'''
    try:
        db = PassDB()
        id = '/'.join((thispass['passTypeIdentifier'], thispass['serialNumber']))
        data = db.get_pass(id)
        changes = data['changes']
        data['changes'] = []
        db.set_pass(data)
        return changes
    except Exception:
        return []
