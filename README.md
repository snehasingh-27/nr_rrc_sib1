# 5G NR SIB1 ASN.1 TCP Encoder/Decoder

Complete implementation of 5G NR System Information Block Type 1 (SIB1) encoding, transmission, and decoding using ASN.1 PER encoding over TCP.

## 🚀 Quick Start
```bash
cd nr_sib1
make clean && make

# Terminal 1 - Start server
./decoder

# Terminal 2 - Run client
./encoder
```

## 📁 Repository Structure
```
nr_rrc_sib1/
├── nr_sib1/              # Main project directory
│   ├── 38331-15.asn      # 3GPP TS 38.331 ASN.1 definitions
│   ├── encoder.c         # TCP client: builds & sends SIB1
│   ├── decoder.c         # TCP server: receives & decodes SIB1
│   ├── Makefile          # Build configuration
│   └── *.c, *.h          # ASN.1 generated files (~1600 files)
└── README.md             # This file
```

## 🏗️ What This Does

1. **Encoder (Client)**: Builds a 5G NR SIB1 message, encodes it using PER, sends via TCP
2. **Decoder (Server)**: Receives encoded SIB1, decodes it, displays all fields

## 📋 Requirements

- **OS**: Linux/Unix
- **Compiler**: GCC 7.5+
- **Tools**: make, asn1c (for regenerating files)

## 🔧 Build Instructions
```bash
cd nr_sib1

# Build everything
make

# Clean and rebuild
make clean
make
```

## ▶️ Usage

### Start the Decoder (Server)
```bash
./decoder
```

Expected output:
```
[TCP SERVER] Starting on port 8888...
[TCP SERVER] Waiting for client...
```

### Run the Encoder (Client)
```bash
./encoder
```

Expected output:
```
[TCP CLIENT] Building SIB1...
[TCP CLIENT] PER encoding...
[TCP CLIENT] Connecting to 127.0.0.1:8888...
[TCP CLIENT] Sending 47 bytes...
[TCP CLIENT] Success!
```

### Decoder Output After Reception
```
[TCP SERVER] Client connected
[TCP SERVER] Received 47 bytes
[TCP SERVER] Decoding...

╔════════════════════════════════════════╗
║       DECODED SIB1 CONTENT             ║
╚════════════════════════════════════════╝

PLMN Identity:
  MCC: 001
  MNC: 01

Tracking Area Code: 0x000001
Cell Identity: 123456789
Cell Barred: notBarred
```

## 📚 Technical Details

- **Protocol**: 3GPP TS 38.331 (NR RRC)
- **Encoding**: PER (Packed Encoding Rules)
- **Transport**: TCP/IP
- **Message Type**: BCCH-DL-SCH-Message carrying SIB1

## 🔮 Features

- ✅ Complete SIB1 message construction
- ✅ PER encoding/decoding
- ✅ TCP client-server architecture
- ✅ Field-by-field SIB1 display
- ✅ Error handling and validation

## 📖 References

- [3GPP TS 38.331 - NR RRC](https://www.3gpp.org/DynaReport/38331.htm)
- [ASN.1 Compiler (asn1c)](https://github.com/vlm/asn1c)
- [ITU-T X.691 - PER Encoding](https://www.itu.int/rec/T-REC-X.691)

## 🤝 Contributing

Pull requests are welcome! For major changes, please open an issue first.

## 📝 License

MIT License

## 👤 Author

**Sneha Singh**
- GitHub: [@snehasingh-27](https://github.com/snehasingh-27)

---

⭐ **Star this repo if it helped you!**
