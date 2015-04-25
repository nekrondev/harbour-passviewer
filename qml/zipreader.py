#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''Provides easy access to the passes in the file system.'''

import os
from zipfile import ZipFile
import pyotherside

def scan_home():
    '''Return all direct and indirect subdirectories of the home directory, and the passes within.'''
    inspect_paths = [os.path.expanduser('~')]
    visible_paths = []
    passes = []
    while len(inspect_paths) > 0:
        current_path = inspect_paths.pop(0)
        visible_paths.append(current_path)
        for name in os.listdir(current_path):
            fullname = os.path.realpath(os.path.join(current_path, name))
            if os.path.isdir(fullname):
                # ignore hidden and temp directories, and what we already know about
                if name.startswith('.') or name.lower() == 'tmp' or name.lower() == 'temp' or fullname in inspect_paths or fullname in visible_paths:
                    continue
                inspect_paths.append(fullname)
            if not name.endswith('.pkpass'):
                continue
            try:
                with ZipFile(fullname) as zip:
                    with zip.open('pass.json') as json:
                        passes.append({'name': name, 'path': fullname, 'data': json.read()})
            except Exception:
                continue
    return visible_paths, passes

def image_provider(id, requested_size):
    '''Provide images from within ZIP files.'''
    imgdata = b''
    try:
        zipname, imgname = id.rsplit('/',1)
        imgdata = b''
        with ZipFile('/' + zipname) as zip:
            with zip.open(imgname) as img:
                imgdata = img.read()
    except Exception:
        return bytearray(), (0, 0), pyotherside.format_rgb32
    return bytearray(imgdata), (-1, -1), pyotherside.format_data

pyotherside.set_image_provider(image_provider)
