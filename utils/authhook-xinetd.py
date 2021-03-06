#!/usr/bin/python

# ------------ This is what should be but in xinetd configuration file -----
#service wab-rdp
#{
#    disable     = yes
#    port        = 3333
#    protocol    = tcp
#    redirect    = 127.0.0.1 3389
##    server      = /usr/local/bin/rdpproxy
##    server_args = -i
#    socket_type = stream
#    type        = UNLISTED
#    user        = root
#    wait        = no
#}

#service wab-rdp
#{
#    disable     = no
#    port        = 3450
#    protocol    = tcp
##    redirect    = 127.0.0.1 3389
#    server      = <path>/utils/authhook-xinetd.py
##    server_args = -i
#    socket_type = stream
#    type        = UNLISTED
#    user        = root
#    wait        = no
#}
# --------------------------------------------------------------------------

import syslog

import select
import httplib, socket
from socket import error
from struct    import unpack
from struct    import pack

import socket
import sys

from sys import stdin, stdout

from socket import fromfd
from socket import AF_INET
from socket import SOCK_STREAM
from socket import SOL_SOCKET
from socket import SO_REUSEADDR


class Authentifier(object):
    # we should just transmit some kind of salted hash to get something
    # more secure for password transmission, but for now the authentication
    # protocol is just supposed to be used locally on a secure system.
    # It will certainly change to something stronger to avoid storing passwords
    # at all. Comparing hashes is enough anyway.
    passwords = {
        'w2008': 'w2008',
        'w7': 'w7',
        'w2000': 'w2000',
        'xp': 'xp',
        'v' : 'v',
        'n' : 'n',
        'cgr' : 'cgr',
        'bouncer' : 'bouncer',
        'test' : 'test',
        'card' : 'card',
        'loop' : 'loop',
        'looop' : 'looop',
        'error_invalid' : 'error_invalid'
    }
    targets = {
        'w2008' : {
            'proxy_type': 'RDP',
            'target_login' : r'qa\administrateur',
            'target_device' : '10.10.14.78',
            'target_password' : 'S3cur3!1nux',
            'proto_dest': 'RDP',
            'target_port':'3389',
            'is_rec' : 'true',
            'rec_path':'/tmp/trace2008.cpp',
        },
        'w7' : {
            'proxy_type': 'RDP',
            'target_login' : r'qa\administrateur',
            'target_device' : '10.10.14.77',
            'target_password' : 'S3cur3!1nux',
            'proto_dest': 'RDP',
            'target_port':'3389',
            'is_rec' : 'true',
            'rec_path':'/tmp/tracew7.cpp',
        },
        'w2000' : {
            'proxy_type': 'RDP',
            'target_login' : r'administrateur',
            'target_device' : '10.10.14.64',
            'target_password' : 'SecureLinux',
            'proto_dest': 'RDP',
            'target_port':'3389'
        },
        'xp' : {
            'proxy_type': 'RDP',
            'target_login' : r'qa\administrateur',
            'target_device' : '10.10.14.111',
            'target_password' : 'S3cur3!1nux',
            'proto_dest': 'RDP',
            'target_port':'3389',
            'is_rec':'True',
            'rec_path':'/tmp/tracexp.cpp',
        },
        'bouncer' : {
            'proxy_type': 'RDP',
            'target_device' : 'bouncer2',
            'proto_dest': 'INTERNAL'
        },
        'test' : {
            'proxy_type': 'RDP',
            'target_device' : 'test',
            'proto_dest': 'INTERNAL'
        },
        'card' : {
            'proxy_type': 'RDP',
            'target_device' : 'test_card',
            'proto_dest': 'INTERNAL',
            'is_rec':'True',
            'rec_path':'/tmp/test_card.cpp',
        },
        'n' : {
            'proxy_type': 'RDP',
            'target_login' : r'vnc',
            'target_device' : '10.10.4.163',
            'target_password' : 'tabra',
            'target_port':'5900',
            'proto_dest': 'VNC'
        },
        'v' : {
            'proxy_type': 'RDP',
            'target_login' : r'vnc',
            'target_device' : '192.168.1.28',
            'target_password' : 'secure',
            'target_port':'5900',
            'proto_dest': 'VNC'
        },
        'cgr' : {
            'proxy_type': 'RDP',
            'target_login' : r'vnc',
            'target_device' : '10.10.4.13',
            'target_password' : 'secure',
            'target_port':'5901',
            'proto_dest': 'VNC'
        },
        'loop' : {
            'proxy_type': 'RDP',
            'target_login' : r'card',
            'target_device' : '127.0.0.1',
            'target_password' : 'card',
            'target_port':'3389',
            'proto_dest': 'RDP'
        },
        'looop' : {
            'proxy_type': 'RDP',
            'target_login' : r'loop',
            'target_device' : '127.0.0.1',
            'target_password' : 'loop',
            'target_port':'3389',
            'proto_dest': 'RDP'
        },
        'error_invalid' : {
            'target_device': "10.10.3.54" ,
            'target_login': "administrateur",
            'authenticated': 'False',
            'rejected':"Invalid IP Source"}
    }

    def __init__(self, sck):
        self.sck = sck

    def read(self):
        syslog.syslog("Reading")
        try:
            _packet_size, = unpack(">L", self.sck.recv(4))
            syslog.syslog("Received Data length : %s" % _packet_size)
            _data = self.sck.recv(int(_packet_size))
        except Exception, e:
            # It's quick and dirty, but we do as if all possible errors
            # are authentifier socket was closed.
            return False

        p = iter(_data.split('\n'))
        self.dic = dict((x, y) for x, y in zip(p, p) if (x[:6] != 'trans_'))

        syslog.syslog("%s" % self.dic)
        _login = self.dic.get('login')
        if _login:
            _password = self.dic.get('password')
            if _password and self.passwords.get(_login) == _password:
                syslog.syslog("Password OK")
                if _login[:5] == 'error':
                    self.dic = self.targets.get(_login, {})
                else:
                    self.dic.update(self.targets.get(_login, {}))
            else:
                syslog.syslog("Wrong Password")
                self.dic = {'login' : 'ASK' }
        self.send()
        return True

    def send(self):
        syslog.syslog("Sending %s" % self.dic)
        _data = ''.join(["%s\n" % v for tu in self.dic.iteritems() for v in tu])
        _len = len(_data)
        self.sck.send(pack(">L", _len+4))
        self.sck.send(_data)


wsockets = []
sck = fromfd(stdin.fileno(), AF_INET, SOCK_STREAM)
rsockets = [sck]
manager[sck] = Authentifier(sck)
syslog.syslog("Starting authhook-xinetd")

while 1:
    rfds, wfds, xfds = select.select(rsockets, [], [], 1)

    for s in rfds:
        if not manager[s].read():
            del manager[s]
