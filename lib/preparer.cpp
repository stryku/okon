#include "preparer.hpp"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#include <iostream>

namespace {
constexpr auto k_string_sha1_length{ 40u };
constexpr auto k_max_digit_length{ 15u };
constexpr auto k_max_line_length{ k_string_sha1_length + k_max_digit_length + 1u }; // 1u for ':'

std::array<char, k_max_line_length> line_buffer{};
}

namespace okon {
preparer::preparer(std::string_view input_file_path, std::string_view working_directory_path,
                   std::string_view output_file_path)
  : m_input_file{ input_file_path.data() }
  , m_output_files{ working_directory_path, std::ios::in | std::ios::out | std::ios::trunc }
  , m_file_wrapper{ output_file_path }
  , m_btree{ m_file_wrapper, 2048u }
  , m_sha1_buffers{ 256u }
{
}

void preparer::prepare()
{
  while (const auto sha1 = get_next_sha1()) {
    add_sha1_to_file(*sha1);
  }

  for (auto i = 0u; i < m_sha1_buffers.size(); ++i) {
    write_sha1_buffer(i);
  }

  sort_files();
  process_sorted_files();
}

std::optional<std::string_view> preparer::get_next_sha1()
{
  if (!m_input_file.getline(&line_buffer[0], k_max_line_length)) {
    return std::nullopt;
  }

  return std::string_view{ &line_buffer[0], k_string_sha1_length };
}

void preparer::add_sha1_to_file(std::string_view sha1)
{
  const auto index = two_first_chars_to_byte(sha1.data());
  m_sha1_buffers[index].emplace_back(string_sha1_to_binary(sha1));

  if (m_sha1_buffers[index].size() > 1024 * 100) {
    write_sha1_buffer(index);
    m_sha1_buffers[index].clear();
  }
}

void preparer::sort_files()
{
  const auto pred = [](const sha1_t& lhs, const sha1_t& rhs) {
    return std::memcmp(lhs.data(), rhs.data(), sizeof(sha1_t)) < 0;
  };

  auto sorter = [pred, this](unsigned start_index) {
    std::vector<sha1_t> sha1s;

    for (auto i = start_index; i < start_index + 64; ++i) {
      auto& file = *(std::next(m_output_files.begin(), i));
      const std::streamsize file_size = file.tellp();
      const auto sha1_count = file_size / sizeof(sha1_t);
      sha1s.resize(sha1_count);
      file.seekg(0);
      file.read(reinterpret_cast<char*>(&sha1s[0]), file_size);

      std::sort(std::begin(sha1s), std::end(sha1s), pred);

      file.seekp(0);
      file.write(reinterpret_cast<char*>(&sha1s[0]), file_size);
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(4u);

  for (auto i = 0u; i < 4u; ++i) {
    threads.emplace_back(sorter, i * 64u);
  }

  for (auto& t : threads) {
    t.join();
  }
}

void preparer::process_sorted_files()
{
  std::vector<sha1_t> sha1s;

  for (auto& file : m_output_files) {
    file.seekp(0, std::ios::end);
    const std::streamsize file_size = file.tellp();
    const auto sha1_count = file_size / sizeof(sha1_t);
    sha1s.resize(sha1_count);
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&sha1s[0]), file_size);

    for (const auto& sha1 : sha1s) {
      m_btree.insert_sorted(sha1);
    }
  }

  m_btree.finalize_inserting();
}

void preparer::write_sha1_buffer(unsigned buffer_index)
{
  auto& file = m_output_files[buffer_index];
  file.write(reinterpret_cast<const char*>(m_sha1_buffers[buffer_index][0].data()),
             sizeof(sha1_t) * m_sha1_buffers[buffer_index].size());
}
}
