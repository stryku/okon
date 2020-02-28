#pragma once

#include "sha1_utils.hpp"

#include <cstdint>
#include <vector>

namespace okon {
class btree_node
{
public:
  using pointer_t = uint32_t;
  static constexpr auto k_unused_pointer = std::numeric_limits<pointer_t>::max();

  explicit btree_node(uint32_t order, pointer_t parent_ptr);

  static uint64_t binary_size(unsigned order);
  static uint64_t binary_pointers_size(unsigned order);
  static uint64_t binary_keys_size(unsigned order);

  uint32_t insert(const sha1_t& sha1);

  uint32_t order() const;
  uint32_t place_for(const sha1_t& sha1) const;
  bool contains(const sha1_t& sha1) const;

  bool is_full() const;

public:
  bool is_leaf{ false };
  pointer_t keys_count{ 0u };
  std::vector<pointer_t> pointers;
  std::vector<sha1_t> keys;
  pointer_t parent_pointer{ k_unused_pointer };

  pointer_t this_pointer{};
};
}
