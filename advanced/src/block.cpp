/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "block.hpp"
#include "crypto.hpp"
#include "merkle.hpp"
#include "util.hpp"

#include <chrono>
#include <sstream>

namespace sbc {

nlohmann::json Block::to_json() const {
  nlohmann::json j;
  j["index"] = index;
  j["timestamp"] = timestamp;
  j["prev_hash"] = prev_hash;
  j["merkle_root"] = merkle_root;
  j["hash"] = hash;
  j["nonce"] = nonce;
  j["difficulty"] = difficulty;
  j["mine_ms"] = mine_ms;
  j["transactions"] = nlohmann::json::array();
  for (const auto& t : transactions) j["transactions"].push_back(t.to_json());
  return j;
}

Block Block::from_json(const nlohmann::json& j) {
  Block b;
  b.index = j.at("index").get<std::uint64_t>();
  b.timestamp = j.at("timestamp").get<std::string>();
  b.prev_hash = j.at("prev_hash").get<std::string>();
  b.merkle_root = j.at("merkle_root").get<std::string>();
  b.hash = j.at("hash").get<std::string>();
  b.nonce = j.at("nonce").get<std::uint64_t>();
  b.difficulty = j.at("difficulty").get<int>();
  b.mine_ms = j.value("mine_ms", 0ull);
  for (const auto& jt : j.at("transactions")) b.transactions.push_back(Tx::from_json(jt));
  return b;
}

static bool meets_difficulty(std::string_view hex, int diff) {
  for (int i = 0; i < diff; ++i) if (i >= (int)hex.size() || hex[i] != '0') return false;
  return true;
}

std::string calculate_block_hash(const Block& b) {
  std::ostringstream oss;
  // include difficulty in header to avoid weirdness on retargeting
  oss << b.index << ';' << b.timestamp << ';' << b.prev_hash << ';' << b.merkle_root << ';'
      << b.difficulty << ';' << b.nonce;
  return crypto::sha256(oss.str());
}

void mine_block(Block& b) {
  using namespace std::chrono;
  auto start = steady_clock::now();
  b.nonce = 0;
  b.hash = calculate_block_hash(b);
  while (!meets_difficulty(b.hash, b.difficulty)) {
    ++b.nonce;
    b.hash = calculate_block_hash(b);
  }
  b.mine_ms = duration_cast<milliseconds>(steady_clock::now() - start).count();
}

}  // namespace sbc
