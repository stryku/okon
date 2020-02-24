#include "btree_sorted_keys_inserter.hpp"

#include "btree_tests_utils.hpp"
#include "memory_storage.hpp"

#include <gmock/gmock.h>

namespace okon::test {
TEST(BtreeSortedKeysInserter, SingleInsertion_CreatesOneRootNode)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_t_value };

  tree.insert_sorted(to_sha1("0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.finalize_inserting();

  const auto expected_nodes = { make_node(
    true, 1,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
      btree_node::k_unused_pointer },
    { "0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", k_empty_sha1, k_empty_sha1 },
    btree_node::k_unused_pointer) };
  const auto expected_storage = to_storage(k_test_t_value, 0u, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsTillFullNode_CreatesOneRootNode)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_t_value };

  tree.insert_sorted(to_sha1("0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.finalize_inserting();

  const auto expected_nodes = { make_node(
    /*is_leaf=*/true, /*keys_count=*/3,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
      btree_node::k_unused_pointer },
    { "0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
      "2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
    btree_node::k_unused_pointer) };
  const auto expected_storage = to_storage(k_test_t_value, 0u, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsExceedFullNodeByOne_CreatesRootNodeWithTwoChildrenNode)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_t_value };

  tree.insert_sorted(to_sha1("0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("3AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.finalize_inserting();

  const auto left_child_ptr = 0u;
  const auto root_ptr = 1u;
  const auto right_child_ptr = 2u;

  const auto left_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/3u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
      btree_node::k_unused_pointer },
    { "0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
      "2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
    root_ptr);

  const auto root = make_node(
    /*is_leaf=*/false, /*keys_count=*/1u,
    { left_child_ptr, right_child_ptr, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "3AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", k_empty_sha1, k_empty_sha1 },
    btree_node::k_unused_pointer);

  const auto right_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/0u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
      btree_node::k_unused_pointer },
    { k_empty_sha1, k_empty_sha1, k_empty_sha1 }, root_ptr);

  const auto expected_nodes = { left_child, root, right_child };
  const auto expected_storage = to_storage(k_test_t_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsTillTwoFullChildren_CreatesRootNodeWithTwoChildrenNode)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_t_value };

  tree.insert_sorted(to_sha1("0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("3AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("5AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("6AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.finalize_inserting();

  const auto left_child_ptr = 0u;
  const auto root_ptr = 1u;
  const auto right_child_ptr = 2u;

  const auto left_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/3u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
      btree_node::k_unused_pointer },
    { "0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
      "2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
    root_ptr);

  const auto root = make_node(
    /*is_leaf=*/false, /*keys_count=*/1u,
    { left_child_ptr, right_child_ptr, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "3AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", k_empty_sha1, k_empty_sha1 },
    btree_node::k_unused_pointer);

  const auto right_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/3u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
      btree_node::k_unused_pointer },
    { "4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "5AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
      "6AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
    root_ptr);

  const auto expected_nodes = { left_child, root, right_child };
  const auto expected_storage = to_storage(k_test_t_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter,
     InsertionsExceedTwoFullChildrenByOne_CreatesRootNodeWithTwoChildrenNode)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_t_value };

  tree.insert_sorted(to_sha1("0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("3AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("5AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("6AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("7AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.finalize_inserting();

  const auto left_child_ptr = 0u;
  const auto root_ptr = 1u;
  const auto middle_child_ptr = 2u;
  const auto right_child_ptr = 3u;

  const auto left_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/3u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
      btree_node::k_unused_pointer },
    { "0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
      "2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
    root_ptr);

  const auto root = make_node(
    /*is_leaf=*/false, /*keys_count=*/2u,
    { left_child_ptr, middle_child_ptr, right_child_ptr, btree_node::k_unused_pointer },
    { "3AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "7AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
      k_empty_sha1 },
    btree_node::k_unused_pointer);

  const auto middle_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/3u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
      btree_node::k_unused_pointer },
    { "4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "5AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
      "6AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
    root_ptr);

  const auto right_child = make_node(
    /*is_leaf=*/true, /*keys_count=*/0u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
      btree_node::k_unused_pointer },
    { k_empty_sha1, k_empty_sha1, k_empty_sha1 }, root_ptr);

  const auto expected_nodes = { left_child, root, middle_child, right_child };
  const auto expected_storage = to_storage(k_test_t_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsTillTreeHeight3)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_t_value };

  tree.insert_sorted(to_sha1("0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("3AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("5AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("6AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("7AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("9AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("DAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("EAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("FAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.finalize_inserting();

  const auto root_ptr = 5u;

  const auto expected_nodes = {
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
      1u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/3u, { 0u, 2u, 3u, 4u },
      { "3AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "7AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
      root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "5AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "6AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
      1u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "9AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
      1u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "DAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "EAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
      1u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u,
      { 1u, 6u, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "FAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", k_empty_sha1, k_empty_sha1 },
      btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/0u,
      { 7u, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_empty_sha1, k_empty_sha1, k_empty_sha1 }, root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/0u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_empty_sha1, k_empty_sha1, k_empty_sha1 }, 6u)
  };

  const auto expected_storage = to_storage(k_test_t_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}

TEST(BtreeSortedKeysInserter, InsertionsTillTreeHeight3AndNextChild)
{
  memory_storage storage;
  btree_sorted_keys_inserter tree{ storage, k_test_t_value };

  tree.insert_sorted(to_sha1("0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("3AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("5AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("6AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("7AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("9AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("DAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("EAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("FAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.insert_sorted(to_sha1("FBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  tree.finalize_inserting();

  const auto root_ptr = 5u;

  const auto expected_nodes = {
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
      1u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/3u, { 0u, 2u, 3u, 4u },
      { "3AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "7AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
      root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "5AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "6AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
      1u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "9AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
      1u),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "DAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "EAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
      1u),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u,
      { 1u, 6u, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
      { "FAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", k_empty_sha1, k_empty_sha1 },
      btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/false, /*keys_count=*/0u,
      { 7u, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { k_empty_sha1, k_empty_sha1, k_empty_sha1 }, root_ptr),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/1u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
        btree_node::k_unused_pointer },
      { "FBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", k_empty_sha1, k_empty_sha1 }, 6u)
  };

  const auto expected_storage = to_storage(k_test_t_value, root_ptr, expected_nodes);

  EXPECT_THAT(storage.m_storage, StorageEq(expected_storage));
}
}
