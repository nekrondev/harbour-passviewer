#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''Provides functions to mimic a certain date/time formatter.'''

import datetime
import locale

def _init():
    '''Initializes the date and time formats.'''
    global datefmts
    global timefmts

    # get locale information
    locale.resetlocale()
    dtfmt = date_codes(locale.nl_langinfo(locale.D_T_FMT))
    dfmt = date_codes(locale.nl_langinfo(locale.D_FMT))
    tfmt = time_codes(locale.nl_langinfo(locale.T_FMT))

    # derive formats from the locale information
    datefmts['short'] = clear_codes(dfmt.replace('%b', '%m').
            replace('%B', '%m').replace('%Y', '%y'), 'demy')
    datefmts['medium'] = clear_codes(dtfmt.replace('%B', '%b').
            replace('%y', '%Y'), 'debmY')
    datefmts['long'] = clear_codes(dtfmt.replace('%b', '%B').
            replace('%y', '%Y'), 'deBmY')
    datefmts['full'] = clear_codes(dtfmt.replace('%a', '%A').
            replace('%b', '%B').replace('%y', '%Y'), 'AdeBmY')
    if '%p' in tfmt.lower():
        timefmts['short'] = '%I:%M %p'
        timefmts['medium'] = '%I:%M:%S %p'
        timefmts['long'] = '%I:%M:%S %p %Z'
    else:
        timefmts['short'] = '%H:%M'
        timefmts['medium'] = '%H:%M:%S'
        timefmts['long'] = '%H:%M:%S %Z'
    timefmts['full'] = timefmts['long']

def date_codes(orig):
    '''Replace %D, %F and %h with generic counterparts.'''
    return orig.replace('%D', '%m/%d/%y').replace('%F', '%Y-%m-%d').\
            replace('%h', '%b')

def time_codes(orig):
    '''Replace %r, %R and %T with generic counterparts.'''
    return orig.replace('%r', '%I:%M:%S %p').replace('%R', '%H:%M').\
            replace('%T', '%H:%M:%S')

def clear_codes(orig, allowed):
    '''Clear out unwanted date/time codes.'''
    cleared = ''
    first = True
    while orig.find('%') != -1:
        pos = orig.find('%')
        if not first:
            cleared += orig[:pos]
        size = 3
        code = orig[pos + 1]
        if code in ('E', 'O'):
            size = 4
            code = orig[pos + 2]
        if code in allowed:
            cleared += orig[pos:pos + size]
            first = False
        orig = orig[pos + size:]
    pos = cleared.rfind('%') + 1
    if cleared[pos] in ('E', 'O'):
        pos += 1
    return cleared[:pos + 1]

def interpret_iso_8601(isodt):
    '''Turn an ISO 8601 date, time or datetime into a datetime object.'''
    # only the common forms matching ISO 2014, ISO 3307 and ISO 4031
    # are supported.
    has_date = False
    has_time = False
    is_utc = False
    has_timezone = False
    if 'T' in isodt:
        has_date = True
        has_time = True
        if isodt.endswith('Z'):
            is_utc = True
        elif '+' in isodt or '-' in isodt[10:]:
            has_timezone = True
    elif ':' in isodt:
        has_time = True
        if isodt.endswith('Z'):
            is_utc = True
        elif '+' in isodt or '-' in isodt:
            has_timezone = True
    else:
        has_date = True
    parse = []
    if has_date:
        parse.append('%Y-%m-%d')
        if has_time:
            parse.append('T')
    if has_time:
        parse.append('%H:%M')
        mincols = 1
        if has_timezone:
            mincols = 2
        if isodt.count(':') > mincols:
            parse.append(':%S')
            if isodt.count('.') > 0:
                # eliminate milliseconds
                ms_start = isodt.find('.')
                ms_end = isodt.find('Z', ms_start)
                if ms_end == -1:
                    ms_end = isodt.find('+', ms_start)
                if ms_end == -1:
                    ms_end = isodt.find('-', ms_start)
                if ms_start > -1 and ms_end > -1:
                    isodt = isodt[:ms_start] + isodt[ms_end:]
        if is_utc:
            isodt = isodt[:-1] + '+0000'
            parse.append('%z')
        elif has_timezone:
            tzcol = isodt.rfind(':')
            isodt = isodt[:tzcol] + isodt[tzcol + 1:]
            parse.append('%z')
    try:
        return datetime.datetime.strptime(isodt, ''.join(parse))
    except ValueError:
        raise ValueError('cannot recognize date/time format')

def format_date_time(isodt, datefmt, timefmt, ignore_time_zone = False):
    global datefmts
    global timefmts

    try:
        dtobj = interpret_iso_8601(isodt)
    except ValueError:
        return isodt
    if not ignore_time_zone:
        try:
            dtobj = dtobj.astimezone()  # converts to local time zone
        except ValueError:
            pass

    dtstrs = []
    if datefmt in datefmts:
        dtstrs.append(dtobj.strftime(datefmts[datefmt]))
    if timefmt in timefmts:
        dtstrs.append(dtobj.strftime(timefmts[timefmt]))
    return ' '.join(dtstrs)

datefmts = {}
timefmts = {}
_init()
