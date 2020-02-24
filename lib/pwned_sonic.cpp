#include "pwned_sonic.hpp"

#include <array>
#include <fstream>
#include <string>

namespace {
constexpr auto k_sha1_length{ 40u };
}

namespace okon {

bool transform_to_trie(std::string_view input_file, std::string_view output_file)
{
}

search_result exists_substring(std::string_view sha1_to_search, std::string_view file_path)
{
  std::ifstream in{ file_path.data() };

  if (!in.is_open()) {
    return search_result::file_not_found;
  }

  for (std::string line; std::getline(in, line);) {
    const auto sha1_in_line = std::string_view{ line.data(), k_sha1_length };
    if (sha1_to_search == sha1_in_line) {
      return search_result::found;
    }
  }

  return search_result::not_found;
}

search_result exists_static_buffer_length(std::string_view sha1_to_search,
                                          std::string_view file_path)
{
  std::ifstream in{ file_path.data() };

  if (!in.is_open()) {
    return search_result::file_not_found;
  }

  constexpr auto k_max_line_length{ k_sha1_length + 15u };

  for (std::array<char, k_max_line_length> line{}; in.getline(&line[0], k_max_line_length);) {
    const auto sha1_in_line = std::string_view{ line.data(), k_sha1_length };
    if (sha1_to_search == sha1_in_line) {
      return search_result::found;
    }
  }

  return search_result::not_found;
}

void prepare_chunks(std::string_view file_path)
{
}
}
