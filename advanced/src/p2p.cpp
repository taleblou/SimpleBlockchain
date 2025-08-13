/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "p2p.hpp"
#include <cstring>
#include <sstream>
#include <string>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace p2p {

static bool split_hostport(const std::string& hp, std::string* host, int* port) {
  auto pos = hp.find(':');
  if (pos == std::string::npos) return false;
  *host = hp.substr(0, pos);
  *port = std::stoi(hp.substr(pos + 1));
  return true;
}

Listener::Listener(const std::string& host, int port, std::function<void(std::string)> on_message)
    : host_(host), port_(port), on_message_(std::move(on_message)) {}

void Listener::start() {
  running_ = true;
  th_ = std::thread([this] {
#if defined(_WIN32)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    int server_fd_close_marker = server_fd;
    if (server_fd < 0) return;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = inet_addr(host_.c_str());
    int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));
    if (::bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) { /* fail silently */ }
    ::listen(server_fd, 8);
    while (running_) {
      sockaddr_in cli{};
      socklen_t clilen = sizeof(cli);
      int cfd = ::accept(server_fd, (sockaddr*)&cli, &clilen);
      if (cfd < 0) continue;
      std::string buf;
      char tmp[1024];
      int n;
#if defined(_WIN32)
      while ((n = ::recv(cfd, tmp, sizeof(tmp), 0)) > 0) buf.append(tmp, tmp + n);
      closesocket(cfd);
#else
      while ((n = ::recv(cfd, tmp, sizeof(tmp), 0)) > 0) buf.append(tmp, tmp + n);
      ::close(cfd);
#endif
      if (!buf.empty() && on_message_) on_message_(buf);
    }
#if defined(_WIN32)
    WSACleanup();
#endif
            #if defined(_WIN32)
            closesocket(server_fd_close_marker);
            #else
            ::close(server_fd_close_marker);
            #endif
      });
}


void Listener::stop() {
  running_ = false;
  if (th_.joinable()) th_.join();
}

Listener::~Listener() { stop(); }

void broadcast(const std::vector<std::string>& peers, const std::string& line) {
#if defined(_WIN32)
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  for (const auto& hp : peers) {
    std::string host; int port;
    if (!split_hostport(hp, &host, &port)) continue;
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) continue;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());
    if (::connect(fd, (sockaddr*)&addr, sizeof(addr)) == 0) {
      ::send(fd, line.data(), (int)line.size(), 0);
    }
#if defined(_WIN32)
    closesocket(fd);
#else
    ::close(fd);
#endif
  }
#if defined(_WIN32)
  WSACleanup();
#endif
}

}  // namespace p2p
