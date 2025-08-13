/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "blockchain.hpp"
#include "block.hpp"
#include "crypto.hpp"
#include "util.hpp"

#include <stdexcept>
#include <string>
#include <utility>

namespace sbc {

// Forward decls from block.cpp
std::string calculate_hash(const Block& b);
void mine_block(Block& b, int difficulty);

Blockchain::Blockchain(int difficulty) : difficulty_(difficulty) {
  chain_.push_back(makeGenesis());
}

Block Blockchain::makeGenesis() {
  Block g{};
  g.index = 0;
  g.timestamp = util::now_iso8601();
  g.transactions = {"Genesis Block"};
  g.prev_hash = "0";
  g.nonce = 0;
  g.hash = calculate_hash(g);
  return g;
}

void Blockchain::addTransaction(std::string tx) {
  if (tx.empty()) throw std::invalid_argument("Transaction cannot be empty");
  mempool_.push_back(std::move(tx));
}

const Block& Blockchain::minePending() {
  std::vector<std::string> txs;
  txs.swap(mempool_);  // move all pending txs into the new block

  Block b;
  b.index = chain_.size();
  b.timestamp = util::now_iso8601();
  b.transactions = std::move(txs);
  b.prev_hash = chain_.back().hash;

  mine_block(b, difficulty_);
  chain_.push_back(std::move(b));
  return chain_.back();
}

bool Blockchain::validateLink(std::size_t i) const {
  if (i == 0 || i >= chain_.size()) return true;
  const Block& prev = chain_[i - 1];
  const Block& cur = chain_[i];
  if (cur.prev_hash != prev.hash) return false;
  // Recompute hash and check difficulty
  Block tmp = cur;
  std::string recomputed = calculate_hash(tmp);
  if (recomputed != cur.hash) return false;
  // Difficulty check: leading zeros
  for (int k = 0; k < difficulty_; ++k) {
    if (k >= (int)cur.hash.size() || cur.hash[k] != '0') return false;
  }
  return true;
}

bool Blockchain::isValid() const {
  for (std::size_t i = 1; i < chain_.size(); ++i) {
    if (!validateLink(i)) return false;
  }
  return true;
}

std::string Blockchain::toJsonString(int indent) const {
  nlohmann::json j;
  j["difficulty"] = difficulty_;
  j["chain"] = nlohmann::json::array();
  for (const auto& b : chain_) {
    j["chain"].push_back(b.to_json());
  }
  return j.dump(indent);
}

Blockchain Blockchain::fromJsonString(std::string_view data) {
  auto j = nlohmann::json::parse(data.begin(), data.end());
  Blockchain bc{j.at("difficulty").get<int>()};
  bc.chain_.clear();
  for (const auto& jb : j.at("chain")) {
    bc.chain_.push_back(Block::from_json(jb));
  }
  return bc;
}

}  // namespace sbc
