#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''Provides functions for iCal export.'''

import datetime
import os
import subprocess
import dtformat

def create_ical(subject, isodt):
    '''Create a string with an iCal entry for a point in time.'''
    event_time = dtformat.interpret_iso_8601(isodt)
    try:
        event_time = event_time.astimezone(datetime.timezone.utc)
    except ValueError:
        pass
    icalstr = []
    icalstr.append('BEGIN:VCALENDAR')
    icalstr.append('VERSION:2.0')
    icalstr.append('PRODID:-//p2501.ch//Pass Viewer 0.4//EN')  # Don't forget to update this
    icalstr.append('BEGIN:VEVENT')
    icalstr.append('UID:' + subject.replace(' ', '_') + '/' + isodt + '/harbour-passviewer')
    icalstr.append('DTSTAMP:' + datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ'))
    icalstr.append('DTSTART:' + event_time.strftime('%Y%m%dT%H%M%SZ'))
    icalstr.append('SUMMARY:' + subject)
    icalstr.append('TRANSP:TRANSPARENT')
    icalstr.append('END:VEVENT')
    icalstr.append('END:VCALENDAR')
    return '\n'.join(icalstr)

def create_calendar_entry(subject, isodt):
    '''Create a calendar entry in the system calendar.'''
    xdg_cache = ''
    try:
        xdg_cache = os.environ['XDG_CACHE_HOME']
    except KeyError:
        xdg_cache = os.path.expanduser('~/.cache')
    if not os.path.isdir(xdg_cache):
        os.mkdir(xdg_cache)
    xdg_cache = os.path.join(xdg_cache, 'harbour-passviewer')
    if not os.path.isdir(xdg_cache):
        os.mkdir(xdg_cache)
    xdg_cache = os.path.join(xdg_cache, 'harbour-passviewer')
    if not os.path.isdir(xdg_cache):
        os.mkdir(xdg_cache)
    try:
        icalstr = create_ical(subject, isodt)
    except Exception:
        return "format"
    with open(os.path.join(xdg_cache, 'passbook.ics'), 'w') as icalfile:
        icalfile.write(icalstr)
    if subprocess.call(['xdg-open', os.path.join(xdg_cache, 'passbook.ics')]) != 0:
        return "xdg-open"
    return "ok"
