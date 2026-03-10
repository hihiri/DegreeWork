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
            
            msg = logic.formatDataMessageFromFile(filePath)
            resp = logic.communicate(msg)
            if resp and resp[:1] == logic.msg_byte(logic.MessageType.DATA_ACK):
                print('Server ack for data')
            elif resp and resp[:1] == logic.msg_byte(logic.MessageType.BUSY_ERROR):
                print('Server error: busy')
            else:
                print('Unexpected data response', resp)

        case 'sendConfig':
            msg = logic.formatConfigMessage()
            logic.communicate(msg)
            logic.log('Sent config')
            print('Sent config')

        case 'getStatus':
            data = logic.communicate(logic.msg_byte(logic.MessageType.GET_STATUS))
            if data and data[:1] == logic.msg_byte(logic.MessageType.STATUS_RESPONSE) and len(data)>=2:
                status = data[1:2].decode('ascii')
                print('Status:', status)
            else:
                print('Bad status response', data)

        case 'getResult':
            resp = logic.communicate(logic.msg_byte(logic.MessageType.GET_RESULT))
            if resp and resp[:1] == logic.msg_byte(logic.MessageType.RESULT_RESPONSE):
                result = resp[1:].decode('ascii')
                print('Result:', result)
            elif resp and resp[:1] == logic.msg_byte(logic.MessageType.RESULT_NOT_READY_ERROR):
                reason = resp[1:].decode('ascii', errors='replace').strip()
                if reason:
                    print('Server error:', reason)
                else:
                    print('Server error: result not ready')
            else:
                print('Unexpected result response', resp)

        case _:
            print('Unknown command:', cmd)
            logic.usage()

if __name__=='__main__':
    main()
