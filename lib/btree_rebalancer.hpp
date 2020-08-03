#pragma once

#include "btree_base.hpp"
#include "sha1_utils.hpp"

#include <unordered_map>
#include <unordered_set>

namespace okon {

template <typename DataStorage>
class btree_rebalancer : public btree_base<DataStorage>
{
public:
  explicit btree_rebalancer(DataStorage& storage, btree_node::pointer_t next_node_ptr,
                            unsigned tree_height);

  void rebalance();

private:
  btree_node::pointer_t new_node_pointer();
  void create_nodes_to_fulfill_b_tree(btree_node::pointer_t current_node_ptr, unsigned level);
  void rebalance_keys();

  void rebalance_keys_in_node(btree_node& node);

  void initialize_current_key_providing_path();

  std::optional<btree_node> get_rightmost_not_visited_node(btree_node::pointer_t start_node_ptr);

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
  btree_node::pointer_t m_next_node_ptr;
  std::vector<btree_node> m_current_path;
  unsigned m_tree_height;

  std::unordered_set<btree_node::pointer_t> m_visited_nodes;

  std::unordered_set<btree_node::pointer_t> m_nodes_created_during_rebalancing;
  std::unordered_map<btree_node::pointer_t, unsigned> m_keys_took_by_provider;

  std::vector<keys_provider_node_data> m_current_key_providing_path;
  std::unordered_set<btree_node::pointer_t> m_nodes_written_during_rebalancing;
};

template <typename DataStorage>
btree_rebalancer<DataStorage>::btree_rebalancer(DataStorage& storage,
                                                btree_node::pointer_t next_node_ptr,
                                                unsigned tree_height)
  : btree_base<DataStorage>{ storage }
  , m_next_node_ptr{ next_node_ptr }
  , m_tree_height{ tree_height }
{
}

template <typename DataStorage>
void btree_rebalancer<DataStorage>::rebalance()
{
  initialize_current_key_providing_path();
  if (m_current_key_providing_path.empty()) {
    return;
  }

  create_nodes_to_fulfill_b_tree(this->root_ptr(), 1u);
  rebalance_keys();
}

template <typename DataStorage>
void btree_rebalancer<DataStorage>::create_nodes_to_fulfill_b_tree(
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
btree_node::pointer_t btree_rebalancer<DataStorage>::new_node_pointer()
{
  return m_next_node_ptr++;
}

template <typename DataStorage>
void btree_rebalancer<DataStorage>::rebalance_keys()
{
  auto root = this->read_node(this->root_ptr());
  rebalance_keys_in_node(root);

  for (auto& node : m_current_key_providing_path) {
    if (m_nodes_written_during_rebalancing.find(node.node.this_pointer) !=
        std::cend(m_nodes_written_during_rebalancing)) {
      continue;
    }

    if (node.node.keys_count != get_number_of_keys_in_node_during_rebalance(node.node)) {
      node.node.keys_count = get_number_of_keys_in_node_during_rebalance(node.node);
      this->write_node(node.node);
    }
  }
}

template <typename DataStorage>
void btree_rebalancer<DataStorage>::rebalance_keys_in_node(btree_node& node)
{
  if (node.is_leaf) {
    return;
  }
  const auto number_of_keys = this->get_number_of_keys_in_node_during_rebalance(node);

  bool children_are_leafs = false;

  const auto expected_number_of_keys =
    (node.this_pointer == this->root_ptr()) ? 1u : this->expected_min_number_of_keys();

  for (int key_index = static_cast<int>(expected_number_of_keys - 1);
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

  if (new_number_of_keys < expected_number_of_keys) {
    node.keys_count = new_number_of_keys;
    for (int key_index = static_cast<int>(expected_number_of_keys - 1);
         key_index >= static_cast<int>(new_number_of_keys); --key_index) {

      const auto key = this->get_greatest_not_visited_key();
      node.keys[key_index] = key;
      ++node.keys_count;

      if (!children_are_leafs) {
        const auto child_ptr = node.pointers[key_index];
        auto child = this->read_node(child_ptr);
        if (child.is_leaf) {
          children_are_leafs = true;
        } else {
          rebalance_keys_in_node(child);
        }
      }
    }
  }

  if (number_of_keys < this->expected_min_number_of_keys() ||
      new_number_of_keys < this->expected_min_number_of_keys()) {
    m_nodes_written_during_rebalancing.insert(node.this_pointer);
    this->write_node(node);
  }
}
template <typename DataStorage>
void btree_rebalancer<DataStorage>::initialize_current_key_providing_path()
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
std::optional<btree_node> btree_rebalancer<DataStorage>::get_rightmost_not_visited_node(
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
unsigned btree_rebalancer<DataStorage>::get_number_of_keys_taken_from_node_during_rebalance(
  const btree_node& node) const
{
  const auto found = m_keys_took_by_provider.find(node.this_pointer);
  if (found == std::cend(m_keys_took_by_provider)) {
    return 0u;
  }

  return found->second;
}

template <typename DataStorage>
unsigned btree_rebalancer<DataStorage>::get_number_of_keys_in_node_during_rebalance(
  const btree_node& node) const
{
  return node.keys_count - get_number_of_keys_taken_from_node_during_rebalance(node);
}

template <typename DataStorage>
sha1_t btree_rebalancer<DataStorage>::get_greatest_not_visited_key()
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

      while (current_key_providing_node().node.keys_count == 0u) {
        m_current_key_providing_path.pop_back();
      }
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
}

template <typename DataStorage>
typename btree_rebalancer<DataStorage>::keys_provider_node_data&
btree_rebalancer<DataStorage>::current_key_providing_node()
{
  return m_current_key_providing_path.back();
}
}