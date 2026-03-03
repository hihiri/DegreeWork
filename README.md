Simple TCP server (C++) and Python client implementing the specified protocol in ProjectDetails.txt

Build server (Windows Visual Studio Developer Command Prompt):
cl /EHsc server\main.cpp /link ws2_32.lib

or using Solution (.sln) build it (main.exe in this case)
Or using g++ (MinGW):
g++ -static -o server.exe server\main.cpp -lws2_32

Run server.exe (it listens on port 12345). Then run the client with a single argument:

Usage examples (from project root):
In Windows (PowerShell or cmd):
python client\client.py sendConfig
python client\client.py getStatus
python client\client.py sendData:1.txt
python client\client.py getResult

The `sendData:<filename>` form reads the file from `client/data/<filename>` and sends its contents as the payload.

Client config notes:
- `serverIP`: the IP address the client will connect to (not sent to the server). Keep it in `client/config/config.json`.

Protocol summary:
- Messages start with a single ASCII digit (message type) followed by payload digits as described in the ProjectDetails.txt
