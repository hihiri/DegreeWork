import socket
import struct
import json
import os

DEFAULT_MSGS = ["GetStatus", "PutConfig"]

def load_allowed_msgs(config_path="c:/Users/oem user/communicator/config.json"):
    if not os.path.exists(config_path):
        return DEFAULT_MSGS
    with open(config_path, "r") as f:
        cfg = json.load(f)
        msgs = cfg.get("allowed_message_types", [])
        return DEFAULT_MSGS + msgs

def send_msg(sock, msg):
    data = msg.encode('utf-8')
    sock.sendall(struct.pack('>I', len(data)) + data)

def recv_msg(sock):
    raw_len = sock.recv(4)
    if not raw_len:
        return None
    msg_len = struct.unpack('>I', raw_len)[0]
    data = b''
    while len(data) < msg_len:
        more = sock.recv(msg_len - len(data))
        if not more:
            return None
        data += more
    return data.decode('utf-8')

if __name__ == "__main__":
    allowed_msgs = load_allowed_msgs()
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 12345))
    print("Allowed messages:", allowed_msgs)
    msg = input("Enter message type to send: ")
    if msg not in allowed_msgs:
        print("Invalid message type.")
    else:
        send_msg(sock, msg)
        reply = recv_msg(sock)
        print("Server replied:", reply)
    sock.close()
