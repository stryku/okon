#include "trie_file_generator.hpp"

#include "trie_manipulator.hpp"

#include <iostream>

#include <array>

namespace {
constexpr auto k_sha1_length{ 40u };
constexpr auto k_max_digit_length{ 15u };
constexpr auto k_max_line_length{ k_sha1_length + k_max_digit_length + 2u }; // 2u for : and \0

std::array<char, k_max_line_length> line_buffer{};
}

namespace pwned {
trie_file_generator::trie_file_generator(std::string_view input_file_path,
                                         std::string_view output_file_path)
  : m_input_file_path{ input_file_path }
  , m_output_file_path{ output_file_path }
  , m_input_file{ m_input_file_path.data() }
  , m_output_file{ m_output_file_path.data() }
{
  if (!m_output_file.is_open()) {
    std::ofstream{ m_output_file_path.data() };
    m_output_file.open(m_output_file_path.data());
  }
}

trie_file_generator::generation_result trie_file_generator::generate()
{
  if (!m_input_file.is_open()) {
    return generation_result::input_file_not_found;
  }

  if (!m_output_file.is_open()) {
    return generation_result::cant_open_output_file;
  }

  trie_manipulator{ m_output_file, m_nodes_count }.write_initial_node();

  uint64_t counter{ 0u };

  while (const auto sha1 = get_next_sha1()) {
    add_sha1_to_trie(*sha1);
    if (++counter % 100000 == 0) {
      std::cout << (float)counter / 555278657.f << '\n';
    }
  }

  return generation_result::success;
}

std::optional<std::string_view> trie_file_generator::get_next_sha1()
{
  if (!m_input_file.getline(&line_buffer[0], k_max_line_length)) {
    return std::nullopt;
  }

  return std::string_view{ &line_buffer[0], k_sha1_length };
}

void trie_file_generator::add_sha1_to_trie(std::string_view sha1)
{
  trie_manipulator manipulator{ m_output_file, m_nodes_count };
  manipulator.go_to_start_node();

  for (const auto c : sha1) {
    if (!manipulator.go_to_next_node(c)) {
      manipulator.write_next_node(c);
      manipulator.go_to_next_node(c);
    }
  }
}

constexpr uint8_t trie_file_generator::char_to_index(char c) const
{
  switch (c) {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
      c -= 'A';
      [[fallthrough]];
    default:
      c -= '0';
  }

  return static_cast<uint8_t>(c);
}

uint64_t trie_file_generator::generate_next_node_index()
{
  return std::exchange(m_nodes_count, m_nodes_count + 1u);
}
}
