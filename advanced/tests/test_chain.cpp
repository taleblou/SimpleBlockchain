/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "gtest/gtest.h"
#include "blockchain.hpp"
#include "crypto.hpp"
#include "tx.hpp"

using namespace sbc;

TEST(AdvancedChain, MineAndValidate) {
  Blockchain::Params p; p.initial_difficulty = 1; p.target_block_time_sec = 1; p.retarget_interval = 2;
  Blockchain bc(p);
  auto kp = crypto::generate_ec_keypair();
  auto addr = Tx::addr_from_pubkey(kp.second);

  // Give sender some coins via mining coinbase to itself
  bc.minePending(addr);
  bc.minePending(addr);

  // Create tx: send 5 to a random address
  Tx tx;
  tx.from_pubkey_pem = kp.second;
  tx.to_addr = "deadbeefcafebabe0123";
  tx.amount = 5;
  tx.nonce = 1;  // first tx from this sender
  tx.signature_hex = crypto::ecdsa_sign_p256(kp.first, tx.message());
  bc.addTransaction(tx);

  bc.minePending(addr);
  EXPECT_TRUE(bc.isValid());
}
