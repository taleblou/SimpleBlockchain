/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <openssl/evp.h>
#include <string>
#include <string_view>
#include <vector>

namespace crypto {
// SHA-256 hex
std::string sha256(std::string_view data);

// ECDSA (prime256v1) helpers
// Generate keypair in PEM strings (private, public)
std::pair<std::string, std::string> generate_ec_keypair();

// Sign message bytes with a PEM private key -> DER signature (hex)
std::string ecdsa_sign_p256(std::string_view pem_private_key, std::string_view message);

// Verify message/signature with a PEM public key
bool ecdsa_verify_p256(std::string_view pem_public_key,
                       std::string_view message,
                       std::string_view sig_hex);

// Hex helpers
std::string bytes_to_hex(const std::vector<unsigned char>& bytes);
std::vector<unsigned char> hex_to_bytes(std::string_view hex);
}  // namespace crypto
