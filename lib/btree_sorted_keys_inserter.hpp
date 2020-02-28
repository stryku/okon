#pragma once

#include "btree_base.hpp"
#include "sha1_utils.hpp"

namespace okon {

template <typename DataStorage>
class btree_sorted_keys_inserter : public btree_base<DataStorage>
{
public:
  explicit btree_sorted_keys_inserter(DataStorage& storage, uint32_t order);

  void insert_sorted(const sha1_t& sha1);
  void finalize_inserting();

private:
  btree_node::pointer_t new_node_pointer();
  btree_node split_node(btree_node& node, const sha1_t& sha1, unsigned level = 0u);
  btree_node create_children_till_leaf(btree_node& parent, unsigned level);

private:
  btree_node::pointer_t m_next_node_ptr{ 0u };
  btree_node m_current_node;
  unsigned m_current_tree_height{ 1u };
};

template <typename DataStorage>
btree_sorted_keys_inserter<DataStorage>::btree_sorted_keys_inserter(DataStorage& storage,
                                                                    uint32_t order)
  : btree_base<DataStorage>{ storage, order }
  , m_current_node{ order, btree_node::k_unused_pointer }
{
  m_current_node.this_pointer = new_node_pointer();
  m_current_node.is_leaf = true;
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::insert_sorted(const sha1_t& sha1)
{
  if (m_current_node.is_full()) {
    m_current_node = split_node(m_current_node, sha1);
  } else {
    m_current_node.insert(sha1);
  }
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::finalize_inserting()
{
  this->write_node(m_current_node);
}

template <typename DataStorage>
btree_node::pointer_t btree_sorted_keys_inserter<DataStorage>::new_node_pointer()
{
  return m_next_node_ptr++;
}

template <typename DataStorage>
btree_node btree_sorted_keys_inserter<DataStorage>::split_node(btree_node& node, const sha1_t& sha1,
                                                               unsigned level)
{
  const auto is_root = (node.this_pointer == this->root_ptr());
  if (is_root) {

    const auto new_root_ptr = new_node_pointer();

    btree_node new_root{ this->order(), node.parent_pointer };
    new_root.insert(sha1);
    new_root.pointers[0] = node.this_pointer;
    new_root.this_pointer = new_root_ptr;
    new_root.is_leaf = false;

    // create_children_till_leaf() will write the new_root to file.
    const auto new_child_node = create_children_till_leaf(new_root, m_current_tree_height - 1);

    node.parent_pointer = new_root_ptr;
    this->write_node(node);

    this->set_root_ptr(new_root_ptr);

    ++m_current_tree_height;

    return new_child_node;
  } else {
    this->write_node(node);
    auto parent_node = this->read_node(node.parent_pointer);
    if (parent_node.is_full()) {
      return split_node(parent_node, sha1, level + 1);
    } else {
      parent_node.insert(sha1);
      this->write_node(parent_node);
      return create_children_till_leaf(parent_node, level);
    }
  }
}

template <typename DataStorage>
btree_node btree_sorted_keys_inserter<DataStorage>::create_children_till_leaf(btree_node& parent,
                                                                              unsigned level)
{
  const auto is_on_leaf_level = (level == 0u);

  btree_node node{ this->order(), parent.this_pointer };
  node.this_pointer = new_node_pointer();
  node.keys_count = 0u;
  node.is_leaf = is_on_leaf_level;
  this->write_node(node);

  parent.pointers[parent.keys_count] = node.this_pointer;
  this->write_node(parent);

  if (is_on_leaf_level) {
    return node;
  }

  return create_children_till_leaf(node, level - 1u);
}

}
