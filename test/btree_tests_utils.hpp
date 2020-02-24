#pragma once

#include <gmock/gmock.h>

#define GTEST_COUT std::cerr << "[          ] [ INFO ] "

namespace pwned::test {
constexpr auto k_file_metadata_size{ sizeof(uint32_t) +
                                     sizeof(btree_node::pointer_t) }; // t + root pointer
constexpr std::string_view k_empty_sha1{ "0000000000000000000000000000000000000000" };
constexpr uint32_t k_test_t_value{ 2u };

#pragma pack(push, 1)
struct storage_metadata
{
  uint32_t t{};
  btree_node::pointer_t root_ptr;
};

template <unsigned T>
struct binary_node
{
  bool is_leaf;
  uint32_t keys_count;
  std::array<btree_node::pointer_t, 2 * T> pointers;
  std::array<sha1_t, 2 * T - 1> keys;
  btree_node::pointer_t parent_pointer;
};
#pragma pack(pop)

template <unsigned T>
struct decoded_storage
{
  storage_metadata metadata;
  std::vector<binary_node<T>> nodes;
};

inline std::vector<uint8_t> to_storage(uint32_t t, btree_node::pointer_t root_ptr,
                                       const std::vector<btree_node>& nodes)
{
  const auto pointers_size = btree_node::binary_pointers_size(t);
  const auto keys_size = btree_node::binary_keys_size(t);

  std::vector<uint8_t> result;
  result.resize(k_file_metadata_size + nodes.size() * btree_node::binary_size(t));

  auto result_ptr = &result[0];

  auto write = [result_ptr](const void* src, unsigned size) mutable {
    std::memcpy(result_ptr, src, size);
    std::advance(result_ptr, size);
  };

  write(&t, sizeof(t));
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

template <unsigned T>
inline decoded_storage<T> decode_storage(const std::vector<uint8_t>& binary_data)
{
  using node_t = binary_node<T>;

  decoded_storage<T> s;

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
                            btree_node::pointer_t parent_ptr)
{
  btree_node node{ k_test_t_value, 0u };

  node.is_leaf = is_leaf;
  node.keys_count = keys_count;
  node.pointers = std::move(pointers);

  node.keys.clear();
  std::transform(std::cbegin(keys), std::cend(keys), std::back_inserter(node.keys),
                 [](std::string_view sha1) { return to_sha1(sha1); });

  node.parent_pointer = parent_ptr;

  return node;
}

MATCHER_P(StorageEq, expected, "")
{
  const auto expected_storage = decode_storage<k_test_t_value>(expected);
  const auto result_storage = decode_storage<k_test_t_value>(arg);

  auto ok{ true };

  if (result_storage.metadata.t != expected_storage.metadata.t) {
    GTEST_COUT << "Result T (" << result_storage.metadata.t << ") != Expected T ("
               << expected_storage.metadata.t << "); ";
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
      if (result.keys[j] != exp.keys[j]) {
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
}
