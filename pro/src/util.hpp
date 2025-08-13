/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

namespace util {

inline std::string now_iso8601() {
  using namespace std::chrono;
  auto now = system_clock::now();
  auto t = system_clock::to_time_t(now);
  std::tm tm{};
#if defined(_WIN32)
  gmtime_s(&tm, &t);
#else
  gmtime_r(&t, &tm);
#endif
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
  return oss.str();
}

}  // namespace util
