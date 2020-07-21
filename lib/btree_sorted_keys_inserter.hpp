#pragma once

#include "btree_base.hpp"
#include "sha1_utils.hpp"

#include <vector>

namespace okon {

template <typename DataStorage>
class btree_sorted_keys_inserter : public btree_base<DataStorage>
{
public:
  explicit btree_sorted_keys_inserter(DataStorage& storage, btree_node::order_t order);

  void insert_sorted(const sha1_t& sha1);
  void finalize_inserting();

private:
  btree_node::pointer_t new_node_pointer();
  void split_node(const sha1_t& sha1, unsigned level_from_leafs = 0u);
  void split_root_and_grow(const sha1_t& sha1, unsigned level_from_leafs);
  void create_children_till_leaf(unsigned level_from_leafs);
  btree_node& current_node();
  void rebalance_tree();

private:
  btree_node::pointer_t m_next_node_ptr{ 0u };
  std::vector<btree_node> m_current_path;
};

template <typename DataStorage>
btree_sorted_keys_inserter<DataStorage>::btree_sorted_keys_inserter(DataStorage& storage,
                                                                    btree_node::order_t order)
  : btree_base<DataStorage>{ storage, order }
{
  auto& root = m_current_path.emplace_back(order, btree_node::k_unused_pointer);

  root.this_pointer = new_node_pointer();
  root.is_leaf = true;
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::insert_sorted(const sha1_t& sha1)
{
  if (current_node().is_full()) {
    split_node(sha1);
  } else {
    current_node().push_back(sha1);
  }
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::finalize_inserting()
{
  for (const auto& node : m_current_path) {
    this->write_node(node);
  }

  rebalance_tree();
}

template <typename DataStorage>
btree_node::pointer_t btree_sorted_keys_inserter<DataStorage>::new_node_pointer()
{
  return m_next_node_ptr++;
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::split_node(const sha1_t& sha1,
                                                         unsigned level_from_leafs)
{
  const auto is_root = (m_current_path.size() == 1u);
  if (is_root) {
    return split_root_and_grow(sha1, level_from_leafs);
  } else {

    this->write_node(current_node());
    m_current_path.pop_back();

    auto& parent_node = current_node();
    if (parent_node.is_full()) {
      return split_node(sha1, level_from_leafs + 1);
    } else {
      parent_node.insert(sha1);
      return create_children_till_leaf(level_from_leafs);
    }
  }
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::split_root_and_grow(const sha1_t& sha1,
                                                                  unsigned level_from_leafs)
{
  const auto new_root_ptr = new_node_pointer();

  auto& old_root = current_node();
  const auto old_root_ptr = old_root.this_pointer;

  old_root.parent_pointer = new_root_ptr;
  this->write_node(old_root);
  m_current_path.pop_back();

  auto& new_root = m_current_path.emplace_back(this->order(), btree_node::k_unused_pointer);
  new_root.insert(sha1);
  new_root.pointers[0] = old_root_ptr;
  new_root.this_pointer = new_root_ptr;
  new_root.is_leaf = false;

  create_children_till_leaf(level_from_leafs);

  this->set_root_ptr(new_root_ptr);
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::create_children_till_leaf(unsigned level_from_leafs)
{
  const auto is_on_leaf_level = (level_from_leafs == 0u);

  const auto parent_ptr = current_node().this_pointer;
  const auto parent_place_in_current_path = m_current_path.size() - 1u;

  auto& node = m_current_path.emplace_back(this->order(), parent_ptr);
  node.this_pointer = new_node_pointer();
  node.keys_count = 0u;
  node.is_leaf = is_on_leaf_level;

  auto& parent = m_current_path[parent_place_in_current_path];
  parent.pointers[parent.keys_count] = node.this_pointer;

  if (is_on_leaf_level) {
    return;
  }

  return create_children_till_leaf(level_from_leafs - 1u);
}

template <typename DataStorage>
btree_node& btree_sorted_keys_inserter<DataStorage>::current_node()
{
  return m_current_path.back();
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::rebalance_tree()
{
}

}
