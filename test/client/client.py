import unittest
from unittest.mock import MagicMock, patch
import struct

import sys
sys.path.append('..')
import client

ALLOWED_MSGS = [
    "GetStatus", "GetTaskQue", "QueTask", "RemoveTask", "EmptyQue", "PutConfig", "EndCurrentTask"
]

class TestClient(unittest.TestCase):
    def test_send_msg(self):
        sock = MagicMock()
        msg = "test"
        client.send_msg(sock, msg)
        expected = struct.pack('>I', 4) + b'test'
        sock.sendall.assert_called_once_with(expected)

    def test_recv_msg(self):
        sock = MagicMock()
        msg = "hello"
        msg_bytes = msg.encode('utf-8')
        sock.recv.side_effect = [
            struct.pack('>I', len(msg_bytes)),
            msg_bytes
        ]
        result = client.recv_msg(sock)
        self.assertEqual(result, msg)

    def test_recv_msg_none(self):
        sock = MagicMock()
        sock.recv.return_value = b''
        result = client.recv_msg(sock)
        self.assertIsNone(result)

    def test_allowed_messages(self):
        sock = MagicMock()
        for msg in ALLOWED_MSGS:
            client.send_msg(sock, msg)
            expected = struct.pack('>I', len(msg.encode('utf-8'))) + msg.encode('utf-8')
            sock.sendall.assert_called_with(expected)
        sock.sendall.reset_mock()

    def test_disallowed_message(self):
        sock = MagicMock()
        disallowed = "NotAllowed"
        client.send_msg(sock, disallowed)
        expected = struct.pack('>I', len(disallowed.encode('utf-8'))) + disallowed.encode('utf-8')
        sock.sendall.assert_called_with(expected)

if __name__ == '__main__':
    unittest.main()
