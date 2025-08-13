/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "block.hpp"
#include "state.hpp"
#include "tx.hpp"

namespace sbc {

class Blockchain {
 public:
  struct Params {
    int initial_difficulty = 3;
    std::uint64_t target_block_time_sec = 10;  // educational
    std::size_t retarget_interval = 10;        // adjust every N blocks
  };

  explicit Blockchain(Params p = {});

  void addTransaction(Tx tx);
  const Block& minePending(const std::string& miner_addr);

  bool isValid() const;

  const std::vector<Block>& chain() const noexcept { return chain_; }
  const StateMachine& state() const noexcept { return state_; }
  int difficulty() const noexcept { return current_diff_; }

  // Persistence helpers
  std::string toJson() const;
  static Blockchain fromJson(const std::string& s);

 private:
  static Block genesis();
  void retargetIfNeeded();
  std::string compute_merkle() const;
  bool validate_block_link(std::size_t i) const;

 private:
  Params params_;
  int current_diff_;
  std::vector<Block> chain_;
  std::vector<Tx> mempool_;
  StateMachine state_;
};

}  // namespace sbc
