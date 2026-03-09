# DegreeWork

Simple TCP server (C++) and Python client implementing the protocol specified in `ProjectDetails.md`.

## Build the Server

### Windows (Visual Studio Developer Command Prompt)
```
cl /EHsc server\main.cpp /link ws2_32.lib
```

### Windows (Visual Studio Solution)
Open `ServerSolution.sln` and build the `server` project.

### Windows (MinGW g++)
```
g++ -static -o server.exe server\main.cpp -lws2_32
```

### Linux/ARM (FPGA SoC)
```
g++ -o server server/main.cpp -pthread
```

## Notes

The server uses cross-platform socket code with conditional compilation (`#ifdef _WIN32`) for Windows (Winsock2) and POSIX sockets (Linux/ARM).

Configure the IP address and port on the client, and the port on the server.

## Run

1. Start the server (`server.exe` on Windows).
2. Run the client with a single argument (PowerShell or CMD on Windows).

Usage examples (from project root):

```
python client\client.py sendConfig
python client\client.py getStatus
python client\client.py sendData:cfd_init.txt
python client\client.py getResult
```
