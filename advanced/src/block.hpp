/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
#include "tx.hpp"

namespace sbc {

struct Block {
  std::uint64_t index{};
  std::string timestamp;
  std::string prev_hash;
  std::string merkle_root;
  std::string hash;
  std::uint64_t nonce{};
  int difficulty{};
  std::uint64_t mine_ms{};  // mining time in ms (informational)

  std::vector<Tx> transactions;  // includes non-coinbase; coinbase implied separately? keep simple

  nlohmann::json to_json() const;
  static Block from_json(const nlohmann::json& j);
};

std::string calculate_block_hash(const Block& b);
void mine_block(Block& b);

}  // namespace sbc
