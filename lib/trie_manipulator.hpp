#pragma once

#include "trie_node.hpp"
#include "trie_utils.hpp"

#include <fstream>
#include <optional>

namespace pwned {
class trie_manipulator : public trie_utils
{
public:
  explicit trie_manipulator(std::fstream& file, trie_node::index_t& node_count);

  std::optional<trie_node> node();

  void write_initial_node();

  bool go_to_next_node(char c);
  void go_to_node(uint64_t node_index);

  void mark_as_end_node();

  void write_next_node(char c);

  void go_to_start_node();

private:
  trie_node node_unsafe();
  trie_node read_node();

  void write_node(trie_node::index_t node_index, const trie_node& node);

private:
  std::fstream& m_file;
  uint64_t m_current_node_index{ 0u };
  std::optional<trie_node> m_current_node;
  trie_node::index_t& m_node_count;
};
}
