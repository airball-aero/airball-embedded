import socket
import time

UDP_IP = "127.0.0.1"
UDP_PORT = 30123

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind(('', UDP_PORT))

last_n = 0

while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    s = data.decode().split(',')
    if not s[0] == '$AR': continue
    n = int(s[1])
    if n < last_n:
        print('reset at: %s' % str(time.time()))
    last_n = n
