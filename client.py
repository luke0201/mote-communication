import sys
import socket

HOST = 127.0.0.1
PORT = 4444
BUFSIZE = 1024


def main():
    print('Team 4', end='\n\n')

    # Connect to server
    print('Connecting to server..')
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    print(s.recv(BUFSIZE), end='\n\n')

    while True:
        print('>', end=' ')
        oper = input()
        if oper == 'help':
            print('Message format: <req_type> [<payload>] [to <mote_no>]')
        elif oper == 'exit':
            break;
        else:
            print('Sending message to server..')
            s.send(input())
            print(s.recv(BUFSIZE))

        print()
    
    # Close the connection
    s.close()

    print('Goodbye')


if __name__ == '__main__':
    main()
