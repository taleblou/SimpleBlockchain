# SimpleBlockchain Suite (C++17, OpenSSL, JSON, Cross‑Platform)

This repository contains **two educational blockchain implementations** in modern C++:

1. **pro/** – a clean, modular Proof‑of‑Work blockchain with a mempool, chain validation, JSON import/export, unit tests, and GitHub Actions CI.
2. **advanced/** – an extended version that adds **Merkle root**, **dynamic difficulty retargeting**, **ECDSA P‑256 signed account transactions with nonces**, **coinbase rewards**, **JSON persistence**, a **tiny localhost P2P** broadcaster, unit tests, and CI.

> ⚠️ Educational only. Not production‑ready. No real wallet, fee market, P2P consensus, or economic/security guarantees.

---

## Features

### pro/
- Minimal PoW blockchain (OpenSSL **EVP SHA‑256**)
- Mempool → mine → append block
- Chain validation: prev‑hash linkage + difficulty check
- JSON import/export (`nlohmann/json` via FetchContent)
- CLI demo (interactive)
- **CMake**, **GoogleTest**, **GitHub Actions CI**
- Uniform code style (`.clang-format`)

### advanced/
- Block header: `prev_hash`, `merkle_root`, `difficulty`, `nonce`, `timestamp`
- **Merkle root** over transactions (duplicates last leaf if odd; empty root = `sha256("")`)
- **Retarget difficulty** by average mining time (interval and target are configurable)
- **ECDSA P‑256** signed transactions (OpenSSL) in an **account‑based** model with **nonces**
- **Coinbase** reward for the miner on each block
- **JSON** persistence (save/load)
- **Tiny P2P (localhost TCP)** to broadcast mined blocks between peers
- **CMake**, **GoogleTest**, **GitHub Actions CI**, `.clang-format`

---

## Build (All Targets)

This top‑level CMake builds **both** projects. You can also `cd` into `pro/` or `advanced/` and build them separately.

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update && sudo apt-get install -y cmake build-essential libssl-dev git
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```
Binaries:
- `build/pro/simple_blockchain_pro`
- `build/advanced/simple_blockchain_adv`

### macOS (Intel/Apple Silicon)
Install dependencies with Homebrew:
```bash
brew install cmake openssl@3 git
OPENSSL_ROOT_DIR="$(brew --prefix openssl@3)"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DOPENSSL_ROOT_DIR="$OPENSSL_ROOT_DIR" -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```
If CMake still cannot find OpenSSL, also add:
```bash
-DCMAKE_PREFIX_PATH="$(brew --prefix openssl@3)"
```

### Windows (MSVC + vcpkg recommended)
```powershell
# Developer PowerShell for Visual Studio
git clone <this repo>
cd <this repo>

# Install OpenSSL with vcpkg (once)
git clone https://github.com/microsoft/vcpkg
.cpkgootstrap-vcpkg.bat
.cpkgcpkg install openssl:x64-windows

# Configure both subprojects with the toolchain
cmake -S . -B build -A x64 `
  -DCMAKE_TOOLCHAIN_FILE="$PWD\vcpkg\scripts\buildsystems\vcpkg.cmake" `
  -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```
Binaries:
- `build\pro\Release\simple_blockchain_pro.exe`
- `build\advanced\Release\simple_blockchain_adv.exe`

If you have a standalone OpenSSL install, you can also pass:
```powershell
-DOPENSSL_ROOT_DIR="C:\OpenSSL-Win64"
```

---

## Run

### pro (minimal CLI)
```
./build/pro/simple_blockchain_pro [--difficulty N] [--load FILE]
```
Interactive menu lets you:
- Add transactions
- Mine a block from the mempool
- Print/validate the chain
- Save the chain to JSON
- Change difficulty live

### advanced (CLI + P2P)
```
./build/advanced/simple_blockchain_adv   [--listen 127.0.0.1:9001] [--peer 127.0.0.1:9002]... [--db chain.json]
```
Menu options include generating keypairs (PEM), computing address (from public key), creating **signed** transactions (ECDSA P‑256), mining with a **miner address** (coinbase), printing/validating the chain, and saving/loading JSON.

**Local P2P demo:** run two terminals:
```bash
# Node 1
./build/advanced/simple_blockchain_adv --listen 127.0.0.1:9001 --peer 127.0.0.1:9002 --db chain1.json
# Node 2
./build/advanced/simple_blockchain_adv --listen 127.0.0.1:9002 --peer 127.0.0.1:9001 --db chain2.json
```
Mining on Node 1 broadcasts the new block to Node 2, which accepts it if it links to its tip and the hash verifies.

---

## Repository Layout
```
.
├── CMakeLists.txt            # builds both subprojects
├── LICENSE                   # MIT
├── README.md                 # this file
├── .github/workflows/ci.yml  # CI for Linux
├── pro/
│   ├── CMakeLists.txt
│   ├── src/                  # core (block, blockchain, crypto, util, main)
│   └── tests/                # GoogleTest
└── advanced/
    ├── CMakeLists.txt
    ├── src/                  # crypto(EVP/ECDSA), tx/state, merkle, p2p, storage, ...
    └── tests/                # GoogleTest
```

---

## Notes & Disclaimers
- This code is intentionally **simple** for learning. It omits critical production elements:
  peer discovery, consensus/fork-choice, transaction fees/mempool policies, signature formats,
  reorg handling, chain finality, persistence guarantees, etc.
- The **address** in the advanced project is a truncated SHA‑256 of the public key PEM (demo only).
- Difficulty retargeting is simplified (interval‑based; integer difficulty = leading zero hex).
- For macOS, always pass the Homebrew OpenSSL path to CMake as shown above.

---

## Contributing
- Use `clang-format` (Google style) before sending PRs.
- Add unit tests for new logic.
- Consider educational PRs: Merkle proofs, SQLite storage, UTXO model, better P2P (handshake, longest‑chain rule, block requests).

---

## License
MIT — Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
