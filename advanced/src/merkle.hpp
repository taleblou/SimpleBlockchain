/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace merkle {
// Given vector of tx hashes (hex), compute merkle root (hex).
// If empty, return sha256(""), and if odd, duplicate the last element.
std::string merkle_root(const std::vector<std::string>& tx_hashes);
}  // namespace merkle
