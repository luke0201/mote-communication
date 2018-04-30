import sys
import re
import socket
import threading

from utils import coap


def load_iplist(path):
    with open(path, 'r') as f:
        iplist = [s.strip() for s in f.readlines()]

    """
    # Check if any invalid address exists
    pat = re.compile(r'(?:[A-F0-9]{1,4}:){7}[A-F0-9]{1,4}', re.IGNORECASE)
    for i, ip in enumerate(iplist, start=1):
        if pat.fullmatch(ip) is None:  # Invalid address format
            print('Wrong IPv6 format in line {}'.format(i))
            sys.exit()
    """

    return iplist


class TcpThread(threading.Thread):

    def __init__(self, host, port, bufsize):
        threading.Thread.__init__(self)
        self.host = host
        self.port = port
        self.bufsize = bufsize

    def run(self):
        global iplist

        # Create a socket and listen
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind((self.host, self.port))
        s.listen()

        while True:
            print('Waiting for a new connection..')
            c, addr = s.accept()
            print('Connection from {}'.format(addr))
            c.send(b'Connected')
            print()

            # Wait for the client to send requests and then response
            while True:
                ret = str(c.recv(BUFSIZE), 'utf-8')
                print('From client:', ret, sep='\n', end='\n\n')

                if ret == 'exit':
                    break
                ret = ret.split()

                # Message format: put <payload> to <moteno>
                if len(ret) != 4 or ret[0] != 'put' or ret[2] != 'to':
                    c.send(b'Wrong message format')
                else:
                    payload = ret[1]
                    moteno = int(ret[3])

                    # Send CoAP request to actual mote
                    print('Send to mote {}..'.format(moteno))
                    ret = coap.req_coap(iplist[moteno], 'PUT', payload)
                    print('From mote {}:'.format(moteno), ret, sep='\n')

                    # Forward response message to client
                    c.send(byte(ret, 'utf-8'))
                    print('Sent back to client')

                print()
            c.close()
        s.close()


class CoapThread(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        pass


def main():
    iplist_path = 'iplist.txt'
    host = '0.0.0.0'
    port = 4444
    bufsize = 1024

    print('Team 4 (server)', end='\n\n')

    iplist = load_iplist(iplist_path)
    print('{} motes connected'.format(len(iplist)))
    for ip in iplist:
        print('-', ip)

    # Create server threads
    tcp_thread = TcpThread(host, port, bufsize)
    coap_thread = CoapThread()
    threads = [tcp_thread, coap_thread]

    # Start threads
    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()
    print('Server closed')


if __name__ == '__main__':
    main()
