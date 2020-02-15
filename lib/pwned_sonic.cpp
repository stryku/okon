#include "pwned_sonic.hpp"

#include <fstream>
#include <string>

namespace {
constexpr auto k_sha1_length{ 40u };
}

namespace pwned {

bool transform_to_trie(std::string_view input_file, std::string_view output_file)
{
}

search_result exists(std::string_view sha1_to_search, std::string_view file_path)
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
}
