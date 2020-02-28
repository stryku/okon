#include "btree.hpp"
#include "btree_tests_utils.hpp"
#include "memory_storage.hpp"

namespace okon::test {
TEST(Btree, KeyInRoot_Contains_ReturnsTrue)
{

  const auto expected_nodes = { make_node(
    true, 1,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
      btree_node::k_unused_pointer },
    { "0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", k_empty_sha1, k_empty_sha1 },
    btree_node::k_unused_pointer) };

  memory_storage storage;
  storage.m_storage = to_storage(k_test_order_value, 0u, expected_nodes);

  btree tree{ storage };

  const auto result = tree.contains(to_sha1("0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));

  EXPECT_TRUE(result);
}

TEST(Btree, NotExistingKey_Contains_ReturnsFalse)
{
  const auto expected_nodes = { make_node(
    true, 1,
    { btree_node::k_unused_pointer, btree_node::k_unused_pointer, btree_node::k_unused_pointer,
      btree_node::k_unused_pointer },
    { "0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", k_empty_sha1, k_empty_sha1 },
    btree_node::k_unused_pointer) };

  memory_storage storage;
  storage.m_storage = to_storage(k_test_order_value, 0u, expected_nodes);

  btree tree{ storage };

  const auto result = tree.contains(to_sha1("1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));

  EXPECT_FALSE(result);
}
}
