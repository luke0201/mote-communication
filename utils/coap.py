#!/usr/bin/env python

import getopt
import socket
import sys

# Python3-compatibility modules
from __future__ import print_function
from __future__ import unicode_literals
from __future__ import division
from __future__ import absolute_import

from coapthon.client.helperclient import HelperClient
from coapthon.utils import parse_uri

__author__ = 'Giacomo Tanganelli'

client = None


def client_callback(response):
    print "Callback"


def client_callback_observe(response):  # pragma: no cover
    global client
    print "Callback_observe"
    check = True
    while check:
        chosen = raw_input("Stop observing? [y/N]: ")
        if chosen != "" and not (
                chosen == "n" or chosen == "N"
                or chosen == "y" or chosen == "Y"):
            print "Unrecognized choose."
            continue
        elif chosen == "y" or chosen == "Y":
            while True:
                rst = raw_input("Send RST message? [Y/n]: ")
                if rst != "" and not (
                        rst == "n" or rst == "N"
                        or rst == "y" or rst == "Y"):
                    print "Unrecognized choose."
                    continue
                elif rst == "" or rst == "y" or rst == "Y":
                    client.cancel_observing(response, True)
                else:
                    client.cancel_observing(response, False)
                check = False
                break
        else:
            break


# path: coap://[IPv6 address of mote]/[resource name]
# op: GET, OBSERVE, DELETE, POST, PUT, DISCOVER
# payload: payload
def req_coap(host, op, payload):
    if op is None:
        raise Exception("Operation must be specified")

    if host is None:
        raise Exception("IPv6 Address must be specified")

    if not path.startswith("coap://"):
        raise Exception("Path must be conform to coap://host[:port]/path")

    path = "red"
    port = 5683

    try:
        tmp = socket.gethostbyname(host)
        host = tmp
    except socket.gaierror:
        pass
    client = HelperClient(server=(host, port))
    if op == "GET":
        if path is None:
            raise Exception("Path cannot be empty for a GET request")
        response = client.get(path)
        print response.pretty_print()
        client.stop()
    elif op == "OBSERVE":
        if path is None:
            raise Exception("Path cannot be empty for a GET request")
        client.observe(path, client_callback_observe)
    elif op == "DELETE":
        if path is None:
            raise Exception("Path cannot be empty for a DELETE request")
        response = client.delete(path)
        print response.pretty_print()
        client.stop()
    elif op == "POST":
        if path is None:
            raise Exception("Path cannot be empty for a POST request")
        if payload is None:
            raise Exception("Payload cannot be empty for a POST request")
        response = client.post(path, payload)
        print response.pretty_print()
        client.stop()
    elif op == "PUT":
        if path is None:
            raise Exception("Path cannot be empty for a PUT request")
        if payload is None:
            raise Exception("Payload cannot be empty for a PUT request")
        response = client.put(path, payload)
        print response.pretty_print()
        client.stop()
    elif op == "DISCOVER":
        response = client.discover()
        print response.pretty_print()
        client.stop()
    else:
        raise Exception("Operation not recognized")

    return response
