import os
import time
import json
import socket

BASE = os.path.dirname(__file__)
configPath = os.path.join(BASE, 'config', 'config.json')
logDir = os.path.join(BASE, 'log')
logPath = os.path.join(logDir, 'client.log')
dataDir = os.path.join(BASE, 'data')

ip = None
port = None
logging = False

def initConfig(cfg):
    global ip, port, logging
    ip = cfg.get('serverIP', '127.0.0.1')
    port = cfg.get('port', 12345)
    logging = cfg.get('log', False)

def now():
    return time.strftime('%Y-%m-%d %H:%M:%S')

def readConfig():
    with open(configPath,'r') as f:
        return json.load(f)

def log(msg):
    if not os.path.exists(logDir): os.makedirs(logDir)
    if logging:
        with open(logPath,'a') as f:
            f.write(f"{now()} {msg}\n")

def sendMsg(sock, data: bytes):
    sock.sendall(data)
    if logging:
        log('tx: ' + ' '.join(f"{b:02x}" for b in data))

def recvMsg(sock, bufsize=1024):
    data = sock.recv(bufsize)
    if logging:
        log('rx: ' + ' '.join(f"{b:02x}" for b in data))
    return data

def communicate(message):
    with socket.create_connection((ip, port)) as s:
        sendMsg(s, message)
        return recvMsg(s)

def formatConfigMessage(cfg):
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
