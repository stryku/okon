#pragma once

#include "btree_node.hpp"

namespace pwned {
template <typename DataStorage>
class btree
{
public:
  explicit btree(DataStorage& storage)
    : m_storage{ storage }
  {
    m_storage.seek_in(0u);
    m_storage.read(&m_t, sizeof(m_t));
    m_storage.read(&m_root_ptr, sizeof(m_root_ptr));
  }

  bool contains(const sha1_t& sha1) const
  {
    const auto node = read_root();
    return contains(node, sha1);
  }

private:
  bool contains(const btree_node& node, const sha1_t& sha1) const
  {
    if (node.contains(sha1)) {
      return true;
    }

    if (node.is_leaf) {
      return false;
    }

    const auto place = node.place_for(sha1);
    const auto next_node_ptr = node.pointers[place];
    const auto next_node = read_node(next_node_ptr);
    return contains(next_node, sha1);
  }

  btree_node read_root() const { return read_node(m_root_ptr); }

  btree_node read_node(btree_node::pointer_t ptr) const
  {
    btree_node node{ m_t, btree_node::k_unused_pointer };

    const auto pointers_size = btree_node::binary_pointers_size(m_t);
    const auto keys_size = btree_node::binary_keys_size(m_t);
    const auto node_place =
      uint64_t{ tree_offset() } + uint64_t{ btree_node::binary_size(m_t) } * uint64_t{ ptr };

    m_storage.seek_in(node_place);
    m_storage.read(reinterpret_cast<char*>(&node.is_leaf), sizeof(node.is_leaf));
    m_storage.read(reinterpret_cast<char*>(&node.keys_count), sizeof(node.keys_count));
    m_storage.read(reinterpret_cast<char*>(&node.pointers[0]), pointers_size);
    m_storage.read(reinterpret_cast<char*>(&node.keys[0]), keys_size);
    m_storage.read(reinterpret_cast<char*>(&node.parent_pointer), sizeof(node.parent_pointer));

    node.this_pointer = ptr;

    return node;
  }

  uint64_t tree_offset() const
  {
    // t + root_pointer
    return sizeof(uint32_t) + sizeof(btree_node::pointer_t);
  }

private:
  DataStorage& m_storage;
  uint32_t m_t{};
  btree_node::pointer_t m_root_ptr{};
};
}
