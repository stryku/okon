#pragma once

#include "trie_node.hpp"

#include <array>
#include <fstream>
#include <optional>
#include <string_view>

namespace pwned {
class trie_file_generator
{
public:
  explicit trie_file_generator(std::string_view input_file_path, std::string_view output_file_path);

  enum class generation_result
  {
    success,
    input_file_not_found,
    cant_open_output_file
  };

  generation_result generate();

private:
  std::optional<std::string_view> get_next_sha1();
  void add_sha1_to_trie(std::string_view sha1);

  constexpr uint8_t char_to_index(char c) const;

  uint64_t generate_next_node_index();

private:
  std::string_view m_input_file_path;
  std::string_view m_output_file_path;
  std::fstream m_input_file;
  std::fstream m_output_file;
  trie_node::index_t m_nodes_count{ 0u };
};
}
