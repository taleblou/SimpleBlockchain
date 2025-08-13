/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "crypto.hpp"
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace crypto {

std::string bytes_to_hex(const std::vector<unsigned char>& bytes) {
  std::ostringstream oss;
  for (auto b : bytes) {
    oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
  }
  return oss.str();
}

std::vector<unsigned char> hex_to_bytes(std::string_view hex) {
  std::vector<unsigned char> out;
  if (hex.size() % 2 != 0) throw std::runtime_error("hex length must be even");
  out.reserve(hex.size() / 2);
  for (size_t i = 0; i < hex.size(); i += 2) {
    unsigned int x;
    std::string byte = std::string(hex.substr(i, 2));
    std::istringstream(byte) >> std::hex >> x;
    out.push_back(static_cast<unsigned char>(x));
  }
  return out;
}

std::string sha256(std::string_view data) {
  unsigned int len = SHA256_DIGEST_LENGTH;
  std::vector<unsigned char> hash(len);
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
  return bytes_to_hex(hash);
}

std::pair<std::string, std::string> generate_ec_keypair() {
  std::string priv_pem, pub_pem;
  EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
  if (!pctx) throw std::runtime_error("EVP_PKEY_CTX_new_id failed");
  if (EVP_PKEY_paramgen_init(pctx) != 1) {
    EVP_PKEY_CTX_free(pctx);
    throw std::runtime_error("paramgen_init failed");
  }
  if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1) != 1) {
    EVP_PKEY_CTX_free(pctx);
    throw std::runtime_error("set curve failed");
  }
  EVP_PKEY* params = nullptr;
  if (EVP_PKEY_paramgen(pctx, &params) != 1) {
    EVP_PKEY_CTX_free(pctx);
    throw std::runtime_error("paramgen failed");
  }
  EVP_PKEY_CTX* kctx = EVP_PKEY_CTX_new(params, nullptr);
  if (!kctx) {
    EVP_PKEY_free(params);
    EVP_PKEY_CTX_free(pctx);
    throw std::runtime_error("PKEY_CTX_new failed");
  }
  if (EVP_PKEY_keygen_init(kctx) != 1) throw std::runtime_error("keygen_init failed");
  EVP_PKEY* pkey = nullptr;
  if (EVP_PKEY_keygen(kctx, &pkey) != 1) throw std::runtime_error("keygen failed");

  // Private PEM
  {
    BIO* mem = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(mem, pkey, nullptr, nullptr, 0, nullptr, nullptr);
    char* data;
    long len = BIO_get_mem_data(mem, &data);
    priv_pem.assign(data, len);
    BIO_free(mem);
  }
  // Public PEM
  {
    BIO* mem = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(mem, pkey);
    char* data;
    long len = BIO_get_mem_data(mem, &data);
    pub_pem.assign(data, len);
    BIO_free(mem);
  }

  EVP_PKEY_free(pkey);
  EVP_PKEY_free(params);
  EVP_PKEY_CTX_free(kctx);
  EVP_PKEY_CTX_free(pctx);
  return {priv_pem, pub_pem};
}

std::string ecdsa_sign_p256(std::string_view pem_private_key, std::string_view message) {
  BIO* bio = BIO_new_mem_buf(pem_private_key.data(), (int)pem_private_key.size());
  EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
  BIO_free(bio);
  if (!pkey) throw std::runtime_error("read private key failed");

  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  if (!ctx) { EVP_PKEY_free(pkey); throw std::runtime_error("MD_CTX_new failed"); }
  if (EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, pkey) != 1) {
    EVP_MD_CTX_free(ctx); EVP_PKEY_free(pkey); throw std::runtime_error("DigestSignInit failed");
  }
  if (EVP_DigestSignUpdate(ctx, message.data(), message.size()) != 1) {
    EVP_MD_CTX_free(ctx); EVP_PKEY_free(pkey); throw std::runtime_error("DigestSignUpdate failed");
  }
  size_t siglen = 0;
  EVP_DigestSignFinal(ctx, nullptr, &siglen);
  std::vector<unsigned char> sig(siglen);
  if (EVP_DigestSignFinal(ctx, sig.data(), &siglen) != 1) {
    EVP_MD_CTX_free(ctx); EVP_PKEY_free(pkey); throw std::runtime_error("DigestSignFinal failed");
  }
  sig.resize(siglen);
  EVP_MD_CTX_free(ctx);
  EVP_PKEY_free(pkey);
  return bytes_to_hex(sig);
}

bool ecdsa_verify_p256(std::string_view pem_public_key,
                       std::string_view message,
                       std::string_view sig_hex) {
  auto sig = hex_to_bytes(sig_hex);
  BIO* bio = BIO_new_mem_buf(pem_public_key.data(), (int)pem_public_key.size());
  EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
  BIO_free(bio);
  if (!pkey) return false;
  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  if (!ctx) { EVP_PKEY_free(pkey); return false; }
  bool ok = false;
  if (EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, pkey) == 1 &&
      EVP_DigestVerifyUpdate(ctx, message.data(), message.size()) == 1 &&
      EVP_DigestVerifyFinal(ctx, sig.data(), sig.size()) == 1) {
    ok = true;
  }
  EVP_MD_CTX_free(ctx);
  EVP_PKEY_free(pkey);
  return ok;
}

}  // namespace crypto
