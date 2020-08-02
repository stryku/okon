#pragma once

#include "btree_base.hpp"
#include "sha1_utils.hpp"

#include <cmath>
#include <unordered_map>
#include <unordered_set>
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
  void create_nodes_to_fulfill_b_tree(btree_node::pointer_t current_node_ptr, unsigned level);
  void rebalance_keys();

  void rebalance_keys_in_node(btree_node& node);

  void initialize_current_key_providing_path();

  std::optional<btree_node> get_node_with_greatest_key();

  std::optional<btree_node> get_rightmost_not_visited_node(btree_node::pointer_t start_node_ptr);
  std::optional<btree_node> get_rightmost_not_visited_node2(btree_node::pointer_t start_node_ptr);

  unsigned get_number_of_keys_in_node_during_rebalance(const btree_node& node) const;
  unsigned get_number_of_keys_taken_from_node_during_rebalance(const btree_node& node) const;

  sha1_t get_greatest_not_visited_key();

  struct keys_provider_node_data
  {
    btree_node node;
    btree_node::pointer_t child_index{ btree_node::k_unused_pointer };
  };

  keys_provider_node_data& current_key_providing_node();

private:
  btree_node::pointer_t m_next_node_ptr{ 0u };
  std::vector<btree_node> m_current_path;
  unsigned m_tree_height{ 0u };

  std::unordered_set<btree_node::pointer_t> m_visited_nodes;

  std::unordered_set<btree_node::pointer_t> m_nodes_created_during_rebalancing;
  std::unordered_map<btree_node::pointer_t, unsigned> m_keys_took_by_provider;

  std::vector<keys_provider_node_data> m_current_key_providing_path;
};

template <typename DataStorage>
btree_sorted_keys_inserter<DataStorage>::btree_sorted_keys_inserter(DataStorage& storage,
                                                                    btree_node::order_t order)
  : btree_base<DataStorage>{ storage, order }
  , m_tree_height{ 1u }
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
  ++m_tree_height;
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
  initialize_current_key_providing_path();
  if (m_current_key_providing_path.empty()) {
    return;
  }

  create_nodes_to_fulfill_b_tree(this->root_ptr(), 1u);
  rebalance_keys();
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::create_nodes_to_fulfill_b_tree(
  btree_node::pointer_t current_node_ptr, unsigned level)
{
  auto node = this->read_node(current_node_ptr);
  if (node.is_leaf) {
    return;
  }

  if (current_node_ptr == this->root_ptr()) {
    create_nodes_to_fulfill_b_tree(node.rightmost_pointer(), level + 1u);
    return;
  }

  const auto expected_min_number_of_children = this->expected_min_number_of_keys() + 1u;

  // While sorted inserting number of children is equal to number of keys, not number of keys + 1.
  if (node.keys_count >= expected_min_number_of_children) {
    return;
  }

  const auto children_are_leafs = (level + 1 == this->m_tree_height);

  for (auto child_index = node.children_count(); child_index < expected_min_number_of_children;
       ++child_index) {

    auto child = btree_node{ this->order(), node.this_pointer };
    child.this_pointer = new_node_pointer();
    child.keys_count = 0u;
    child.is_leaf = children_are_leafs;

    m_nodes_created_during_rebalancing.insert(child.this_pointer);

    this->write_node(child);

    node.pointers[child_index] = child.this_pointer;
    if (!children_are_leafs) {
      create_nodes_to_fulfill_b_tree(child.this_pointer, level + 1u);
    }
  }

  this->write_node(node);
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::rebalance_keys()
{
  auto root = this->read_node(this->root_ptr());
  rebalance_keys_in_node(root);
  //  // No node that needs to be check found.
  //  auto node = get_rightmost_not_visited_node2(this->root_ptr());
  //  if (!node.has_value()) {
  //    return;
  //  }
  //
  //  rebalance_keys_in_node(*node);
}

template <typename DataStorage>
unsigned
btree_sorted_keys_inserter<DataStorage>::get_number_of_keys_taken_from_node_during_rebalance(
  const btree_node& node) const
{
  const auto found = m_keys_took_by_provider.find(node.this_pointer);
  if (found == std::cend(m_keys_took_by_provider)) {
    return 0u;
  }

  return found->second;
}

template <typename DataStorage>
unsigned btree_sorted_keys_inserter<DataStorage>::get_number_of_keys_in_node_during_rebalance(
  const btree_node& node) const
{
  return node.keys_count - get_number_of_keys_taken_from_node_during_rebalance(node);
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::rebalance_keys_in_node(btree_node& node)
{
  if (node.is_leaf) {
    return;
  }
  const auto number_of_keys = this->get_number_of_keys_in_node_during_rebalance(node);

  bool children_are_leafs = false;

  for (int key_index = static_cast<int>(this->expected_min_number_of_keys() - 1);
       key_index >= static_cast<int>(number_of_keys); --key_index) {
    if (!children_are_leafs) {
      const auto child_ptr = node.pointers[key_index];
      auto child = this->read_node(child_ptr);
      if (child.is_leaf) {
        children_are_leafs = true;
      } else {
        rebalance_keys_in_node(child);
      }
    }

    const auto key = this->get_greatest_not_visited_key();
    node.keys[key_index] = key;
    ++node.keys_count;
  }

  if (!children_are_leafs) {
    const auto child_ptr = node.pointers[number_of_keys];
    auto child = this->read_node(child_ptr);
    rebalance_keys_in_node(child);
  }

  const auto new_number_of_keys = this->get_number_of_keys_in_node_during_rebalance(node);

  if (new_number_of_keys < this->expected_min_number_of_keys()) {
    node.keys_count = new_number_of_keys;
    for (int key_index = static_cast<int>(this->expected_min_number_of_keys() - 1);
         key_index >= static_cast<int>(new_number_of_keys); --key_index) {
      if (!children_are_leafs) {
        const auto child_ptr = node.pointers[key_index];
        auto child = this->read_node(child_ptr);
        if (child.is_leaf) {
          children_are_leafs = true;
        } else {
          rebalance_keys_in_node(child);
        }
      }

      const auto key = this->get_greatest_not_visited_key();
      node.keys[key_index] = key;
      ++node.keys_count;
    }
  }

  if (number_of_keys < this->expected_min_number_of_keys() ||
      new_number_of_keys < this->expected_min_number_of_keys()) {
    this->write_node(node);
  }
}

template <typename DataStorage>
std::optional<btree_node> btree_sorted_keys_inserter<DataStorage>::get_rightmost_not_visited_node2(
  btree_node::pointer_t start_node_ptr)
{
  auto node = this->read_node(start_node_ptr);

  while (!node.is_leaf) {
    const auto rightmost_child_ptr = (m_nodes_created_during_rebalancing.find(node.this_pointer) ==
                                      std::cend(m_nodes_created_during_rebalancing))
      ? node.rightmost_pointer()
      : node.pointers[this->expected_min_number_of_keys()];

    node = this->read_node(rightmost_child_ptr);
  }

  // If the found leaf is root, nothing to return.
  if (node.this_pointer == this->root_ptr()) {
    return std::nullopt;
  }

  // We don't care about the leafs, so go one node up.
  node = this->read_node(node.parent_pointer);

  // If the found node is not visited, return it.
  if (m_visited_nodes.find(node.this_pointer) == std::cend(m_visited_nodes)) {
    return std::move(node);
  }

  // If the node is root, nothing to return.
  if (node.this_pointer == this->root_ptr()) {
    return std::nullopt;
  }

  const auto parent = this->read_node(node.parent_pointer);

  auto sibling_ptr = parent.get_child_pointer_prev_of(node.this_pointer);
  while (sibling_ptr.has_value()) {
    if (m_visited_nodes.find(*sibling_ptr) == std::cend(m_visited_nodes)) {
      auto rightmost_from_sibling = get_rightmost_not_visited_node(*sibling_ptr);
      if (rightmost_from_sibling.has_value()) {
        return std::move(rightmost_from_sibling);
      }
    }

    sibling_ptr = parent.get_child_pointer_prev_of(*sibling_ptr);
  }

  return std::nullopt;
}

template <typename DataStorage>
std::optional<btree_node> btree_sorted_keys_inserter<DataStorage>::get_rightmost_not_visited_node(
  btree_node::pointer_t start_node_ptr)
{
  auto node = this->read_node(start_node_ptr);

  // Get rightmost leaf
  while (!node.is_leaf) {
    node = this->read_node(node.rightmost_pointer());
  }

  // If the found leaf is root, nothing to return.
  if (node.this_pointer == this->root_ptr()) {
    return std::nullopt;
  }

  // We don't care about the leafs, so go one node up.
  node = this->read_node(node.parent_pointer);

  // If the found node is not visited, return it.
  if (m_visited_nodes.find(node.this_pointer) == std::cend(m_visited_nodes)) {
    return std::move(node);
  }

  // If the node is root, nothing to return.
  if (node.this_pointer == this->root_ptr()) {
    return std::nullopt;
  }

  const auto parent = this->read_node(node.parent_pointer);

  auto sibling_ptr = parent.get_child_pointer_prev_of(node.this_pointer);
  while (sibling_ptr.has_value()) {
    if (m_visited_nodes.find(*sibling_ptr) == std::cend(m_visited_nodes)) {
      auto rightmost_from_sibling = get_rightmost_not_visited_node(*sibling_ptr);
      if (rightmost_from_sibling.has_value()) {
        return std::move(rightmost_from_sibling);
      }
    }

    sibling_ptr = parent.get_child_pointer_prev_of(*sibling_ptr);
  }

  return std::nullopt;
}

template <typename DataStorage>
sha1_t btree_sorted_keys_inserter<DataStorage>::get_greatest_not_visited_key()
{
  auto& current = current_key_providing_node();

  const auto number_of_keys = get_number_of_keys_in_node_during_rebalance(current.node);
  ++m_keys_took_by_provider[current.node.this_pointer];
  const auto key = current.node.keys[number_of_keys - 1u];

  if (current.node.is_leaf) {
    if (number_of_keys == 1u) {
      const auto taken_keys_count =
        get_number_of_keys_taken_from_node_during_rebalance(current.node);
      if (taken_keys_count > 0u) {
        current.node.keys_count -= taken_keys_count;
        this->write_node(current.node);
      }
      m_current_key_providing_path.pop_back();
    }
    return key;
  }

  // Can go to left sibling
  if (current.child_index >= 1u) {
    --current.child_index;

    bool is_leaf{ false };

    do {
      auto& cur = current_key_providing_node();

      auto node = this->read_node(cur.node.pointers[cur.child_index]);
      is_leaf = node.is_leaf;
      const auto child_index = node.keys_count;
      keys_provider_node_data data{ std::move(node), child_index };
      m_current_key_providing_path.emplace_back(std::move(data));
    } while (!is_leaf);

    return key;
  }

  this->write_node(current.node);
  m_current_key_providing_path.pop_back();

  return key;

  //  auto& current = m_current_key_providing_path.back();
  //  const auto number_of_keys = get_number_of_keys_in_node_during_rebalance(current.node);
  //  if (number_of_keys == 0u) {
  //    // goto next node
  //
  //    if (current.this_pointer == this->root_ptr()) {
  //      if (current.child_index == 0u) {
  //        // Todo: should not happen
  //      }
  //
  //      --current.child_index;
  //
  //      std::vector<btree_node> nodes_path;
  //
  //      bool is_leaf{ false };
  //
  //      do {
  //        auto& cur = m_current_key_providing_path.back();
  //
  //        auto node = this->read_node(cur.node.pointers[cur.child_index]);
  //        is_leaf = node.is_leaf;
  //        const auto child_index = node.keys_count;
  //        m_current_key_providing_path.emplace_back({ std::move(node), child_index });
  //      } while (!is_leaf);
  //    } else {
  //    }
  //  }
  //
  //  ++m_keys_took_by_provider[current.this_pointer];
  //  return current.keys[number_of_keys - 1u];
}

template <typename DataStorage>
std::optional<btree_node> btree_sorted_keys_inserter<DataStorage>::get_node_with_greatest_key()
{
  std::vector<btree_node> nodes_path;

  for (auto node = this->read_node(this->root_ptr()); !node.is_leaf;
       node = this->read_node(node.rightmost_pointer())) {
    nodes_path.emplace_back(std::move(node));
  }

  while (!nodes_path.empty() && nodes_path.back().keys_count == 0u) {
    nodes_path.pop_back();
  }

  if (nodes_path.empty()) {
    return std::nullopt;
  }

  return std::move(nodes_path.back());
}

template <typename DataStorage>
void btree_sorted_keys_inserter<DataStorage>::initialize_current_key_providing_path()
{
  std::vector<btree_node> nodes_path;

  auto ptr = this->root_ptr();

  while (true) {
    auto node = this->read_node(ptr);
    const auto is_leaf = node.is_leaf;
    ptr = node.rightmost_pointer();
    nodes_path.emplace_back(std::move(node));

    if (is_leaf) {
      break;
    }
  }

  while (!nodes_path.empty() && nodes_path.back().keys_count == 0u) {
    nodes_path.pop_back();
  }

  if (nodes_path.empty()) {
    return;
  }

  for (auto& node : nodes_path) {
    const auto child_index = node.keys_count;
    keys_provider_node_data data{ std::move(node), child_index };
    m_current_key_providing_path.emplace_back(std::move(data));
  }
}

template <typename DataStorage>
typename btree_sorted_keys_inserter<DataStorage>::keys_provider_node_data&
btree_sorted_keys_inserter<DataStorage>::current_key_providing_node()
{
  return m_current_key_providing_path.back();
}
}
