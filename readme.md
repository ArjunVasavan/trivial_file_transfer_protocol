# TFTP — Trivial File Transfer Protocol in C

A UDP-based TFTP client-server implementation written in C from scratch, without any networking libraries. The goal was to understand how file transfer actually works at the socket level — packet design, binary serialization, network byte order, and building reliability on top of an unreliable transport.

---

## Project Structure

```
.
├── include/
│   └── tftp.h              # Shared packet struct, opcodes, transfer modes
├── src/
│   ├── client/
│   │   ├── tftp_client.c   # Client CLI, connect/put/get/disconnect logic
│   │   ├── tftp_client.h
│   │   └── (files to send/receive go here)
│   ├── server/
│   │   ├── tftp_server.c   # Binds to port, dispatches RRQ/WRQ
│   │   └── (received files are stored here)
│   └── common/
│       ├── tftp.c          # send_file() and receive_file() — used by both sides
│       └── validation.c    # IP address, port, and file permission checks
└── docs/
    ├── HK_TFTP/            # Peer implementation (reference / comparison)
    ├── TFTP_shwe/          # Peer implementation (reference / comparison)
    └── TFTP_skeleton_code/ # Original skeleton provided at the start
```

> `docs/` contains three other implementations from the same exercise — kept for reference and comparison, not part of the main build.

---

## Protocol Design

Every packet starts with a 2-byte opcode. What follows depends on the opcode:

```
RRQ / WRQ:  [ opcode : 2 ][ filename : n ][ \0 ][ mode : 2 ]
DATA:        [ opcode : 2 ][ block_number : 2 ][ data : 0–512 ]
ACK:         [ opcode : 2 ][ block_number : 2 ]
```

The struct uses `#pragma pack(1)` to disable compiler padding. Without it, the compiler inserts alignment bytes into the struct and the receiver reads fields at the wrong offsets — silent data corruption that's hard to debug.

**Opcodes:**

| Value | Name | Direction |
|-------|------|-----------|
| 1 | RRQ | Client → Server (download request) |
| 2 | WRQ | Client → Server (upload request) |
| 3 | DATA | Sender → Receiver |
| 4 | ACK | Receiver → Sender |
| 5 | ERROR | Either direction |
| 6 | DISCONNECT | Client → Server (custom extension) |

---

## Transfer Modes

| Mode | Chunk Size | Description |
|------|-----------|-------------|
| Default | 512 bytes | Standard TFTP block size |
| Octet | 1 byte | Byte-by-byte, useful for debugging |
| NetASCII | 512 bytes | Translates `\n` → `\r\n` on send, `\r\n` → `\n` on receive |

---

## Reliability on UDP

TFTP uses UDP, which gives no delivery guarantees. Reliability is implemented manually:

- Every DATA packet has a block number
- The receiver sends back an ACK with the same block number
- If the sender receives an ACK with a mismatched block number, it retransmits
- A short packet (less than chunk size) signals end of transfer

This is essentially a stop-and-wait protocol — one block in flight at a time.

---

## Build

```bash
# Server
gcc src/server/tftp_server.c src/common/tftp.c src/common/validation.c -Iinclude -o server

# Client
gcc src/client/tftp_client.c src/common/tftp.c src/common/validation.c -Iinclude -o client
```

---

## Run

```bash
# Terminal 1
./server
# TFTP Server listening on port 6969...

# Terminal 2
./client
```

Client prompts:
```
1) Connect   — enter server IP and port (must be done first)
2) Put       — upload a file to the server
3) Get       — download a file from the server
4) Mode      — switch transfer mode
5) Exit      — sends a DISCONNECT packet, then closes socket
```

---

## Problems Worth Noting

**Struct padding corrupting packets**

Early on, transfers were producing garbage on the receiver. The root cause was struct alignment padding — the compiler was inserting hidden bytes between fields to align them in memory. When `sendto()` serialized the struct, those padding bytes went over the wire, and the receiver read fields at the wrong offsets. `#pragma pack(1)` forces byte-level packing and fixed it.

**Union memory layout and exact-length sends**

The packet body is a union — all variants (request, data, ack) share the same memory starting at the same address. Only the fields you write are meaningful; everything after is uninitialized. By passing exactly `2 + 2 + data_length` to `sendto()` instead of `sizeof(packet)`, only the intended bytes are transmitted. This is why packet length is computed explicitly rather than using sizeof.

**Encoding the mode after a variable-length filename**

The request struct holds a filename and a mode. Since the filename is variable-length (null-terminated), the mode can't sit at a fixed struct offset. The solution: write the mode as a `uint16_t` immediately after the filename's null terminator using `memcpy`, and read it back the same way on the server. A full write-up of this is in the [commit comment here](https://github.com/ArjunVasavan/trivial_file_transfer_protocol/commit/b5783f441e1808450b8bce0231c617dead6bf03f#commitcomment-175840992).

**512-byte-aligned file edge case**

TFTP uses a short final packet (less than chunk size) to signal end of transfer. If a file's size is an exact multiple of 512 bytes, the last `read()` returns 512 — which looks like a full block, not a terminator. The sender must follow up with a zero-byte DATA packet so the receiver knows to stop. Missing this causes the receiver to hang waiting for more data.

---

## Known Limitations

- **Single-threaded** — the server handles one client at a time; a second connection blocks until the first finishes
- **No socket timeout** — if a packet is lost and never retransmitted, both sides hang indefinitely (`SO_RCVTIMEO` not yet set via `setsockopt`)
- **No ERROR packets to client** — if the server can't open a file, it silently fails rather than sending an ERROR opcode back

---

## What This Project Covers

- UDP socket programming (`socket`, `bind`, `sendto`, `recvfrom`)
- Binary packet serialization without external libraries
- Network byte order (`htons`, `ntohs`, `htobe64`, `be64toh`)
- Manual reliability over an unreliable transport
- C struct layout, union memory model, and `#pragma pack`
- Input validation (`inet_pton` for IP, port range checks, `access()` for file permissions)
