# Trivial File Transfer Protocol (TFTP)
![C](https://img.shields.io/badge/-00599C?style=flat-square&logo=c&logoColor=white)
![Status](https://img.shields.io/badge/status-completed-green?style=flat-square)
![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)

## Description
A **complete implementation of the Trivial File Transfer Protocol (TFTP)** using **C** with support for multiple transfer modes.

## Installation
Clone the repository:
```bash
git clone https://github.com/ArjunVasavan/trivial_file_transfer_protocol
cd trivial_file_transfer_protocol
```

## Compilation
```bash
make
make clean
```

## Usage
Run server:
```bash
./tftp_server
```
Server listens on port **6969**.

Run client:
```bash
./tftp_client
```

### Client Menu
1. **Connect** - Connect to server (IP + Port 6969)
2. **Put** - Upload file to server
3. **Get** - Download file from server
4. **Mode** - Change transfer mode (Default/Octet/NetASCII)
5. **Exit** - Disconnect and quit

### File Locations
- **PUT (Upload)**: Files read from `src/client/` → saved to `src/server/`
- **GET (Download)**: Files read from `src/server/` → saved to `src/client/`

## Author
Arjun Vasavan

## License
This project is licensed under the [MIT License](./LICENSE).

> © 2025 Arjun Vasavan
