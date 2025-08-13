/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace p2p {

// Starts a background listener on host:port and calls on_message(json) for each line of input.
class Listener {
 public:
  Listener(const std::string& host, int port, std::function<void(std::string)> on_message);
  ~Listener();
  void start();
  void stop();

 private:
  std::string host_;
  int port_;
  std::function<void(std::string)> on_message_;
  std::atomic<bool> running_{false};
  std::thread th_;
};

// Broadcast a line of text (json) to each peer host:port ("127.0.0.1:9002", ...)
void broadcast(const std::vector<std::string>& peers, const std::string& line);

}  // namespace p2p
