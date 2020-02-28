#include "btree.hpp"
#include "btree_tests_utils.hpp"
#include "memory_storage.hpp"

namespace okon::test {
TEST(Btree, Contains_RootWithoutKey_ReturnsFalse)
{
  const auto expected_nodes = { make_node(
    /*is_leaf=*/true, /*keys_count=*/1u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "0000000000000000000000000000000000000000", k_empty_sha1 }, btree_node::k_unused_pointer) };

  memory_storage storage;
  storage.m_storage = to_storage(k_test_order_value, 0u, expected_nodes);

  btree tree{ storage };
  const auto result =
    tree.contains(string_sha1_to_binary("1000000000000000000000000000000000000000"));
  EXPECT_FALSE(result);
}

TEST(Btree, Contains_TreeOfHeightTwoWithoutKey_ReturnsFalse)
{
  const auto expected_nodes = {
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 1u, 2u, btree_node::k_unused_pointer },
      { "2000000000000000000000000000000000000000", k_empty_sha1 }, btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, 0u },
      { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000" },
      btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, 0u },
      { "3000000000000000000000000000000000000000", "4000000000000000000000000000000000000000" },
      btree_node::k_unused_pointer)
  };

  memory_storage storage;
  storage.m_storage = to_storage(k_test_order_value, 0u, expected_nodes);

  btree tree{ storage };
  const auto result =
    tree.contains(string_sha1_to_binary("F000000000000000000000000000000000000000"));
  EXPECT_FALSE(result);
}

TEST(Btree, Contains_KeyInRoot_ReturnsTrue)
{
  const auto expected_nodes = { make_node(
    /*is_leaf=*/true, /*keys_count=*/1u,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer },
    { "0000000000000000000000000000000000000000", k_empty_sha1 }, btree_node::k_unused_pointer) };

  memory_storage storage;
  storage.m_storage = to_storage(k_test_order_value, 0u, expected_nodes);

  btree tree{ storage };
  const auto result =
    tree.contains(string_sha1_to_binary("0000000000000000000000000000000000000000"));
  EXPECT_TRUE(result);
}

TEST(Btree, Contains_KeyInLeftLeafChild_ReturnsTrue)
{
  const auto expected_nodes = {
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 1u, 2u, btree_node::k_unused_pointer },
      { "2000000000000000000000000000000000000000", k_empty_sha1 }, btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, 0u },
      { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000" },
      btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, 0u },
      { "3000000000000000000000000000000000000000", "4000000000000000000000000000000000000000" },
      btree_node::k_unused_pointer)
  };

  memory_storage storage;
  storage.m_storage = to_storage(k_test_order_value, 0u, expected_nodes);

  btree tree{ storage };
  const auto result =
    tree.contains(string_sha1_to_binary("1000000000000000000000000000000000000000"));
  EXPECT_TRUE(result);
}

TEST(Btree, Contains_KeyInRightLeafChild_ReturnsTrue)
{
  const auto expected_nodes = {
    make_node(
      /*is_leaf=*/false, /*keys_count=*/1u, { 1u, 2u, btree_node::k_unused_pointer },
      { "2000000000000000000000000000000000000000", k_empty_sha1 }, btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/2u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, 0u },
      { "0000000000000000000000000000000000000000", "1000000000000000000000000000000000000000" },
      btree_node::k_unused_pointer),
    make_node(
      /*is_leaf=*/true, /*keys_count=*/3u,
      { btree_node::k_unused_pointer, btree_node::k_unused_pointer, 0u },
      { "3000000000000000000000000000000000000000", "4000000000000000000000000000000000000000" },
      btree_node::k_unused_pointer)
  };

  memory_storage storage;
  storage.m_storage = to_storage(k_test_order_value, 0u, expected_nodes);

  btree tree{ storage };
  const auto result =
    tree.contains(string_sha1_to_binary("3000000000000000000000000000000000000000"));
  EXPECT_TRUE(result);
}

TEST(Btree, Contains_KeyInNonLeafChildOfRoot_ReturnsTrue)
{
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

  memory_storage storage;
  storage.m_storage = to_storage(k_test_order_value, root_ptr, expected_nodes);

  btree tree{ storage };
  const auto result =
    tree.contains(string_sha1_to_binary("E000000000000000000000000000000000000000"));
  EXPECT_TRUE(result);
}
}
