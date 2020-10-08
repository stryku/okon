#pragma once

#include <gmock/gmock.h>

#define GTEST_COUT std::cerr << "[          ] [ INFO ] "

namespace okon::test {
constexpr auto k_file_metadata_size{ sizeof(uint32_t) +
                                     sizeof(btree_node::pointer_t) }; // t + root pointer
constexpr std::string_view k_empty_sha1{ "0000000000000000000000000000000000000000" };
constexpr std::string_view k_uninteresting_sha1{ "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" };
constexpr btree_node::order_t k_test_order_value{ 2u };

#pragma pack(push, 1)
struct storage_metadata
{
  uint32_t order{};
  btree_node::pointer_t root_ptr;
};

template <btree_node::order_t Order>
struct binary_node
{
  bool is_leaf;
  uint32_t keys_count;
  std::array<btree_node::pointer_t, Order + 1> pointers;
  std::array<sha1_t, Order> keys;
  btree_node::pointer_t parent_pointer;
};
#pragma pack(pop)

template <btree_node::order_t Order>
struct decoded_storage
{
  storage_metadata metadata;
  std::vector<binary_node<Order>> nodes;
};

inline std::vector<uint8_t> to_storage(btree_node::order_t order, btree_node::pointer_t root_ptr,
                                       const std::vector<btree_node>& nodes)
{
  const auto pointers_size = btree_node::binary_pointers_size(order);
  const auto keys_size = btree_node::binary_keys_size(order);

  std::vector<uint8_t> result;
  result.resize(k_file_metadata_size + nodes.size() * btree_node::binary_size(order));

  auto result_ptr = &result[0];

  auto write = [result_ptr](const void* src, unsigned size) mutable {
    std::memcpy(result_ptr, src, size);
    std::advance(result_ptr, size);
  };

  write(&order, sizeof(order));
  write(&root_ptr, sizeof(root_ptr));

  for (const auto& node : nodes) {
    write(reinterpret_cast<const char*>(&node.is_leaf), sizeof(node.is_leaf));
    write(reinterpret_cast<const char*>(&node.keys_count), sizeof(node.keys_count));
    write(reinterpret_cast<const char*>(node.pointers.data()), pointers_size);
    write(reinterpret_cast<const char*>(node.keys.data()), keys_size);
    write(reinterpret_cast<const char*>(&node.parent_pointer), sizeof(node.parent_pointer));
  }

  return result;
}

template <btree_node::order_t Order>
inline decoded_storage<Order> decode_storage(const std::vector<uint8_t>& binary_data)
{
  using node_t = binary_node<Order>;

  decoded_storage<Order> s;

  const auto* ptr = binary_data.data();

  s.metadata = *(reinterpret_cast<const storage_metadata*>(ptr));
  std::advance(ptr, sizeof(storage_metadata));

  const auto node_count = (binary_data.size() - sizeof(storage_metadata)) / sizeof(node_t);

  for (auto i = 0u; i < node_count; ++i) {
    node_t node = *(reinterpret_cast<const node_t*>(ptr));
    s.nodes.push_back(node);
    std::advance(ptr, sizeof(node_t));
  }

  return s;
}

inline btree_node make_node(bool is_leaf, uint32_t keys_count,
                            std::vector<btree_node::pointer_t> pointers,
                            const std::vector<std::string_view>& keys,
                            btree_node::pointer_t parent_ptr,
                            btree_node::order_t order = k_test_order_value)
{
  btree_node node{ order, 0u };

  node.is_leaf = is_leaf;
  node.keys_count = keys_count;
  node.pointers = std::move(pointers);

  node.keys.clear();
  std::transform(std::cbegin(keys), std::cend(keys), std::back_inserter(node.keys),
                 [](std::string_view sha1) { return string_sha1_to_binary(sha1); });

  node.parent_pointer = parent_ptr;

  return node;
}

template <btree_node::order_t Order, typename Result, typename Expected>
bool storage_eq_impl(Result&& result, Expected&& expected)
{
  const auto uninteresting_key = string_sha1_to_binary(k_uninteresting_sha1);

  const auto expected_storage = decode_storage<Order>(expected);
  const auto result_storage = decode_storage<Order>(result);

  auto ok{ true };

  if (result_storage.metadata.order != expected_storage.metadata.order) {
    GTEST_COUT << "Result Order (" << result_storage.metadata.order << ") != Expected Order ("
               << expected_storage.metadata.order << "); ";
    ok = false;
  }

  if (result_storage.metadata.root_ptr != expected_storage.metadata.root_ptr) {
    GTEST_COUT << "Result Root (" << result_storage.metadata.root_ptr << ") != Expected Root ("
               << expected_storage.metadata.root_ptr << ")\n";
    ok = false;
  }

  if (result_storage.nodes.size() != expected_storage.nodes.size()) {
    GTEST_COUT << "Result Node Count (" << result_storage.nodes.size()
               << ") != Expected Node Count (" << expected_storage.nodes.size() << ")\n";

    return false;
  }

  for (auto i = 0u; i < expected_storage.nodes.size(); ++i) {
    const auto& exp = expected_storage.nodes[i];
    const auto& result = result_storage.nodes[i];

    if (result.is_leaf != exp.is_leaf) {
      GTEST_COUT << "Result Node[" << i << "].IsLeaf (" << result.is_leaf << ") != Expected Node["
                 << i << "].IsLeaf (" << exp.is_leaf << ")\n";
      ok = false;
    }

    if (result.keys_count != exp.keys_count) {
      GTEST_COUT << "Result Node[" << i << "].KeysCount (" << result.keys_count
                 << ") != Expected Node[" << i << "].KeysCount (" << exp.keys_count << ")\n";
      ok = false;
    }

    for (auto j = 0u; j < exp.pointers.size(); ++j) {
      if (result.pointers[j] != exp.pointers[j]) {
        GTEST_COUT << "Result Node[" << i << "].Ptrs[" << j << "] (" << result.pointers[j]
                   << ") != Expected Node[" << i << "].Ptrs[" << j << "] (" << exp.pointers[j]
                   << ")\n";
        ok = false;
      }
    }

    for (auto j = 0u; j < exp.keys.size(); ++j) {
      if (exp.keys[j] != uninteresting_key && result.keys[j] != exp.keys[j]) {
        GTEST_COUT << "Result Node[" << i << "].Keys[" << j << "] ("
                   << binary_sha1_to_string(result.keys[j]) << ") != Expected Node[" << i
                   << "].Keys[" << j << "] (" << binary_sha1_to_string(exp.keys[j]) << ")\n";
        ok = false;
      }
    }

    if (result.parent_pointer != exp.parent_pointer) {
      GTEST_COUT << "Result Node[" << i << "].Parent (" << result.parent_pointer
                 << ") != Expected Node[" << i << "].Parent (" << exp.parent_pointer << ")\n";
      ok = false;
    }
  }

  return ok;
}

MATCHER_P(StorageEq, expected, "")
{
  return storage_eq_impl<k_test_order_value>(arg, expected);
}

MATCHER_P(StorageOrder3Eq, expected, "")
{
  return storage_eq_impl<3u>(arg, expected);
}
}
