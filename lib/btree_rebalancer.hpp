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
  void create_nodes_to_fulfill_b_tree(btree_node& node, unsigned current_level);
  void rebalance_keys();

  void rebalance_keys_in_node(btree_node& node);

  void initialize_current_key_providing_path();

  unsigned get_number_of_keys_in_node_during_rebalance(const btree_node& node) const;
  unsigned get_number_of_keys_taken_from_node_during_rebalance(const btree_node& node) const;

  sha1_t get_greatest_not_visited_key();

  struct keys_provider_path_part_data
  {
    btree_node node;
    btree_node::pointer_t child_index{ btree_node::k_unused_pointer };
  };

  keys_provider_path_part_data& current_key_providing_node();

private:
  btree_node::pointer_t m_next_node_ptr;
  unsigned m_tree_height;

  std::unordered_set<btree_node::pointer_t> m_nodes_created_during_rebalancing;
  std::unordered_map<btree_node::pointer_t, unsigned> m_keys_took_by_provider;

  std::vector<keys_provider_path_part_data> m_current_key_providing_path;
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
  const auto tree_is_empty = m_next_node_ptr == 0u;
  if (tree_is_empty) {
    return;
  }

  initialize_current_key_providing_path();

  {
    auto root_node = this->read_node(this->root_ptr());
    create_nodes_to_fulfill_b_tree(root_node, /*current_level=*/1u);
  }

  rebalance_keys();
}

template <typename DataStorage>
void btree_rebalancer<DataStorage>::create_nodes_to_fulfill_b_tree(btree_node& node,
                                                                   unsigned current_level)
{
  if (node.is_leaf) {
    return;
  }

  const auto children_are_leafs = (current_level + 1u == this->m_tree_height);
  const auto children_count = node.children_count();
  
  if (!children_are_leafs && children_count > 0u) {
    auto rightmost_child = this->read_node(node.rightmost_pointer());
    create_nodes_to_fulfill_b_tree(rightmost_child, current_level + 1u);
  }

  const auto expected_min_number_of_children = this->expected_min_number_of_keys(node) + 1u;

  // While sorted inserting number of children is equal to number of keys, not number of keys + 1.
  const auto has_enough_children = (children_count >= expected_min_number_of_children);
  if (has_enough_children) {
    return;
  }

  // Create missing children.
  for (auto child_index = children_count; child_index < expected_min_number_of_children;
       ++child_index) {
    auto child = btree_node{ this->order(), node.this_pointer };
    child.this_pointer = new_node_pointer();
    child.keys_count = 0u;
    child.is_leaf = children_are_leafs;

    m_nodes_created_during_rebalancing.insert(child.this_pointer);

    node.pointers[child_index] = child.this_pointer;

    // If the child is leaf, just write it out. If not, handle the child's subtree.
    if (children_are_leafs) {
      this->write_node(child);
    } else {
      create_nodes_to_fulfill_b_tree(child, current_level + 1u);
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

  const auto expected_number_of_keys = this->expected_min_number_of_keys(node);
  const auto number_of_keys_expected_after_rebalancing =
    std::max(node.keys_count, expected_number_of_keys);

  int key_index = static_cast<int>(number_of_keys_expected_after_rebalancing - 1);
  bool children_are_leafs = false;
  auto stopped_key_left_child_might_be_unbalanced = false;

  do {
    if (!children_are_leafs) {
      const auto child_ptr = node.pointers[key_index + 1u];
      auto child = this->read_node(child_ptr);
      if (child.is_leaf) {
        children_are_leafs = true;
      } else {
        rebalance_keys_in_node(child);
      }
    }

    const auto has_enough_keys =
      (this->get_number_of_keys_in_node_during_rebalance(node) >= key_index + 1);
    if (has_enough_keys) {
      break;
    }

    const auto key = this->get_greatest_not_visited_key();
    node.keys[key_index] = key;
    --key_index;

    stopped_key_left_child_might_be_unbalanced = true;
  } while (key_index >= 0);

  if (!children_are_leafs && stopped_key_left_child_might_be_unbalanced) {
    const auto stopped_key_left_child_ptr_index = key_index + 1u;
    const auto child_ptr = node.pointers[stopped_key_left_child_ptr_index];
    auto child = this->read_node(child_ptr);
    rebalance_keys_in_node(child);
  }

  const auto needs_write_out =
    (key_index + 1 < static_cast<int>(number_of_keys_expected_after_rebalancing));
  if (needs_write_out) {
    node.keys_count = number_of_keys_expected_after_rebalancing;
    m_nodes_written_during_rebalancing.insert(node.this_pointer);
    this->write_node(node);
  }
}

template <typename DataStorage>
void btree_rebalancer<DataStorage>::initialize_current_key_providing_path()
{
  std::vector<btree_node> nodes_path;

  // Go to rightmost leaf node.
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

  // Omit all empty nodes in the path.
  while (!nodes_path.empty() && nodes_path.back().keys_count == 0u) {
    nodes_path.pop_back();
  }

  for (auto& node : nodes_path) {
    const auto child_index = node.keys_count;
    keys_provider_path_part_data data{ std::move(node), child_index };
    m_current_key_providing_path.emplace_back(std::move(data));
  }
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

  const auto index_of_key_to_take = get_number_of_keys_in_node_during_rebalance(current.node) - 1u;
  ++m_keys_took_by_provider[current.node.this_pointer];
  const auto key = current.node.keys[index_of_key_to_take];

  const auto skip_empty_nodes_in_path = [this] {
    while (current_key_providing_node().node.keys_count == 0u) {
      m_current_key_providing_path.pop_back();
    }
  };

  if (current.node.is_leaf) {
    const auto no_keys_left_in_node = (index_of_key_to_take == 0u);
    if (no_keys_left_in_node) {
      const auto taken_keys_count =
        get_number_of_keys_taken_from_node_during_rebalance(current.node);
      if (taken_keys_count > 0u) {
        current.node.keys_count -= taken_keys_count;
        this->write_node(current.node);
      }

      skip_empty_nodes_in_path();
    }
    return key;
  }

  // The node is not a leaf. Try go down the left child subtree.
  const auto can_go_to_left_child = (current.child_index >= 1u);
  if (can_go_to_left_child) {
    --current.child_index;

    // Go to the rightmost node of the left child subtree.
    bool is_leaf{ false };

    do {
      auto& cur = current_key_providing_node();

      auto node = this->read_node(cur.node.pointers[cur.child_index]);
      is_leaf = node.is_leaf;
      const auto child_index = node.keys_count;
      keys_provider_path_part_data data{ std::move(node), child_index };
      m_current_key_providing_path.emplace_back(std::move(data));
    } while (!is_leaf);

    return key;
  }

  // Can not go to the left child (it does not exists). Go to the parent node.
  this->write_node(current.node);
  m_current_key_providing_path.pop_back();

  skip_empty_nodes_in_path();

  return key;
}

template <typename DataStorage>
typename btree_rebalancer<DataStorage>::keys_provider_path_part_data&
btree_rebalancer<DataStorage>::current_key_providing_node()
{
  return m_current_key_providing_path.back();
}
}
