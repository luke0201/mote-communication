import sys
import socket

HOST = '0.0.0.0'
PORT = 4444
BUFSIZE = 1024


def main():
    print('Team 4 (client)', end='\n\n')

    # Connect to server
    print('Connecting to server..')
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    print(s.recv(BUFSIZE), end='\n\n')

    # Interactive terminal-like..
    while True:
        oper = input('> ')

        if oper == 'help': # Print help message
            print('Message format: <req_type> [<payload>] [to <mote_no>]')

        else: # Communicate
            print('Sending message to server..')
            s.send(bytes(oper, 'utf-8'))
            print(s.recv(BUFSIZE))

        print()

        if oper == 'exit': break
    
    # Close the connection
    s.close()

    print('Goodbye')


if __name__ == '__main__':
    main()
