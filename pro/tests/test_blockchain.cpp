/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "gtest/gtest.h"
#include "blockchain.hpp"

using namespace sbc;

TEST(Blockchain, BasicFlow) {
  Blockchain bc(2);
  bc.addTransaction("A pays B 10");
  bc.addTransaction("B pays C 5");
  const auto& b1 = bc.minePending();
  EXPECT_GE(b1.nonce, 0u);
  EXPECT_TRUE(bc.isValid());

  // Tamper data
  auto& chain = const_cast<std::vector<Block>&>(bc.chain());
  chain[1].transactions[0] = "A pays B 1000";
  EXPECT_FALSE(bc.isValid());
}
