import socket
import json
import os
import time
import sys
import helpers

BASE = os.path.dirname(__file__)
CONFIG_PATH = os.path.join(BASE, 'config', 'config.json')
LOG_DIR = os.path.join(BASE, 'log')
LOG_PATH = os.path.join(LOG_DIR, 'client.log')
DATA_DIR = os.path.join(BASE, 'data')

def main():
    if len(sys.argv) != 2:
        helpers.usage()
        sys.exit(1)

    cmd = sys.argv[1]
    cfg = helpers.read_config()
    HOST = cfg.get('serverIP', '127.0.0.1')
    PORT = cfg.get('connectionPort', 12345)

    method, sep, rest = cmd.partition(':')  # rest is payload or filename

    match method:
        case 'sendData':
            # format sendData:filename
            fname = rest
            file_path = os.path.join(helpers.DATA_DIR, fname)
            if not os.path.exists(file_path):
                print('Data file not found:', file_path)
                sys.exit(1)
            with open(file_path,'r') as f:
                payload = f.read().strip()

            with socket.create_connection((HOST, PORT)) as s:
                helpers.send_msg(s, ('2'+payload).encode('ascii'))
                resp = helpers.recv_msg(s)
                if resp and resp[0]==ord('7'):
                    print('Server ack for data')
                elif resp and resp[0]==ord('6'):
                    print('Server error: busy')
                else:
                    print('Unexpected data response', resp)

        case 'sendConfig':
            with socket.create_connection((HOST, PORT)) as s:
                msg = helpers.format_config_message(cfg)
                helpers.send_msg(s, msg)
                helpers.log('Sent config')
                print('Sent config')

        case 'getStatus':
            with socket.create_connection((HOST, PORT)) as s:
                helpers.send_msg(s, b'1')
                data = helpers.recv_msg(s)
                if data and data[0]==ord('5') and len(data)>=2:
                    status = data[1:2].decode('ascii')
                    print('Status:', status)
                else:
                    print('Bad status response', data)

        case 'getResult':
            with socket.create_connection((HOST, PORT)) as s:
                helpers.send_msg(s, b'3')
                resp = helpers.recv_msg(s)
                if resp and resp[0]==ord('9'):
                    result = resp[1:].decode('ascii')
                    print('Result:', result)
                elif resp and resp[0]==ord('8'):
                    print('Server error: result not ready')
                else:
                    print('Unexpected result response', resp)

        case _:
            print('Unknown command:', cmd)
            helpers.usage()

if __name__=='__main__':
    main()
