/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include <fstream>
#include <iostream>
#include <limits>
#include <string>

#include "blockchain.hpp"

using namespace sbc;

static void print_menu() {
  std::cout << "\nSimpleBlockchain CLI\n"
            << "1) Add transaction\n"
            << "2) Mine pending transactions\n"
            << "3) Print chain\n"
            << "4) Validate chain\n"
            << "5) Save to JSON file\n"
            << "6) Set difficulty\n"
            << "0) Exit\n> ";
}

static void print_block(const Block& b) {
  std::cout << "Block #" << b.index << "\n"
            << "  timestamp : " << b.timestamp << "\n"
            << "  prev_hash : " << b.prev_hash << "\n"
            << "  hash      : " << b.hash << "\n"
            << "  nonce     : " << b.nonce << "\n"
            << "  txs(" << b.transactions.size() << "):\n";
  for (const auto& tx : b.transactions) {
    std::cout << "    - " << tx << "\n";
  }
}

int main(int argc, char** argv) {
  Blockchain bc{/*difficulty=*/3};

  // Optional args: --difficulty N, --load FILE
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--difficulty" && i + 1 < argc) {
      bc.setDifficulty(std::stoi(argv[++i]));
    } else if (arg == "--load" && i + 1 < argc) {
      std::ifstream in(argv[++i]);
      if (!in) {
        std::cerr << "Failed to open file.\n";
        return 1;
      }
      std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
      bc = Blockchain::fromJsonString(content);
      std::cout << "Loaded chain with " << bc.chain().size() << " blocks.\n";
    } else {
      std::cerr << "Unknown or incomplete argument: " << arg << "\n";
      return 1;
    }
  }

  while (true) {
    print_menu();
    int choice = -1;
    if (!(std::cin >> choice)) return 0;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (choice == 0) break;
    switch (choice) {
      case 1: {
        std::cout << "Enter transaction text: ";
        std::string tx;
        std::getline(std::cin, tx);
        try {
          bc.addTransaction(std::move(tx));
          std::cout << "Added. Mempool size: " << bc.mempoolSize() << "\n";
        } catch (const std::exception& e) {
          std::cerr << "Error: " << e.what() << "\n";
        }
        break;
      }
      case 2: {
        const auto& b = bc.minePending();
        std::cout << "Mined new block with " << b.transactions.size() << " tx(s).\n";
        print_block(b);
        break;
      }
      case 3: {
        for (const auto& b : bc.chain()) print_block(b);
        break;
      }
      case 4: {
        std::cout << (bc.isValid() ? "Chain is VALID" : "Chain is INVALID") << "\n";
        break;
      }
      case 5: {
        std::cout << "File path to save (e.g., chain.json): ";
        std::string path;
        std::getline(std::cin, path);
        std::ofstream out(path);
        out << bc.toJsonString(2);
        std::cout << "Saved.\n";
        break;
      }
      case 6: {
        std::cout << "New difficulty (integer >= 0): ";
        int d;
        if (std::cin >> d) {
          bc.setDifficulty(d);
          std::cout << "Difficulty set to " << d << ".\n";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        break;
      }
      default:
        std::cout << "Unknown option.\n";
    }
  }
  return 0;
}
