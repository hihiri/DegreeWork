import os
import time
import json

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
