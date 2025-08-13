/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "blockchain.hpp"
#include "crypto.hpp"
#include "merkle.hpp"
#include "util.hpp"

#include <numeric>
#include <stdexcept>

namespace sbc {

Blockchain::Blockchain(Params p)
    : params_(p), current_diff_(p.initial_difficulty), state_(/*reward=*/50) {
  chain_.push_back(genesis());
}

Block Blockchain::genesis() {
  Block g;
  g.index = 0;
  g.timestamp = util::now_iso8601();
  g.prev_hash = "0";
  g.difficulty = 0;
  g.merkle_root = merkle::merkle_root({});
  g.nonce = 0;
  g.hash = calculate_block_hash(g);
  return g;
}

void Blockchain::addTransaction(Tx tx) {
  if (tx.from_pubkey_pem.empty()) throw std::invalid_argument("Use coinbase via miner address");
  mempool_.push_back(std::move(tx));
}

std::string Blockchain::compute_merkle() const {
  std::vector<std::string> txids;
  txids.reserve(mempool_.size());
  for (const auto& t : mempool_) txids.push_back(t.hash());
  return merkle::merkle_root(txids);
}

const Block& Blockchain::minePending(const std::string& miner_addr) {
  // apply pending txs to a copy of state to validate
  StateMachine st = state_;
  for (const auto& tx : mempool_) {
    auto r = st.applyTx(tx);
    if (!r.ok) throw std::runtime_error("Bad tx in mempool: " + r.error);
  }

  Block b;
  b.index = chain_.size();
  b.timestamp = util::now_iso8601();
  b.prev_hash = chain_.back().hash;
  b.difficulty = current_diff_;
  b.merkle_root = compute_merkle();

  // mine
  mine_block(b);

  // commit: coinbase then txs
  state_.applyCoinbase(miner_addr);
  for (const auto& tx : mempool_) state_.applyTx(tx);
  mempool_.clear();

  chain_.push_back(b);

  retargetIfNeeded();
  return chain_.back();
}

void Blockchain::retargetIfNeeded() {
  if (chain_.size() <= 1) return;
  if ((chain_.size() - 1) % params_.retarget_interval != 0) return;

  // average mine_ms over last N blocks
  std::size_t N = params_.retarget_interval;
  std::size_t start = chain_.size() - N;
  if (start < 1) start = 1;
  std::uint64_t sum_ms = 0;
  std::size_t cnt = 0;
  for (std::size_t i = start; i < chain_.size(); ++i) {
    sum_ms += chain_[i].mine_ms;
    ++cnt;
  }
  if (cnt == 0) return;
  auto avg_sec = (sum_ms / cnt) / 1000.0;
  if (avg_sec < params_.target_block_time_sec * 0.8) current_diff_ += 1;
  else if (avg_sec > params_.target_block_time_sec * 1.2 && current_diff_ > 0) current_diff_ -= 1;
}

bool Blockchain::validate_block_link(std::size_t i) const {
  if (i == 0 || i >= chain_.size()) return true;
  const auto& prev = chain_[i - 1];
  const auto& cur = chain_[i];
  if (cur.prev_hash != prev.hash) return false;
  if (calculate_block_hash(cur) != cur.hash) return false;
  // difficulty check
  for (int k = 0; k < cur.difficulty; ++k) if (cur.hash[k] != '0') return false;
  return true;
}

bool Blockchain::isValid() const {
  for (std::size_t i = 1; i < chain_.size(); ++i) if (!validate_block_link(i)) return false;
  return true;
}

std::string Blockchain::toJson() const {
  nlohmann::json j;
  j["params"] = {{"initial_difficulty", params_.initial_difficulty},
                 {"target_block_time_sec", params_.target_block_time_sec},
                 {"retarget_interval", params_.retarget_interval}};
  j["current_diff"] = current_diff_;
  j["chain"] = nlohmann::json::array();
  for (const auto& b : chain_) j["chain"].push_back(b.to_json());
  // state dump
  nlohmann::json st;
  for (const auto& kv : state_.state().balance) st["balance"][kv.first] = kv.second;
  for (const auto& kv : state_.state().nonce) st["nonce"][kv.first] = kv.second;
  j["state"] = st;
  return j.dump(2);
}

Blockchain Blockchain::fromJson(const std::string& s) {
  auto j = nlohmann::json::parse(s);
  Params p;
  auto jp = j["params"];
  p.initial_difficulty = jp.value("initial_difficulty", 3);
  p.target_block_time_sec = jp.value("target_block_time_sec", 10);
  p.retarget_interval = jp.value("retarget_interval", 10);
  Blockchain bc(p);
  bc.current_diff_ = j.value("current_diff", p.initial_difficulty);
  bc.chain_.clear();
  for (const auto& jb : j["chain"]) bc.chain_.push_back(Block::from_json(jb));
  // state (best-effort)
  // Note: For strictness you'd recompute state by replaying txs.
  return bc;
}

}  // namespace sbc
