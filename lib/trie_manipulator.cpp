#include "trie_manipulator.hpp"

namespace pwned {

trie_manipulator::trie_manipulator(std::fstream& file, trie_node::index_t& node_count)
  : m_file{ file }
  , m_node_count{ node_count }
{
  //  go_to_node(0u);
}

std::optional<trie_node> trie_manipulator::node()
{

  m_file.seekg(m_current_node_index * trie_node::k_binary_size);
  if (m_file.rdbuf()->in_avail() < trie_node::k_binary_size) {
    return std::nullopt;
  }

  return read_node();
}

void trie_manipulator::go_to_node(uint64_t node_index)
{
  m_current_node_index = node_index;
  m_current_node = node_unsafe();
}

bool trie_manipulator::go_to_next_node(char c)
{
  if (!m_current_node) {
    return false;
  }

  const auto char_index = char_to_index(c);

  const auto next_node_index = m_current_node->chars[char_index];
  if (next_node_index == trie_node::k_unused_node_index) {
    return false;
  }

  go_to_node(next_node_index);
  return true;
}

trie_node trie_manipulator::node_unsafe()
{
  m_file.seekg(m_current_node_index * trie_node::k_binary_size);
  return read_node();
}

trie_node trie_manipulator::read_node()
{
  trie_node node;
  m_file.read(reinterpret_cast<char*>(&node.chars), trie_node::k_binary_size);
  return node;
}

void trie_manipulator::mark_as_end_node()
{
  m_current_node = trie_node::end_node();

  m_file.seekp(0, std::ostream::end);
  const auto next_node_index = m_file.tellp() / trie_node::k_binary_size;

  const auto next_node = trie_node::unused_node();
  m_file.write(reinterpret_cast<const char*>(&m_current_node->chars[0]), trie_node::k_binary_size);
}

void trie_manipulator::write_next_node(char c)
{
  const auto char_index = char_to_index(c);

  ++m_node_count;
  const auto next_node_index = m_node_count;

  m_current_node->chars[char_index] = next_node_index;
  write_node(m_current_node_index, *m_current_node);

  const auto next_node = trie_node::unused_node();
  write_node(next_node_index, next_node);
}

void trie_manipulator::write_initial_node()
{
  const auto node = trie_node::unused_node();
  write_node(0u, node);
  m_current_node = node;
}

void trie_manipulator::go_to_start_node()
{
  go_to_node(0u);
}

void trie_manipulator::write_node(trie_node::index_t node_index, const trie_node& node)
{
  const auto pos = node_index * trie_node::k_binary_size;
  m_file.seekp(pos);
  m_file.write(reinterpret_cast<const char*>(&node.chars[0]), trie_node::k_binary_size);
}
}
