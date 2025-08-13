/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include "nlohmann/json.hpp"

namespace sbc {

// Canonical account-based transaction with ECDSA P-256
struct Tx {
  std::string from_pubkey_pem;  // empty means coinbase
  std::string to_addr;          // address = sha256(pubkey_pem)[:40] (hex truncation) for demo
  std::uint64_t amount{};       // in minimal units
  std::uint64_t nonce{};        // per-sender
  std::string signature_hex;    // DER hex over message()

  static std::string addr_from_pubkey(const std::string& pem);
  std::string message() const;  // deterministic message string for signing
  std::string hash() const;     // tx id = sha256(json)

  nlohmann::json to_json() const;
  static Tx from_json(const nlohmann::json& j);
};

}  // namespace sbc
