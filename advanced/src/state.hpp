/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

#include "tx.hpp"

namespace sbc {

struct AccountState {
  std::unordered_map<std::string, std::int64_t> balance;  // can go negative in invalid cases
  std::unordered_map<std::string, std::uint64_t> nonce;   // last accepted nonce per address
};

struct ApplyResult {
  bool ok;
  std::string error;
};

class StateMachine {
 public:
  explicit StateMachine(std::int64_t coinbase_reward = 50) : reward_(coinbase_reward) {}
  const AccountState& state() const { return st_; }

  // Verify and apply a transaction (no signature check for coinbase)
  ApplyResult applyTx(const Tx& tx);

  // Apply coinbase to miner address
  void applyCoinbase(const std::string& miner_addr);

 private:
  bool verifySignature(const Tx& tx) const;

 private:
  AccountState st_;
  std::int64_t reward_;
};

}  // namespace sbc
