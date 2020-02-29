#pragma once

#include "btree_base.hpp"
#include "btree_node.hpp"

namespace okon {
template <typename DataStorage>
class btree : public btree_base<DataStorage>
{
public:
  explicit btree(DataStorage& storage);

  bool contains(const sha1_t& sha1) const;

private:
  bool contains(const btree_node& node, const sha1_t& sha1) const;
  btree_node read_root() const;
};

template <typename DataStorage>
btree<DataStorage>::btree(DataStorage& storage)
  : btree_base<DataStorage>{ storage }
{
}

template <typename DataStorage>
bool btree<DataStorage>::contains(const sha1_t& sha1) const
{
  const auto node = read_root();
  return contains(node, sha1);
}

template <typename DataStorage>
bool btree<DataStorage>::contains(const btree_node& node, const sha1_t& sha1) const
{
  if (node.contains(sha1)) {
    return true;
  }

  if (node.is_leaf) {
    return false;
  }

  const auto place = node.place_for(sha1);
  const auto next_node_ptr = node.pointers[place];
  const auto next_node = this->read_node(next_node_ptr);
  return contains(next_node, sha1);
}

template <typename DataStorage>
btree_node btree<DataStorage>::read_root() const
{
  return this->read_node(this->root_ptr());
}
}
