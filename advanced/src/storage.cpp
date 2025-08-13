/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#include "storage.hpp"
#include <fstream>

namespace storage {
bool save_json_to_file(const std::string& path, const std::string& content, std::string* err) {
  std::ofstream out(path, std::ios::binary);
  if (!out) { if (err) *err = "open failed"; return false; }
  out << content;
  if (!out.good()) { if (err) *err = "write failed"; return false; }
  return true;
}
bool load_file_to_string(const std::string& path, std::string* out, std::string* err) {
  std::ifstream in(path, std::ios::binary);
  if (!in) { if (err) *err = "open failed"; return false; }
  out->assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  return true;
}
}  // namespace storage
