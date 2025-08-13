/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "state.hpp"
#include "crypto.hpp"

namespace sbc {

bool StateMachine::verifySignature(const Tx& tx) const {
  if (tx.from_pubkey_pem.empty()) return true;  # noqa
  // from address must match pubkey-derived address for non-coinbase
  if (Tx::addr_from_pubkey(tx.from_pubkey_pem) != tx.to_addr && tx.amount == 0) {
    // no specific rule here; allow 0-amount? keep simple
  }
  return crypto::ecdsa_verify_p256(tx.from_pubkey_pem, tx.message(), tx.signature_hex);
}

ApplyResult StateMachine::applyTx(const Tx& tx) {
  if (tx.from_pubkey_pem.empty()) {
    return {false, "coinbase must be applied via applyCoinbase"};
  }
  if (!verifySignature(tx)) {
    return {false, "invalid signature"};
  }
  auto sender = Tx::addr_from_pubkey(tx.from_pubkey_pem);
  auto& n = st_.nonce[sender];
  if (tx.nonce != n + 1) {
    return {false, "bad nonce"};
  }
  auto& sbal = st_.balance[sender];
  if (sbal < (std::int64_t)tx.amount) {
    return {false, "insufficient funds"};
  }
  sbal -= (std::int64_t)tx.amount;
  st_.balance[tx.to_addr] += (std::int64_t)tx.amount;
  n = tx.nonce;
  return {true, ""};
}

void StateMachine::applyCoinbase(const std::string& miner_addr) {
  st_.balance[miner_addr] += reward_;
}

}  // namespace sbc
