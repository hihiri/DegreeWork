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
cfdConfig = None

def readConfig():
    global ip, port, logging, cfdConfig
    with open(configPath,'r') as f:
        cfg = json.load(f)
    ip = cfg.get('serverIP', '127.0.0.1')
    port = cfg.get('port', 12345)
    logging = cfg.get('log', False)
    cfdConfig = cfg.get('cfdConfig', {})

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
    b = '1' if logging else '0'
    
    width = cfdConfig.get('width', 4096)
    height = cfdConfig.get('height', 32)
    
    # Format: 0|log|width|height
    s = f"0|{b}|{width}|{height}"
    return s.encode('ascii')

def usage():
    print('Usage: client.py <command>')
    print('Commands (single argument):')
    print('  sendConfig           Send current client config to server')
    print('  getStatus            Request server status')
    print('  sendData:<filename>  Send data from client/data/<filename>')
    print('  getResult            Request computation result')
