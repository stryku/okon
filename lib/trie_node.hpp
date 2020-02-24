#pragma once

#include <algorithm>
#include <array>
#include <limits>

namespace pwned {
struct trie_node
{
  using index_t = uint32_t;

  static constexpr auto k_unused_node_index{ std::numeric_limits<index_t>::max() - 1 };
  static constexpr auto k_end_node_index{ std::numeric_limits<index_t>::max() };

  explicit trie_node()
    : trie_node{ k_unused_node_index }
  {
  }
  explicit trie_node(index_t value) { std::fill(std::begin(chars), std::end(chars), value); }

  static trie_node end_node() { return trie_node{ k_end_node_index }; }
  static trie_node unused_node() { return trie_node{ k_unused_node_index }; }

  std::array<index_t, 16u> chars;

  static constexpr auto k_binary_size{ sizeof(chars) };
};
}
