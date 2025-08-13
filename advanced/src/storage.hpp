/*
SPDX-License-Identifier: MIT
Copyright (c) 2025 Morteza Taleblou (https://taleblou.ir/)
*/

#pragma once
#include <string>

namespace storage {
bool save_json_to_file(const std::string& path, const std::string& content, std::string* err);
bool load_file_to_string(const std::string& path, std::string* out, std::string* err);
}  // namespace storage
