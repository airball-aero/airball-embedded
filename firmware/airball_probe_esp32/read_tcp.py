#!/usr/bin/python

import socket
import string

conn = socket.create_connection(('192.168.4.1', 80))
connfile = conn.makefile()
i = 0

while True:
    data = connfile.readline()
    data = string.strip(str(data))
    print(data)
    if data.startswith('$AR'):
        try:
            n = int(data.split(',')[1])
            if n != i:
                print('SKIPPED')
                i = n + 1
            else:
                i = i + 1
        except:
            print('Parse error')
