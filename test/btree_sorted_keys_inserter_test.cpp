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

  const std::vector<btree_node> expected_nodes = { make_node(
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

  const std::vector<btree_node> expected_nodes = { make_node(
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

  const std::vector<btree_node> expected_nodes = { left_child, root, right_child };
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

  const std::vector<btree_node> expected_nodes = { left_child, root, right_child };
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

  const std::vector<btree_node> expected_nodes = { left_child, root, middle_child, right_child };
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

  const std::vector<btree_node> expected_nodes = {
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
      /*is_leaf=*/true, /*keys_count=*/1u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "6000000000000000000000000000000000000000", k_uninteresting_sha1 }, 1u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 1u, 5u, btree_node::k_unused_pointer },
      { "7000000000000000000000000000000000000000", k_uninteresting_sha1 },
      btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 6u, 7u, btree_node::k_unused_pointer },
      { "8000000000000000000000000000000000000000", k_uninteresting_sha1 }, root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 5u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 5u)
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

  const std::vector<btree_node> expected_nodes = {
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
      { "8000000000000000000000000000000000000000", k_uninteresting_sha1 },
      btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 6u, 7u, btree_node::k_unused_pointer },
      { "9000000000000000000000000000000000000000", k_uninteresting_sha1 }, root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 5u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 5u)
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

  const std::vector<btree_node> expected_nodes = {
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
      /*is_leaf=*/false, /*keys_count=*/1u, { 10u, 11u, btree_node::k_unused_pointer },
      { "F300000000000000000000000000000000000000", k_uninteresting_sha1 }, 4u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 9u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 9u)
  };

  const auto expected_storage = to_storage(k_test_order_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsTillTreeHeight3AndAllFullRoot)
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
  tree.insert_sorted(string_sha1_to_binary("F100000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("F200000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto root_ptr = 4u;

  const std::vector<btree_node> expected_nodes = {
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
      { "8000000000000000000000000000000000000000", "F100000000000000000000000000000000000000" },
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
      /*is_leaf=*/true, /*keys_count=*/1u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "F000000000000000000000000000000000000000", k_uninteresting_sha1 }, 5u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 10u, 11u, btree_node::k_unused_pointer },
      { "F200000000000000000000000000000000000000", k_uninteresting_sha1 }, 4u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 9u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 9u)
  };

  const auto expected_storage = to_storage(k_test_order_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsTillTreeHeight4)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_order_value };

  tree.insert_sorted(string_sha1_to_binary("0000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0100000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0200000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0300000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0400000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0500000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0600000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0700000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0800000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0900000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0A00000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0B00000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0C00000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0D00000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0E00000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("0F00000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1000000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1100000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1200000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1300000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1400000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1500000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1600000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1700000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1800000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1900000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("1A00000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto root_ptr = 13u;

  const std::vector<btree_node> expected_nodes = {
    // 0
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "0000000000000000000000000000000000000000", "0100000000000000000000000000000000000000" },
      1u),

    // 1
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 0u, 2u, 3u },
      { "0200000000000000000000000000000000000000", "0500000000000000000000000000000000000000" },
      4u),

    // 2
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "0300000000000000000000000000000000000000", "0400000000000000000000000000000000000000" },
      1u),

    // 3
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "0600000000000000000000000000000000000000", "0700000000000000000000000000000000000000" },
      1u),

    // 4
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 1u, 5u, 9u },
      { "0800000000000000000000000000000000000000", "1100000000000000000000000000000000000000" },
      13u),

    // 5
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 6u, 7u, 8u },
      { "0B00000000000000000000000000000000000000", "0E00000000000000000000000000000000000000" },
      4u),

    // 6
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "0900000000000000000000000000000000000000", "0A00000000000000000000000000000000000000" },
      5u),

    // 7
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "0C00000000000000000000000000000000000000", "0D00000000000000000000000000000000000000" },
      5u),

    // 8
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "0F00000000000000000000000000000000000000", "1000000000000000000000000000000000000000" },
      5u),

    // 9
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 10u, 11u, 12u },
      { "1400000000000000000000000000000000000000", "1600000000000000000000000000000000000000" },
      4u),

    // 10
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "1200000000000000000000000000000000000000", "1300000000000000000000000000000000000000" },
      9u),

    // 11
    make_node(
      /*is_leaf=*/true, /*keys_count=*/1u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "1500000000000000000000000000000000000000", k_uninteresting_sha1 }, 9u),

    // 12
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 9u),

    // 13
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 4u, 14u, btree_node::k_unused_pointer },
      { "1700000000000000000000000000000000000000", k_uninteresting_sha1 },
      btree_node::k_unused_pointer),

    // 14
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 15u, 18u, btree_node::k_unused_pointer },
      { "1900000000000000000000000000000000000000", k_uninteresting_sha1 }, 13u),

    // 15
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 16u, 17u, btree_node::k_unused_pointer },
      { "1800000000000000000000000000000000000000", k_uninteresting_sha1 }, 14u),

    // 16
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 15u),

    // 17
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 15u),

    // 18
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 19u, 20u, btree_node::k_unused_pointer },
      { "1A00000000000000000000000000000000000000", k_uninteresting_sha1 }, 14u),

    // 19
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 18u),

    // 20
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1 }, 18u),
  };

  const auto expected_storage = to_storage(k_test_order_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, Order3_InsertionsTillTreeHeight3)
{
  constexpr auto tree_order{ 3u };

  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, tree_order };

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
  tree.finalize_inserting();

  const auto root_ptr = 5u;

  const std::vector<btree_node> expected_nodes = {
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000",
        "2000000000000000000000000000000000000000" },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/3u, { 0u, 2u, 3u, 4u },
      { "3000000000000000000000000000000000000000", "7000000000000000000000000000000000000000",
        "B000000000000000000000000000000000000000" },
      /*parent_ptr=*/root_ptr, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "4000000000000000000000000000000000000000", "5000000000000000000000000000000000000000",
        "6000000000000000000000000000000000000000" },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "8000000000000000000000000000000000000000", "9000000000000000000000000000000000000000",
        "A000000000000000000000000000000000000000" },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/1u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "C000000000000000000000000000000000000000", k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u,
      { 1u, 6u, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "D000000000000000000000000000000000000000", k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/btree_node::k_unused_pointer, tree_order),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 7u, 8u, 9u, btree_node::k_unused_pointer },
      { "E000000000000000000000000000000000000000", "F000000000000000000000000000000000000000",
        k_uninteresting_sha1 },
      /*parent_ptr=*/root_ptr, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/6u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/6u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/6u, tree_order),
  };

  const auto expected_storage = to_storage(tree_order, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageOrder3Eq(expected_storage));
}

TEST(BtreeSortedKeysInserter, Order3_InsertionsTillTreeHeight3AndNextChild)
{
  constexpr auto tree_order{ 3u };

  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, tree_order };

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
  tree.insert_sorted(string_sha1_to_binary("F100000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto root_ptr = 5u;

  const std::vector<btree_node> expected_nodes = {
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000",
        "2000000000000000000000000000000000000000" },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/3u, { 0u, 2u, 3u, 4u },
      { "3000000000000000000000000000000000000000", "7000000000000000000000000000000000000000",
        "B000000000000000000000000000000000000000" },
      /*parent_ptr=*/root_ptr, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "4000000000000000000000000000000000000000", "5000000000000000000000000000000000000000",
        "6000000000000000000000000000000000000000" },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "8000000000000000000000000000000000000000", "9000000000000000000000000000000000000000",
        "A000000000000000000000000000000000000000" },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "C000000000000000000000000000000000000000", "D000000000000000000000000000000000000000",
        k_uninteresting_sha1 },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u,
      { 1u, 6u, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "E000000000000000000000000000000000000000", k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/btree_node::k_unused_pointer, tree_order),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 7u, 8u, 9u, btree_node::k_unused_pointer },
      { "F000000000000000000000000000000000000000", "F100000000000000000000000000000000000000",
        k_uninteresting_sha1 },
      /*parent_ptr=*/root_ptr, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/6u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/6u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/6u, tree_order),
  };

  const auto expected_storage = to_storage(tree_order, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageOrder3Eq(expected_storage));
}

TEST(BtreeSortedKeysInserter, Order3_InsertionsTillTreeHeight3AndNextChildWithThreeKeys)
{
  constexpr auto tree_order{ 3u };

  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, tree_order };

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
  tree.insert_sorted(string_sha1_to_binary("F100000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("F200000000000000000000000000000000000000"));
  tree.insert_sorted(string_sha1_to_binary("F300000000000000000000000000000000000000"));
  tree.finalize_inserting();

  const auto root_ptr = 5u;

  const std::vector<btree_node> expected_nodes = {
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000",
        "2000000000000000000000000000000000000000" },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/3u, { 0u, 2u, 3u, 4u },
      { "3000000000000000000000000000000000000000", "7000000000000000000000000000000000000000",
        "B000000000000000000000000000000000000000" },
      /*parent_ptr=*/root_ptr, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "4000000000000000000000000000000000000000", "5000000000000000000000000000000000000000",
        "6000000000000000000000000000000000000000" },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "8000000000000000000000000000000000000000", "9000000000000000000000000000000000000000",
        "A000000000000000000000000000000000000000" },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "C000000000000000000000000000000000000000", "D000000000000000000000000000000000000000",
        "E000000000000000000000000000000000000000" },
      /*parent_ptr=*/1u, tree_order),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u,
      { 1u, 6u, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "F000000000000000000000000000000000000000", k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/btree_node::k_unused_pointer, tree_order),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/2u, { 7u, 8u, 9u, btree_node::k_unused_pointer },
      { "F200000000000000000000000000000000000000", "F300000000000000000000000000000000000000",
        k_uninteresting_sha1 },
      /*parent_ptr=*/root_ptr, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/1u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "F100000000000000000000000000000000000000", k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/6u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/6u, tree_order),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_uninteresting_sha1, k_uninteresting_sha1, k_uninteresting_sha1 },
      /*parent_ptr=*/6u, tree_order),
  };

  const auto expected_storage = to_storage(tree_order, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageOrder3Eq(expected_storage));
}
}
