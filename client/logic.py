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
sizeX = None
sizeY = None
sizeZ = None

def readConfig():
    global ip, port, logging, sizeX, sizeY, sizeZ
    with open(configPath,'r') as f:
        cfg = json.load(f)
    ip = cfg.get('serverIP', '127.0.0.1')
    port = cfg.get('port', 12345)
    logging = cfg.get('log', False)
    sizeX = cfg.get('SizeX', 0)
    sizeY = cfg.get('SizeY', 0)
    sizeZ = cfg.get('SizeZ', 0)

def now():
    return time.strftime('%Y-%m-%d %H:%M:%S')

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

def formatConfigMessage():
    sx = str(sizeX).zfill(3)
    sy = str(sizeY).zfill(3)
    sz = str(sizeZ).zfill(3)
    b = '1' if logging else '0'
    s = '0' + sx + sy + sz + b
    return s.encode('ascii')

def usage():
    print('Usage: client.py <command>')
    print('Commands (single argument):')
    print('  sendConfig           Send current client config to server')
    print('  getStatus            Request server status')
    print('  sendData:<filename>  Send data read from client/data/<filename>')
    print('  getResult            Request computation result')
