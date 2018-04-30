import sys
import socket


def main():
    host = ''
    port = 4444
    bufsize = 1024

    print('Team 4 (client)', end='\n\n')

    # Connect to server
    print('Connecting to server..')
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    print(s.recv(bufsize), end='\n\n')

    # Interactive terminal-like..
    while True:
        oper = input('> ')

        if oper == 'help':  # Print help message
            print('Message format: <req_type> [<payload>] [to <mote_no>]')

        else:  # Communicate
            print('Sending message to server..')
            s.send(bytes(oper, 'utf-8'))
            print(s.recv(bufsize))

        print()

        if oper == 'exit':
            break

    # Close the connection
    s.close()

    print('Goodbye')


if __name__ == '__main__':
    main()
