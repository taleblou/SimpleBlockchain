/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "tx.hpp"
#include "crypto.hpp"
#include "nlohmann/json.hpp"

namespace sbc {

std::string Tx::addr_from_pubkey(const std::string& pem) {
  auto h = crypto::sha256(pem);
  return h.substr(0, 40);  // demo address (not production-safe)
}

std::string Tx::message() const {
  // Minimal canonical rep (no spaces, fixed field order)
  return "from=" + from_pubkey_pem + ";to=" + to_addr + ";amount=" + std::to_string(amount) +
         ";nonce=" + std::to_string(nonce);
}

std::string Tx::hash() const {
  return crypto::sha256(to_json().dump());
}

nlohmann::json Tx::to_json() const {
  return nlohmann::json{{"from_pubkey_pem", from_pubkey_pem},
                        {"to_addr", to_addr},
                        {"amount", amount},
                        {"nonce", nonce},
                        {"signature_hex", signature_hex}};
}

Tx Tx::from_json(const nlohmann::json& j) {
  Tx t;
  t.from_pubkey_pem = j.value("from_pubkey_pem", "");
  t.to_addr = j.at("to_addr").get<std::string>();
  t.amount = j.at("amount").get<std::uint64_t>();
  t.nonce = j.at("nonce").get<std::uint64_t>();
  t.signature_hex = j.value("signature_hex", "");
  return t;
}

}  // namespace sbc
