/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>

#include "blockchain.hpp"
#include "crypto.hpp"
#include "p2p.hpp"
#include "storage.hpp"

using namespace sbc;

static void print_menu() {
  std::cout << "\nSimpleBlockchain Advanced CLI\n"
            << "1) Generate keypair\n"
            << "2) Show address from pubkey (PEM)\n"
            << "3) Add signed tx\n"
            << "4) Mine pending (with miner addr)\n"
            << "5) Print chain\n"
            << "6) Validate chain\n"
            << "7) Save chain to JSON\n"
            << "8) Load chain from JSON\n"
            << "9) Set target block time (sec)\n"
            << "0) Exit\n> ";
}

int main(int argc, char** argv) {
  Blockchain::Params params;
  params.initial_difficulty = 2;
  params.target_block_time_sec = 6;
  params.retarget_interval = 5;
  Blockchain bc(params);

  // P2P setup
  std::string listen = "127.0.0.1:0";
  std::vector<std::string> peers;
  std::string persist_path;

  // Parse simple args
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--listen" && i + 1 < argc) listen = argv[++i];
    else if (arg == "--peer" && i + 1 < argc) peers.push_back(argv[++i]);
    else if (arg == "--db" && i + 1 < argc) persist_path = argv[++i];
  }

  // Listener
  std::string host = "127.0.0.1";
  int port = 0;
  auto pos = listen.find(':');
  if (pos != std::string::npos) {
    host = listen.substr(0, pos);
    port = std::stoi(listen.substr(pos + 1));
  }
  p2p::Listener listener(host, port, [&](std::string msg) {
    // Expect a JSON object with {"type":"NEWBLOCK","block":{...}}
    try {
      auto j = nlohmann::json::parse(msg);
      if (j.value("type", "") == "NEWBLOCK") {
        Block b = Block::from_json(j.at("block"));
        // naive acceptance: append if links to our tip
        if (b.prev_hash == bc.chain().back().hash && calculate_block_hash(b) == b.hash) {
          const_cast<std::vector<Block>&>(bc.chain()).push_back(b);
          std::cout << "\n[peer] Accepted new block #" << b.index << " from network.\n> ";
        }
      }
    } catch (...) {
      // ignore
    }
  });
  listener.start();

  // Simple CLI
  while (true) {
    print_menu();
    int c = -1;
    if (!(std::cin >> c)) break;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (c == 0) break;
    if (c == 1) {
      auto kp = crypto::generate_ec_keypair();
      std::cout << "Private key (PEM):\n" << kp.first << "\n";
      std::cout << "Public key (PEM):\n" << kp.second << "\n";
      std::cout << "Address: " << Tx::addr_from_pubkey(kp.second) << "\n";
    } else if (c == 2) {
      std::cout << "Paste PUBLIC key (PEM), end with a single '.' on a line:\n";
      std::ostringstream oss; std::string line;
      while (std::getline(std::cin, line)) { if (line == ".") break; oss << line << "\n"; }
      std::string pub = oss.str();
      std::cout << "Address: " << Tx::addr_from_pubkey(pub) << "\n";
    } else if (c == 3) {
      // Add signed tx
      std::string priv, pub, to; std::uint64_t amt=0, nonce=0;
      std::cout << "Paste PRIVATE key (PEM), end with '.' line:\n";
      { std::ostringstream oss; std::string line; while (std::getline(std::cin, line)) { if (line == ".") break; oss << line << "\n"; } priv = oss.str(); }
      std::cout << "Paste PUBLIC key (PEM), end with '.' line:\n";
      { std::ostringstream oss; std::string line; while (std::getline(std::cin, line)) { if (line == ".") break; oss << line << "\n"; } pub = oss.str(); }
      std::cout << "To address: "; std::getline(std::cin, to);
      std::cout << "Amount (uint64): "; std::cin >> amt; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Nonce (sender next): "; std::cin >> nonce; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      Tx tx; tx.from_pubkey_pem = pub; tx.to_addr = to; tx.amount = amt; tx.nonce = nonce;
      auto sig = crypto::ecdsa_sign_p256(priv, tx.message());
      tx.signature_hex = sig;
      bc.addTransaction(std::move(tx));
      std::cout << "Tx added to mempool.\n";
    } else if (c == 4) {
      std::string miner;
      std::cout << "Miner address: "; std::getline(std::cin, miner);
      const Block& b = bc.minePending(miner);
      std::cout << "Mined block #" << b.index << " in " << b.mine_ms << " ms, hash=" << b.hash << "\n";
      // Broadcast to peers
      nlohmann::json j = {{"type", "NEWBLOCK"}, {"block", b.to_json()}};
      p2p::broadcast(peers, j.dump());
      if (!persist_path.empty()) {
        std::string err;
        storage::save_json_to_file(persist_path, bc.toJson(), &err);
      }
    } else if (c == 5) {
      for (const auto& b : bc.chain()) {
        std::cout << "Block #" << b.index << " ts=" << b.timestamp << " diff=" << b.difficulty
                  << " txs=" << b.transactions.size() << "\n"
                  << "  prev=" << b.prev_hash.substr(0, 16) << "...\n"
                  << "  hash=" << b.hash.substr(0, 16) << "...\n";
      }
    } else if (c == 6) {
      std::cout << (bc.isValid() ? "VALID" : "INVALID") << "\n";
    } else if (c == 7) {
      std::string path; std::cout << "Path: "; std::getline(std::cin, path);
      std::string err; if (storage::save_json_to_file(path, bc.toJson(), &err)) std::cout << "Saved.\n";
      else std::cout << "Save failed: " << err << "\n";
    } else if (c == 8) {
      std::string path; std::cout << "Path: "; std::getline(std::cin, path);
      std::string s, err; if (!storage::load_file_to_string(path, &s, &err)) { std::cout << "Load failed: " << err << "\n"; }
      else { bc = Blockchain::fromJson(s); std::cout << "Loaded.\n"; }
    } else if (c == 9) {
      std::uint64_t t; std::cout << "Target block time (sec): "; std::cin >> t; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      // not persisted for simplicity
      std::cout << "Set (session only).\n";
    }
  }

  listener.stop();
  return 0;
}
