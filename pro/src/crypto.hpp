/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <array>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace crypto {

inline std::string sha256(std::string_view data) {
  unsigned int len = SHA256_DIGEST_LENGTH;
  std::array<unsigned char, SHA256_DIGEST_LENGTH> hash{};

  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  if (!ctx) throw std::runtime_error("EVP_MD_CTX_new failed");

  const EVP_MD* md = EVP_sha256();
  if (EVP_DigestInit_ex(ctx, md, nullptr) != 1) {
    EVP_MD_CTX_free(ctx);
    throw std::runtime_error("EVP_DigestInit_ex failed");
  }
  if (EVP_DigestUpdate(ctx, data.data(), data.size()) != 1) {
    EVP_MD_CTX_free(ctx);
    throw std::runtime_error("EVP_DigestUpdate failed");
  }
  if (EVP_DigestFinal_ex(ctx, hash.data(), &len) != 1) {
    EVP_MD_CTX_free(ctx);
    throw std::runtime_error("EVP_DigestFinal_ex failed");
  }
  EVP_MD_CTX_free(ctx);

  std::ostringstream oss;
  for (unsigned i = 0; i < len; ++i) {
    oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
  }
  return oss.str();
}

}  // namespace crypto
