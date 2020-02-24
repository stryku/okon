#include "btree_node.hpp"

namespace okon {
btree_node::btree_node(unsigned t, pointer_t parent_ptr)
  : pointers(2 * t, k_unused_pointer)
  , keys{ 2 * t - 1 }
  , parent_pointer{ parent_ptr }
{
}

uint64_t btree_node::binary_size(unsigned t)
{
  return sizeof(is_leaf) + sizeof(keys_count) + binary_pointers_size(t) + binary_keys_size(t) +
    sizeof(parent_pointer);
}

uint64_t btree_node::binary_pointers_size(unsigned t)
{
  return 2 * t * sizeof(pointer_t);
}

uint64_t btree_node::binary_keys_size(unsigned t)
{
  return (2 * t - 1) * sizeof(sha1_t);
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

uint32_t btree_node::t() const
{
  return pointers.size() / 2;
}

uint32_t btree_node::place_for(const sha1_t& sha1) const
{
  const auto keys_end = std::next(std::cbegin(keys), keys_count);
  const auto found = std::lower_bound(std::cbegin(keys), keys_end, sha1);
  return std::distance(std::cbegin(keys), found);
}

bool btree_node::is_full() const
{
  return keys_count == 2 * t() - 1;
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
}
