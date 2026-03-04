import socket
import json
import os
import time
import sys

BASE = os.path.dirname(__file__)
CONFIG_PATH = os.path.join(BASE, 'config', 'config.json')
LOG_DIR = os.path.join(BASE, 'log')
LOG_PATH = os.path.join(LOG_DIR, 'client.log')
DATA_DIR = os.path.join(BASE, 'data')

def now():
    return time.strftime('%Y-%m-%d %H:%M:%S')

def read_config():
    with open(CONFIG_PATH,'r') as f:
        return json.load(f)

def log(msg):
    cfg = read_config()
    if not os.path.exists(LOG_DIR): os.makedirs(LOG_DIR)
    if cfg.get('log', False):
        with open(LOG_PATH,'a') as f:
            f.write(f"{now()} {msg}\n")

def send_msg(sock, data: bytes):
    sock.sendall(data)
    cfg = read_config()
    if cfg.get('log', False):
        log('tx: ' + ' '.join(f"{b:02x}" for b in data))

def recv_msg(sock, bufsize=1024):
    data = sock.recv(bufsize)
    cfg = read_config()
    if cfg.get('log', False):
        log('rx: ' + ' '.join(f"{b:02x}" for b in data))
    return data

def format_config_message(cfg):
    sx = str(cfg['SizeX']).zfill(3)
    sy = str(cfg['SizeY']).zfill(3)
    sz = str(cfg['SizeZ']).zfill(3)
    b = '1' if cfg.get('log',False) else '0'
    s = '0' + sx + sy + sz + b
    return s.encode('ascii')

def usage():
    print('Usage: client.py <command>')
    print('Commands (single argument):')
    print('  sendConfig           Send current client config to server')
    print('  getStatus            Request server status')
    print('  sendData:<filename>  Send data read from client/data/<filename>')
    print('  getResult            Request computation result')

def main():
    if len(sys.argv) != 2:
        usage()
        sys.exit(1)

    cmd = sys.argv[1]
    cfg = read_config()
    HOST = cfg.get('serverIP', '127.0.0.1')
    PORT = cfg.get('connectionPort', 12345)

    if cmd.startswith('sendData:'):
        # format sendData:filename
        _, fname = cmd.split(':',1)
        file_path = os.path.join(DATA_DIR, fname)
        if not os.path.exists(file_path):
            print('Data file not found:', file_path)
            sys.exit(1)
        with open(file_path,'r') as f:
            payload = f.read().strip()

        with socket.create_connection((HOST, PORT)) as s:
            send_msg(s, ('2'+payload).encode('ascii'))
            resp = recv_msg(s)
            if resp and resp[0]==ord('7'):
                print('Server ack for data')
            elif resp and resp[0]==ord('6'):
                print('Server error: busy')
            else:
                print('Unexpected data response', resp)

    elif cmd == 'sendConfig':
        with socket.create_connection((HOST, PORT)) as s:
            msg = format_config_message(cfg)
            send_msg(s, msg)
            log('Sent config')
            print('Sent config')

    elif cmd == 'getStatus':
        with socket.create_connection((HOST, PORT)) as s:
            send_msg(s, b'1')
            data = recv_msg(s)
            if data and data[0]==ord('5') and len(data)>=2:
                status = data[1:2].decode('ascii')
                print('Status:', status)
            else:
                print('Bad status response', data)

    elif cmd == 'getResult':
        with socket.create_connection((HOST, PORT)) as s:
            send_msg(s, b'3')
            resp = recv_msg(s)
            if resp and resp[0]==ord('9'):
                result = resp[1:].decode('ascii')
                print('Result:', result)
            elif resp and resp[0]==ord('8'):
                print('Server error: result not ready')
            else:
                print('Unexpected result response', resp)

    else:
        print('Unknown command:', cmd)
        usage()

if __name__=='__main__':
    main()
