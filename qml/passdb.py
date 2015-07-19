# -*- coding: utf-8 -*-

'''Provides access to a DB with pass update information.'''

import os
import sqlite3

class PassDB:
    '''Provides access to a DB with pass update information.'''

    def __init__(self):
        # get (create if necessary) xdg data directory
        datadir = ''
        try:
            datadir = os.environ['XDG_DATA_HOME']
        except KeyError:
            datadir = os.path.expanduser('~/.local/share')
        if not os.path.isdir(datadir):
            os.mkdir(datadir)
        datadir = os.path.join(datadir, 'harbour-passviewer')
        if not os.path.isdir(datadir):
            os.mkdir(datadir)
        datadir = os.path.join(datadir, 'harbour-passviewer')
        if not os.path.isdir(datadir):
            os.mkdir(datadir)
        # open (or create) DB
        self.db = sqlite3.connect(os.path.join(datadir, 'passes.db'))
        self.db.execute('PRAGMA foreign_keys = ON')
        # create tables if necessary
        for table in ('passes', 'changes'):
            check_table = self.db.cursor()
            check_table.execute("select count(*) from sqlite_master where type='table' and name=?", (table,))
            reply = check_table.fetchone()
            if reply is None or reply[0] == 0:
                if table == 'passes':
                    self.db.execute('create table passes (pass_id text primary key, updated integer)')
                else:
                    self.db.execute('''create table changes (
                                           change_id integer primary key,
                                           pass_id text not null references passes (pass_id) on delete cascade,
                                           field text not null)''')
                self.db.commit()

    def get_passes(self):
        '''List all known pass ids.'''
        find_passes = self.db.cursor()
        find_passes.execute('select pass_id from passes')
        passes = []
        for thispass in find_passes.fetchall():
            passes.append(thispass[0])
        return passes

    def get_pass(self, pass_id):
        '''Get update information for a pass.'''
        thispass = { 'id': pass_id, 'updated': None, 'changes': [] }
        find_pass = self.db.cursor()
        find_pass.execute('select updated from passes where pass_id = ?', (pass_id,))
        updated = find_pass.fetchone()
        if updated is not None:
            thispass['updated'] = updated[0]
        find_changes = self.db.cursor()
        find_changes.execute('select field from changes where pass_id = ?', (pass_id,))
        for field in find_changes.fetchall():
            thispass['changes'].append(field[0])
        return thispass

    def set_pass(self, thispass=None, pass_id=None):
        '''Set or delete update information for a pass.'''
        if thispass is None:
            if pass_id is None:
                raise ValueError('pass or pass_id must be set')
            thispass = { 'id': pass_id, 'updated': None, 'changes': () }
        self.db.execute('delete from passes where pass_id = ?', (thispass['id'],))
        if thispass['updated'] is not None or len(thispass['changes']) > 0:
            self.db.execute('insert into passes values (?, ?)', (thispass['id'], thispass['updated']))
            for field in thispass['changes']:
                self.db.execute('insert into changes (pass_id, field) values (?, ?)', (thispass['id'], field))
        self.db.commit()
