#include "btree_node.hpp"

#include <algorithm>

namespace okon {
btree_node::btree_node(uint32_t order, pointer_t parent_ptr)
  : pointers(order + 1, k_unused_pointer)
  , keys{ order }
  , parent_pointer{ parent_ptr }
{
}

uint64_t btree_node::binary_size(uint32_t order)
{
  return sizeof(is_leaf) + sizeof(keys_count) + binary_pointers_size(order) +
    binary_keys_size(order) + sizeof(parent_pointer);
}

uint64_t btree_node::binary_pointers_size(uint32_t order)
{
  return (order + 1) * sizeof(pointer_t);
}

uint64_t btree_node::binary_keys_size(uint32_t order)
{
  return order * sizeof(sha1_t);
}

uint32_t btree_node::insert(const sha1_t& sha1)
{
  const auto place = place_for(sha1);

  if (keys_count > 0) {
    for (auto i = keys_count - 1; i >= place; --i) {
      keys[i + 1] = keys[i];
    }
  }

  keys[place] = sha1;
  ++keys_count;

  return place;
}

void btree_node::push_back(const sha1_t& sha1)
{
  keys[keys_count] = sha1;
  ++keys_count;
}

uint32_t btree_node::order() const
{
  return keys.size();
}

uint32_t btree_node::place_for(const sha1_t& sha1) const
{
  const auto keys_end = std::next(std::cbegin(keys), keys_count);
  const auto found = std::lower_bound(std::cbegin(keys), keys_end, sha1);
  return std::distance(std::cbegin(keys), found);
}

bool btree_node::is_full() const
{
  return keys_count == order();
}

bool btree_node::contains(const sha1_t& sha1) const
{
  const auto keys_end = std::next(std::cbegin(keys), keys_count);
  const auto found = std::lower_bound(std::cbegin(keys), keys_end, sha1);

  if (found == std::cend(keys)) {
    return false;
  }

  return *found == sha1;
}

btree_node::pointer_t btree_node::rightmost_pointer() const
{
  return pointers[keys_count];
}

uint32_t btree_node::children_count() const
{
  return keys_count + 1u;
}

std::optional<btree_node::pointer_t> btree_node::get_child_pointer_prev_of(
  btree_node::pointer_t ptr) const
{
  const auto ptr_index = index_of_child_pointer(ptr);
  if (!ptr_index.has_value() || *ptr_index == 0u) {
    return std::nullopt;
  }

  return pointers[*ptr_index - 1u];
}

std::optional<unsigned> btree_node::index_of_child_pointer(btree_node::pointer_t ptr) const
{
  const auto found = std::find(std::cbegin(pointers), std::cend(pointers), ptr);
  if (found == std::cend(pointers)) {
    return std::nullopt;
  }

  return static_cast<unsigned>(std::distance(std::cbegin(pointers), found));
}
}
