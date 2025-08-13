/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "nlohmann/json.hpp"

namespace sbc {

using json = nlohmann::json;

struct Block {
  std::uint64_t index{};
  std::string timestamp;  // ISO-8601 UTC
  std::vector<std::string> transactions;
  std::string prev_hash;
  std::string hash;
  std::uint64_t nonce{};

  Block() = default;

  Block(std::uint64_t idx,
        std::string ts,
        std::vector<std::string> txs,
        std::string prev)
      : index(idx), timestamp(std::move(ts)), transactions(std::move(txs)), prev_hash(std::move(prev)) {}

  // Serialize
  json to_json() const {
    return json{
        {"index", index},
        {"timestamp", timestamp},
        {"transactions", transactions},
        {"prev_hash", prev_hash},
        {"hash", hash},
        {"nonce", nonce},
    };
  }

  static Block from_json(const json& j) {
    Block b;
    b.index = j.at("index").get<std::uint64_t>();
    b.timestamp = j.at("timestamp").get<std::string>();
    b.transactions = j.at("transactions").get<std::vector<std::string>>();
    b.prev_hash = j.at("prev_hash").get<std::string>();
    b.hash = j.at("hash").get<std::string>();
    b.nonce = j.at("nonce").get<std::uint64_t>();
    return b;
  }
};

}  // namespace sbc
