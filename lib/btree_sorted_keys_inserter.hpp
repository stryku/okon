#pragma once

#include "btree_base.hpp"
#include "sha1_utils.hpp"

namespace pwned {

template <typename DataStorage>
class btree_sorted_keys_inserter : public btree_base<DataStorage>
{
public:
  explicit btree_sorted_keys_inserter(DataStorage& storage, unsigned t)
    : btree_base<DataStorage>{ storage, t }
    , m_current_node{ t, btree_node::k_unused_pointer }
  {
    m_current_node.this_pointer = new_node_pointer();
    m_current_node.is_leaf = true;
  }

  void insert_sorted(const sha1_t& sha1)
  {
    if (m_current_node.is_full()) {
      m_current_node = split_node(m_current_node, sha1);
    } else {
      m_current_node.insert(sha1);
    }
  }

  void finalize_inserting()
  {
    this->write_node(m_current_node);
  }

private:
  btree_node::pointer_t new_node_pointer()
  {
    return m_next_node_ptr++;
  }

  btree_node split_node(btree_node& node, const sha1_t& sha1)
  {
    /*
     * 1. If root, move it to a new place and save pointer
     *  1.1 Create new root, insert sha1 and make it left pointer to old root new place.
     *
     * 2. If not root
     *  2.2 Go to parent.
     *  2.3 If parent have place for key. Add key and create new child. Make right of key point to
     * new child. 2.4 If parent have no place for key, split.
     */
    const auto is_root = (node.this_pointer == this->root_ptr());

    if (is_root) {

      const auto new_root_ptr = new_node_pointer();

      btree_node new_root{ this->t(), node.parent_pointer };
      new_root.insert(sha1);
      new_root.pointers[0] = node.this_pointer;
      new_root.this_pointer = new_root_ptr;
      new_root.is_leaf = false;

      // create_children_till_leaf() will write the new_root to file.
      const auto new_child_node =
        create_children_till_leaf(new_root, sha1, m_current_tree_height - 1);

      node.parent_pointer = new_root_ptr;
      this->write_node(node);

      this->set_root_ptr(new_root_ptr);

      ++m_current_tree_height;

      return new_child_node;
    } else {

      this->write_node(node);

      auto parent_node = this->read_node(node.parent_pointer);
      if (parent_node.is_full()) {
        return split_node(parent_node, sha1);
      } else {
        const auto right_empty_child_pointer = new_node_pointer();
        btree_node right_empty_child{ this->t(), parent_node.this_pointer };
        right_empty_child.this_pointer = right_empty_child_pointer;
        right_empty_child.is_leaf = node.is_leaf;
        this->write_node(right_empty_child);

        const auto insertion_place = parent_node.insert(sha1);
        const auto ptr_place_on_right_of_inserted_key = insertion_place + 1u;
        parent_node.pointers[ptr_place_on_right_of_inserted_key] = right_empty_child_pointer;
        this->write_node(parent_node);

        return right_empty_child;
      }
    }
  }

  btree_node create_children_till_leaf(btree_node& parent, const sha1_t& sha1, unsigned level)
  {
    const auto is_on_leaf_level = (level == 0u);

    btree_node node{ this->t(), parent.this_pointer };
    node.this_pointer = new_node_pointer();
    node.keys_count = 0u;
    node.is_leaf = is_on_leaf_level;
    this->write_node(node);

    parent.pointers[parent.keys_count] = node.this_pointer;
    this->write_node(parent);

    if (is_on_leaf_level) {
      return node;
    }

    return create_children_till_leaf(node, sha1, level - 1u);
  }

private:
  btree_node::pointer_t m_next_node_ptr{ 0u };
  btree_node m_current_node;
  unsigned m_current_tree_height{ 1u };
};
}
