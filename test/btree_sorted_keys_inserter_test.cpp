#include "btree_sorted_keys_inserter.hpp"

#include "btree_tests_utils.hpp"
#include "memory_storage.hpp"

#include <gmock/gmock.h>

namespace okon::test {
TEST(BtreeSortedKeysInserter, SingleInsertion_CreatesOneRootNode)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_order_value };

  tree.insert_sorted(string_sha1_to_binary("0000000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto expected_nodes = { make_node(
    /*is_leaf=*/true, /*keys_count=*/1u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "0000000000000000000000000000000000000000", k_empty_sha1 }, btree_node::k_unused_pointer) };
  const auto expected_storage = to_storage(k_test_order_value, 0u, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsTillFullNode_CreatesOneRootNode)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_order_value };

  tree.insert_sorted(string_sha1_to_binary("0000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1000000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto expected_nodes = { make_node(
    /*is_leaf=*/true, /*keys_count=*/2u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000" },
    btree_node::k_unused_pointer) };
  const auto expected_storage = to_storage(k_test_order_value, 0u, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsExceedFullNodeByOne_CreatesRootNodeWithTwoChildrenNode)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_order_value };

  tree.insert_sorted(string_sha1_to_binary("0000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("2000000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto left_child_ptr = 0u;
  const auto root_ptr = 1u;
  const auto right_child_ptr = 2u;

  const auto left_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/2u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000" },
    root_ptr);

  const auto root = make_node(
    /*is_leaf=*/false, /*keys_count=*/1u,
    { left_child_ptr, right_child_ptr, btree_node::k_unused_pointer },
    { "2000000000000000000000000000000000000000", k_empty_sha1 }, btree_node::k_unused_pointer);

  const auto right_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/0u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { k_empty_sha1, k_empty_sha1 }, root_ptr);

  const auto expected_nodes = { left_child, root, right_child };
  const auto expected_storage = to_storage(k_test_order_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsTillTwoFullChildren_CreatesRootNodeWithTwoChildrenNode)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_order_value };

  tree.insert_sorted(string_sha1_to_binary("0000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("2000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("3000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("4000000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto left_child_ptr = 0u;
  const auto root_ptr = 1u;
  const auto right_child_ptr = 2u;

  const auto left_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/2u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000" },
    root_ptr);

  const auto root = make_node(
    /*is_leaf=*/false, /*keys_count=*/1u,
    { left_child_ptr, right_child_ptr, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "2000000000000000000000000000000000000000", k_empty_sha1 }, btree_node::k_unused_pointer);

  const auto right_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/2u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "3000000000000000000000000000000000000000", "4000000000000000000000000000000000000000" },
    root_ptr);

  const auto expected_nodes = { left_child, root, right_child };
  const auto expected_storage = to_storage(k_test_order_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter,
     InsertionsExceedTwoFullChildrenByOne_CreatesRootNodeWithTwoChildrenNode)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_order_value };

  tree.insert_sorted(string_sha1_to_binary("0000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("2000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("3000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("4000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("5000000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto left_child_ptr = 0u;
  const auto root_ptr = 1u;
  const auto middle_child_ptr = 2u;
  const auto right_child_ptr = 3u;

  const auto left_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/2u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000" },
    root_ptr);

  const auto root = make_node(
    /*is_leaf=*/false, /*keys_count=*/2u, { left_child_ptr, middle_child_ptr, right_child_ptr },
    { "2000000000000000000000000000000000000000", "5000000000000000000000000000000000000000",
      k_empty_sha1 },
    btree_node::k_unused_pointer);

  const auto middle_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/2u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "3000000000000000000000000000000000000000", "4000000000000000000000000000000000000000" },
    root_ptr);

  const auto right_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/0u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { k_empty_sha1, k_empty_sha1 }, root_ptr);

  const auto expected_nodes = { left_child, root, middle_child, right_child };
  const auto expected_storage = to_storage(k_test_order_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsTillTreeHeight3)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_order_value };

  tree.insert_sorted(string_sha1_to_binary("0000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("2000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("3000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("4000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("5000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("6000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("7000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("8000000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto root_ptr = 4u;

  const auto expected_nodes = {
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000" },
      1u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 0u, 2u, 3u },
      { "2000000000000000000000000000000000000000", "5000000000000000000000000000000000000000" },
      root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "3000000000000000000000000000000000000000", "4000000000000000000000000000000000000000" },
      1u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "6000000000000000000000000000000000000000", "7000000000000000000000000000000000000000" },
      1u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 1u, 5u, btree_node::k_unused_pointer },
      { "8000000000000000000000000000000000000000", k_empty_sha1 }, btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/0u,
      { 6u, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_empty_sha1, k_empty_sha1 }, root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_empty_sha1, k_empty_sha1 }, 5u)
  };

  const auto expected_storage = to_storage(k_test_order_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsTillTreeHeight3AndNextChild)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_order_value };

  tree.insert_sorted(string_sha1_to_binary("0000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("2000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("3000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("4000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("5000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("6000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("7000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("8000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("9000000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto root_ptr = 4u;

  const auto expected_nodes = {
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000" },
      1u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 0u, 2u, 3u },
      { "2000000000000000000000000000000000000000", "5000000000000000000000000000000000000000" },
      root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "3000000000000000000000000000000000000000", "4000000000000000000000000000000000000000" },
      1u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "6000000000000000000000000000000000000000", "7000000000000000000000000000000000000000" },
      1u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 1u, 5u, btree_node::k_unused_pointer },
      { "8000000000000000000000000000000000000000", k_empty_sha1 }, btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/0u,
      { 6u, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_empty_sha1, k_empty_sha1 }, root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/1u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "9000000000000000000000000000000000000000", k_empty_sha1 }, 5u)
  };

  const auto expected_storage = to_storage(k_test_order_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsTillTreeHeight3AndAllRootChildren)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_order_value };

  tree.insert_sorted(string_sha1_to_binary("0000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("2000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("3000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("4000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("5000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("6000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("7000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("8000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("9000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("A000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("B000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("C000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("D000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("E000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("F000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("F100000000000000000000000000000000000000")); // 16
  tree.insert_sorted(string_sha1_to_binary("F200000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("F300000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto root_ptr = 4u;

  const auto expected_nodes = {
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000" },
      1u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 0u, 2u, 3u },
      { "2000000000000000000000000000000000000000", "5000000000000000000000000000000000000000" },
      root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "3000000000000000000000000000000000000000", "4000000000000000000000000000000000000000" },
      1u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "6000000000000000000000000000000000000000", "7000000000000000000000000000000000000000" },
      1u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 1u, 5u, 9u },
      { "8000000000000000000000000000000000000000", "F200000000000000000000000000000000000000" },
      btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 6u, 7u, 8u },
      { "B000000000000000000000000000000000000000", "E000000000000000000000000000000000000000" },
      root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "9000000000000000000000000000000000000000", "A000000000000000000000000000000000000000" },
      5u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "C000000000000000000000000000000000000000", "D000000000000000000000000000000000000000" },
      5u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "F000000000000000000000000000000000000000", "F100000000000000000000000000000000000000" },
      5u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/0u,
      { 10u, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_empty_sha1, k_empty_sha1 }, 4u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/1u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "F300000000000000000000000000000000000000", k_empty_sha1 }, 9u)
  };

  const auto expected_storage = to_storage(k_test_order_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}
}
