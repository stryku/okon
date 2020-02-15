#pragma once

#include <string_view>

namespace pwned {

enum class search_result
{
  file_not_found,
  found,
  not_found
};

bool transform_to_trie(std::string_view input_file, std::string_view output_file);

search_result exists(std::string_view sha1, std::string_view file_path);

}
