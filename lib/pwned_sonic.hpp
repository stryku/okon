#pragma once

#include "splitted_files.hpp"

#include <cstring>
#include <string_view>

namespace pwned {

enum class search_result
{
  file_not_found,
  found,
  not_found
};

bool transform_to_trie(std::string_view input_file, std::string_view output_file);

search_result exists_substring(std::string_view sha1_to_search, std::string_view file_path);
search_result exists_static_buffer_length(std::string_view sha1_to_search,
                                          std::string_view file_path);

void prepare_chunks(std::string_view file_path);

inline bool exists_splitted_sorted(std::string_view path_to_splitted, std::string_view sha1)
{
  splitted_files files{ path_to_splitted };
  auto& file = files.sha1_file(sha1);

  file.seekp(0, std::ios::end);
  const std::streamsize file_size = file.tellp();
  const auto sha1_count = file_size / sizeof(sha1_t);

  sha1_t read_sha1;

  auto begin = 0u;
  auto end = sha1_count - 1u;

  while (begin <= end) {
    const auto pivot = (end - begin) / 2 + begin;

    file.seekg(pivot * sizeof(sha1_t));
    file.read(reinterpret_cast<char*>(&read_sha1[0]), sizeof(sha1_t));

    std::string_view view{ (const char*)read_sha1.data(), sizeof(sha1_t) };
    if (view == sha1) {
      return true;
    }

    if (std::memcmp(view.data(), sha1.data(), sizeof(sha1_t)) < 0) {
      begin = pivot + 1;
    } else {
      end = pivot - 1;
    }
  }

  return false;
}

}
