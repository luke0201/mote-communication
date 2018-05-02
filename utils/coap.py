import sys
import socket

from coapthon.client.helperclient import HelperClient


def req_coap(host, op, payload):
    port = 5683
    path = 'red'

    client = HelperClient(server=(host, port))

    if op == 'GET':
        response = client.get(path)
        client.stop()
    elif op == 'POST':
        response = client.post(path, payload)
        client.stop()
    elif op == 'PUT':
        response = client.put(path, payload)
        client.stop()
    else:
        raise Exception('Operation not recognized')

    return response
