#include "preparer.hpp"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

namespace {
constexpr auto k_sha1_buffer_max_size{ 1024u * 100u };
constexpr auto k_sorting_threads{ 4u };
constexpr auto k_intermediate_files_count{ 256u };
constexpr auto k_files_to_sort_per_thread{ k_intermediate_files_count / k_sorting_threads };
}

namespace okon {
preparer::preparer(std::string_view input_file_path, std::string_view working_directory_path,
                   std::string_view output_file_path)
  : m_input_file_wrapper{ input_file_path }
  , m_input_reader{ m_input_file_wrapper, /*buffer_size=*/1024u * 1024u, /*number_of_buffers=*/4u }
  , m_intermediate_files{ working_directory_path, std::ios::in | std::ios::out | std::ios::trunc }
  , m_output_file_wrapper{ output_file_path }
  , m_btree{ m_output_file_wrapper, /*order=*/1024u }
  , m_sha1_buffers{ 256u }
{
  for (auto& buffer : m_sha1_buffers) {
    buffer.reserve(k_sha1_buffer_max_size);
  }
}

preparer::result preparer::prepare()
{
  if (!m_input_reader.is_open()) {
    return result::could_not_open_input_file;
  }

  if (!m_intermediate_files.are_all_open()) {
    return result::could_not_open_intermediate_files;
  }

  if (!m_output_file_wrapper.is_open()) {
    return result::could_not_open_output;
  }

  while (const auto sha1 = m_input_reader.next_sha1()) {
    add_sha1_to_file(*sha1);
  }

  for (auto i = 0u; i < m_sha1_buffers.size(); ++i) {
    write_sha1_buffer(i);
  }

  sort_files();
  process_sorted_files();

  return result::success;
}

void preparer::add_sha1_to_file(std::string_view sha1)
{
  const auto index = two_first_chars_to_byte(sha1.data());
  m_sha1_buffers[index].emplace_back(string_sha1_to_binary(sha1));

  if (m_sha1_buffers[index].size() >= k_sha1_buffer_max_size) {
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

    for (auto i = start_index; i < k_intermediate_files_count; i += k_sorting_threads) {
      auto& file = *(std::next(m_intermediate_files.begin(), i));
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
  threads.reserve(k_sorting_threads);

  for (auto i = 0u; i < k_sorting_threads; ++i) {
    threads.emplace_back(sorter, i);
  }

  for (auto& t : threads) {
    t.join();
  }
}

void preparer::process_sorted_files()
{
  std::vector<sha1_t> sha1s;

  for (auto& file : m_intermediate_files) {
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
  auto& file = m_intermediate_files[buffer_index];
  file.write(reinterpret_cast<const char*>(m_sha1_buffers[buffer_index][0].data()),
             sizeof(sha1_t) * m_sha1_buffers[buffer_index].size());
}
}
