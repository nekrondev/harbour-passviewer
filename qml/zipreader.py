#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''Provides easy access to the passes in the file system.'''

import pyotherside

import os
from zipfile import ZipFile
import json
from base64 import b64encode

from passdb import PassDB

def scan_home(sdcard = True):
    '''Return all direct and indirect subdirectories of the home directory, and the passes within.'''
    inspect_paths = [os.path.expanduser('~')]
    if sdcard and os.path.isdir('/media/sdcard'):
        inspect_paths.append('/media/sdcard')
    visible_paths = []
    passes = []
    while len(inspect_paths) > 0:
        current_path = inspect_paths.pop(0)
        visible_paths.append(current_path)
        try:
            for name in os.listdir(current_path):
                fullname = os.path.realpath(os.path.join(current_path, name))
                if os.path.isdir(fullname):
                    # ignore hidden and temp directories, and what we already know about
                    if name.startswith('.') or name.lower() == 'tmp' or name.lower() == 'temp' or fullname in inspect_paths or fullname in visible_paths:
                        continue
                    inspect_paths.append(fullname)
                    continue
                # only look at pkpass files
                if not name.endswith('.pkpass'):
                    continue
                try:
                    # pass must be a zip archive with a "pass.json" file in it
                    with ZipFile(fullname) as zip:
                        with zip.open('pass.json') as json:
                            data = decode_binary(json.read())
                            passes.append({'name': name, 'path': fullname, 'data': data})
                except Exception:
                    continue
        except Exception:
            visible_paths.pop()
    clean_passdb(passes)
    return visible_paths, passes

def decode_binary(bintext):
    '''Decode a text that may be UTF-16, UTF-8 or Latin-1.'''
    text = ''
    try:
        if bintext[:2] in (b'\xfe\xff', b'\xff\xfe'):
            text = bintext.decode('utf-16')
        else:
            text = bintext.decode('utf-8')
    except Exception:
        text = bintext.decode('latin-1', 'ignore')
    return text

def remove_pass(path):
    '''Remove the pass identified by the path.'''
    if not path.endswith('.pkpass'):
        return
    try:
        os.remove(path)
    except Exception:
        pass

def image_provider(id, requested_size):
    '''Provide images from within ZIP files.'''
    imgdata = b''
    try:
        zipname, imgname = id.rsplit('/',1)
        imgdata = b''
        with ZipFile('/' + zipname) as zip:
            # check for HD icon
            if imgname == 'icon.png':
                try:
                    with zip.open('icon@2x.png') as img:
                        imgdata = img.read()
                except KeyError:
                    imgdata = b''
            # normal search
            if imgdata == b'':
                with zip.open(imgname) as img:
                    imgdata = img.read()
    except Exception:
        return bytearray(), (0, 0), pyotherside.format_rgb32
    return bytearray(imgdata), (-1, -1), pyotherside.format_data

pyotherside.set_image_provider(image_provider)

def clean_passdb(passes):
    '''Clean removed passes from the Pass DB.'''
    try:
        db = PassDB()
    except Exception:
        return
    oldpasses = db.get_passes()
    # find still existing passes
    for newpass in passes:
        try:
            data = json.loads(newpass['data'])
            id = '/'.join((data['passTypeIdentifier'], data['serialNumber']))
            for index, oldpass in enumerate(oldpasses):
                if oldpass == id:
                    del oldpasses[index]
                    break
        except Exception:
            pass
    # remove vanished passes
    for oldpass in oldpasses:
        db.set_pass(pass_id=oldpass)
