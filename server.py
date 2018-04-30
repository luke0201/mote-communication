import sys
import re
import socket

from utils import coap

BUFSIZE = 1024
IPLIST_PATH = 'iplist.txt'


def load_iplist(path):
    with open(path, 'r') as f:
        iplist = [s.strip() for s in f.readlines()]

    # Check if any invalid address exists
    pat = re.compile(b'(?:[A-F0-9]{1,4}:){7}[A-F0-9]{1,4}', re.IGNORECASE)
    for i, ip in enumerate(iplist, start=1):
        if pat.fullmatch(ip) is None:
            print('Wrong IPv6 format in line {}'.format(i))
            sys.exit()

    return iplist


def main():
    print('Team 4 (server)', end='\n\n')

    iplist = load_iplist(IPLIST_PATH)
    print('{} motes connected'.format(len(iplist)))
    for ip in iplist:
        print('-', ip)

    # Create a socket and listen
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    host = socket.gethostname()
    port = 4444
    s.bind((host, port))
    s.listen()

    while True:
        print('Waiting for a new connection..')
        c, addr = s.accept()
        print('Connection from {}'.format(addr))
        c.send(b'Connected')

        while True:
            ret = str(c.recv(BUFSIZE))
            print('From client:', ret, sep='\n', end='\n\n')

            if ret == 'exit':
                break
            ret = ret.split()

            if len(ret) != 4 or ret[0] != 'put' or ret[2] != 'to':
                c.send(b'Wrong message format')
            else:
                payload = ret[1]
                moteno = int(ret[3])

                # Send CoAP request to actual mote
                print('Send to mote {}..'.format(moteno))
                ret = coap.req_coap(iplist[moteno], 'put', payload)
                print('From mote {}:'.format(moteno), ret, sep='\n')

                # Forward response message to client
                c.send(byte(ret, 'utf-8'))
                print('Sent back to client')

            print()

        c.close()

    s.close()


if __name__ == '__main__':
    main()
