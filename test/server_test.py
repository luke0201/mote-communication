import sys
import socket

BUFSIZE = 1024


def main():
    print('Server test', end='\n\n')

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
            ret = c.recv(BUFSIZE)
            print('From client:', ret, sep='\n', end='\n\n')

            c.send(ret)
            if ret == b'exit': break

        c.close()

    s.close()


if __name__ == '__main__':
    main()
