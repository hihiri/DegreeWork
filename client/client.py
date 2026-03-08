import socket
import json
import os
import time
import sys
import logic

def main():
    if len(sys.argv) != 2:
        logic.usage()
        sys.exit(1)

    cmd = sys.argv[1]
    logic.readConfig()

    method, sep, rest = cmd.partition(':')

    match method:
        case 'sendData':
            fileName = rest
            if not fileName:
                print('Usage: sendData:<filename>')
                sys.exit(1)
            filePath = os.path.join(logic.dataDir, fileName)
            if not os.path.exists(filePath):
                print('Data file not found:', filePath)
                sys.exit(1)
            with open(filePath, 'r') as f:
                payload = f.read().strip()
            
            resp = logic.communicate(('2' + payload).encode('ascii'))
            if resp and resp[0]==ord('7'):
                print('Server ack for data')
            elif resp and resp[0]==ord('6'):
                print('Server error: busy')
            else:
                print('Unexpected data response', resp)

        case 'sendConfig':
            msg = logic.formatConfigMessage()
            logic.communicate(msg)
            logic.log('Sent config')
            print('Sent config')

        case 'getStatus':
            data = logic.communicate(b'1')
            if data and data[0]==ord('5') and len(data)>=2:
                status = data[1:2].decode('ascii')
                print('Status:', status)
            else:
                print('Bad status response', data)

        case 'getResult':
            resp = logic.communicate(b'3')
            if resp and resp[0]==ord('9'):
                result = resp[1:].decode('ascii')
                print('Result:', result)
            elif resp and resp[0]==ord('8'):
                print('Server error: result not ready')
            else:
                print('Unexpected result response', resp)

        case _:
            print('Unknown command:', cmd)
            logic.usage()

if __name__=='__main__':
    main()
