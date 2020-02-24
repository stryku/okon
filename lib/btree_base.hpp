#pragma once

#include "btree_node.hpp"

namespace pwned {

template <typename DataStorage>
class btree_base
{
public:
  explicit btree_base(DataStorage& storage, uint32_t t)
    : m_storage{ storage }
    , m_t{ t }
  {
    m_storage.seek_in(0u);
    m_storage.write(&m_t, sizeof(m_t));
  }

protected:
  void set_root_ptr(btree_node::pointer_t ptr)
  {
    m_root_ptr = ptr;
    constexpr auto root_ptr_offset = sizeof(m_t);
    m_storage.seek_out(root_ptr_offset);
    m_storage.write(&m_root_ptr, sizeof(btree_node::pointer_t));
  }

  btree_node read_node(btree_node::pointer_t ptr)
  {
    const auto pointers_size = btree_node::binary_pointers_size(this->t());
    const auto keys_size = btree_node::binary_keys_size(this->t());
    const uint64_t node_place =
      uint64_t{ tree_offset() } + uint64_t{ btree_node::binary_size(m_t) } * uint64_t{ ptr };

    btree_node node{ this->t(), btree_node::k_unused_pointer };

    m_storage.seek_in(node_place);
    m_storage.read(&node.is_leaf, sizeof(node.is_leaf));
    m_storage.read(&node.keys_count, sizeof(node.keys_count));
    m_storage.read(&node.pointers[0], pointers_size);
    m_storage.read(&node.keys[0], keys_size);
    m_storage.read(&node.parent_pointer, sizeof(node.parent_pointer));

    node.this_pointer = ptr;

    return node;
  }

  void write_node(const btree_node& node)
  {
    const auto pointers_size = btree_node::binary_pointers_size(m_t);
    const auto keys_size = btree_node::binary_keys_size(m_t);
    const uint64_t node_place =
      tree_offset() + uint64_t{ btree_node::binary_size(m_t) } * uint64_t{ node.this_pointer };

    m_storage.seek_out(node_place);

    m_storage.write(&node.is_leaf, sizeof(node.is_leaf));
    m_storage.write(&node.keys_count, sizeof(node.keys_count));
    m_storage.write(node.pointers.data(), pointers_size);
    m_storage.write(node.keys.data(), keys_size);
    m_storage.write(&node.parent_pointer, sizeof(node.parent_pointer));
  }

  constexpr uint64_t tree_offset() const
  {
    return sizeof(m_t) + sizeof(m_root_ptr);
  }

  uint32_t t() const
  {
    return m_t;
  }

  btree_node::pointer_t root_ptr() const
  {
    return m_root_ptr;
  }

private:
  DataStorage& m_storage;
  unsigned m_t{};
  btree_node::pointer_t m_root_ptr{ 0u };
};
}
