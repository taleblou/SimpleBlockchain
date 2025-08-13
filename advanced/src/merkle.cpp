/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "merkle.hpp"
#include "crypto.hpp"

namespace merkle {
static std::string pair_hash(const std::string& a, const std::string& b) {
  return crypto::sha256(a + b);
}

std::string merkle_root(const std::vector<std::string>& tx_hashes) {
  if (tx_hashes.empty()) return crypto::sha256("");
  std::vector<std::string> level = tx_hashes;
  while (level.size() > 1) {
    if (level.size() % 2 == 1) level.push_back(level.back());
    std::vector<std::string> next;
    next.reserve(level.size() / 2);
    for (size_t i = 0; i < level.size(); i += 2) {
      next.push_back(pair_hash(level[i], level[i+1]));
    }
    level.swap(next);
  }
  return level[0];
}
}  // namespace merkle
