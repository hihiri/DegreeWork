Simple TCP server (C++) and Python client implementing the specified protocol in ProjectDetails.txt

Build server (Windows Visual Studio Developer Command Prompt):
cl /EHsc server\main.cpp /link ws2_32.lib

or using Solution (.sln) build it

Or using g++ (MinGW):
g++ -static -o server.exe server\main.cpp -lws2_32

Configure the IP adress and port on Client, and the port on Server.

Run server.exe

Then run the client with a single argument. (PowerShell or cmd in Windows):

Usage examples (from project root):
python client\client.py sendConfig
python client\client.py getStatus
python client\client.py sendData:1.txt
python client\client.py getResult

The `sendConfig` command synchronizes the client config with the server `client/config/config.json`.

The `sendData:<filename>` command reads the file from `client/data/<filename>` and sends its contents as the payload.

Protocol summary:
- Messages start with a single ASCII digit (message type) followed by payload digits, as described in the ProjectDetails.txt
