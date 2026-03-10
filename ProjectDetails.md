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
- Messages use ASCII-encoded headers and text fields.
- Exception: in `SendData`, the payload section is raw binary bytes.

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

## Status States

- `0` = `Idle`
- `1` = `Computing`
- `2` = `Done` (result is ready)

Current mock behavior:

- After a valid `SendData`, status is set to `Computing`.
- After 5 seconds, status transitions to `Done`. (mock logic, later controlled by the kernel computation)
- After a successful `GetResult`, status is reset to `Idle`.

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

- Accepts data only when status is `Idle` (`0`).
- If status is not `Idle`, sends error response and does nothing else.
- On successful receive, stores payload, sets status to `Computing` (`1`), and starts a 5-second mock timer. (to be changed later)

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

- If status is `Idle`, responds with `8noInput`.
- If status is `Computing`, responds with `8computing`.
- If status is `Done`, responds with result and resets status to `Idle`.

Responses:

- Error: `8<reason>` where `<reason>` is currently `noInput` or `computing`
- Result: `9xxxxxxxxxxxxx` where `x...` is the result payload

Note:
Current result value is a placeholder (`2026`).