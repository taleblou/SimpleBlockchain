/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "block.hpp"
#include "crypto.hpp"

#include <sstream>

namespace sbc {

static std::string join_transactions(const std::vector<std::string>& txs) {
  std::ostringstream oss;
  for (size_t i = 0; i < txs.size(); ++i) {
    if (i) oss << '|';
    oss << txs[i];
  }
  return oss.str();
}

static std::string calc_hash_raw(std::uint64_t index,
                                 std::string_view timestamp,
                                 const std::vector<std::string>& txs,
                                 std::string_view prev_hash,
                                 std::uint64_t nonce) {
  std::ostringstream oss;
  oss << index << ';' << timestamp << ';' << prev_hash << ';' << nonce << ';' << join_transactions(txs);
  return crypto::sha256(oss.str());
}

// Mine until hash has `difficulty` leading zero hex chars.
static bool meets_difficulty(std::string_view hash, int difficulty) {
  for (int i = 0; i < difficulty; ++i) {
    if (i >= (int)hash.size() || hash[i] != '0') return false;
  }
  return true;
}

std::string calculate_hash(const Block& b) {
  return calc_hash_raw(b.index, b.timestamp, b.transactions, b.prev_hash, b.nonce);
}

void mine_block(Block& b, int difficulty) {
  b.nonce = 0;
  b.hash = calculate_hash(b);
  while (!meets_difficulty(b.hash, difficulty)) {
    ++b.nonce;
    b.hash = calculate_hash(b);
  }
}

}  // namespace sbc
