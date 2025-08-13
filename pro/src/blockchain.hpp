/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "block.hpp"

namespace sbc {

class Blockchain {
 public:
  explicit Blockchain(int difficulty = 3);

  // Transactions go to mempool until mined
  void addTransaction(std::string tx);
  std::size_t mempoolSize() const noexcept { return mempool_.size(); }

  // Mine a block from current mempool
  const Block& minePending();

  // Validate the whole chain
  bool isValid() const;

  // Access
  const std::vector<Block>& chain() const noexcept { return chain_; }
  int difficulty() const noexcept { return difficulty_; }
  void setDifficulty(int d) noexcept { difficulty_ = d; }

  // Persistence
  std::string toJsonString(int indent = 2) const;
  static Blockchain fromJsonString(std::string_view data);

 private:
  static Block makeGenesis();
  bool validateLink(std::size_t i) const;  // link i-1 -> i

 private:
  std::vector<Block> chain_;
  std::vector<std::string> mempool_;
  int difficulty_;
};

}  // namespace sbc
