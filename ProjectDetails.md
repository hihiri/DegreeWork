# Project Details

Documentation Overleaf link:
`https://www.overleaf.com/project/6295c413cfd1dda357467b5d`

## Overview

- C++ server and Python client.
- The server does not need to support multiple clients.
- Communication uses TCP/IP.
- Both client and server have configuration and logging.

## Logging And Encoding

- Message traffic is logged on both sides when the corresponding config boolean is `true`.
- Messages are ASCII/text encoded.
- Exception: the `SendData` payload is binary serialized.

## Configuration Format

Format: JSON

Fields:

- `log`: `boolean`
- `port`: `int`
- `cfdConfig`: object with CFD dimensions (`width`, `height`)

## Protocol Summary

- Each message starts with a single ASCII digit (`messageType`) followed by message content.
- General conceptual format: `dxxxxxxxx...`
  - `d`: single-digit message type
  - `x...`: message content (length depends on message type)

## Messages

### 0 - SendConfig

Purpose:
Client sends current config to server.

Server behavior on receive:

- Logs receipt (if logging is enabled).
- Updates server config with received values.

Format:
`0|log|width|height`

Example:
`0|1|4096|32`

Parameters:

- `log`: logging flag (`0` or `1`)
- `width`, `height`: grid dimensions for CFD simulation

### 1 - GetStatus

Purpose:
Client asks for current computation status.

Request format:
`1`

Response format:
`5s`

Notes:

- `s` is the status value.
- Current status is an internal integer; default is `0`.

### 2 - SendData

Purpose:
Client sends CFD input payload from a file to the server.

Client behavior:

- Reads bytes from a file in `client/data/`.

Server behavior on receive:

- If status is not `0`, sends error response and does nothing else.
- Otherwise stores the binary payload and acknowledges receipt.

Format:
`2|payloadSize|<binary payload>`

Example:
`2|131072|<131072 raw bytes>`

Parameters:

- `payloadSize`: number of payload bytes to read after the second delimiter
- `binary payload`: raw CFD input bytes for later FPGA buffer handoff

Responses:

- Error: `6`
- Acknowledge: `7`

Note:
At this stage, `cfd_init.txt` is a placeholder and has no relevant data.

### 3 - GetResult

Purpose:
Client asks for the computed result.

Request format:
`3`

Server behavior:

- If status is not `done`, sends error response.
- For current debug behavior, status `1` is treated as `done`.
- If done, sends result value.

Responses:

- Error: `8`
- Result: `9xxxxxxxxxxxxx` where `x...` is the result payload

Note:
Current result value is a placeholder (`2026`).