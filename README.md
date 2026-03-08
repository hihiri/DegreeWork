Simple TCP server (C++) and Python client implementing the specified protocol in ProjectDetails.txt

**Build server (Windows Visual Studio Developer Command Prompt):**
```
cl /EHsc server\main.cpp /link ws2_32.lib
```

or using Solution (.sln) build it

**Or using g++ (MinGW on Windows):**
```
g++ -static -o server.exe server\main.cpp -lws2_32
```

**Build server (Linux/ARM - for FPGA SoC):**
```
g++ -o server server/main.cpp -pthread
```

The server uses cross-platform socket code with conditional compilation (`#ifdef _WIN32`) for Windows (Winsock2) and POSIX sockets (Linux/ARM).

Configure the IP adress and port on Client, and the port on Server.

Run server.exe

Then run the client with a single argument. (PowerShell or cmd in Windows):

Usage examples (from project root):
python client\client.py sendConfig
python client\client.py getStatus
python client\client.py sendData:cfd_init.txt
python client\client.py getResult

The `sendConfig` command synchronizes the client config (log, port, width, height) with the server.

The `sendData:<filename>` command reads raw bytes from `client/data/<filename>` and sends them as a binary CFD input payload to the server.

Protocol summary:
- Messages start with a single ASCII digit (message type) followed by payload digits, as described in the ProjectDetails.txt
