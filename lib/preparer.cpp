#include "preparer.hpp"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

namespace {
constexpr auto k_sha1_buffer_max_size{ 1024u * 100u };
constexpr auto k_file_chunk_size_to_read{ 1024u * 1024u };
constexpr auto k_sorting_threads{ 3u };
}

namespace okon {
preparer::preparer(std::string_view input_file_path, std::string_view working_directory_path,
                   std::string_view output_file_path, progress_callback_t progress_callback)
  : m_input_file_wrapper{ input_file_path }
  , m_input_reader{ m_input_file_wrapper,
                    /*buffer_size=*/k_file_chunk_size_to_read + k_text_sha1_length_for_simd,
                    /*size_to_read_from_storage=*/k_file_chunk_size_to_read,
                    /*number_of_buffers=*/4u }
  , m_intermediate_files{ working_directory_path, std::ios::in | std::ios::out | std::ios::trunc }
  , m_output_file_wrapper{ output_file_path }
  , m_btree{ m_output_file_wrapper, /*order=*/1024u }
  , m_sha1_buffers{ 256u }
  , m_sorted_files_ready_state{}
  , m_progress_callback{ std::move(progress_callback) }
{
  m_sorted_files_ready_state.fill(false);

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

  report_progress(k_progress_unknown);

  while (const auto sha1 = m_input_reader.next_sha1()) {
    add_sha1_to_file(*sha1);
    ++m_total_sha1_count;
  }

  for (auto i = 0u; i < m_sha1_buffers.size(); ++i) {
    write_sha1_buffer(i);
  }

  start_writing_sorted_files_thread();
  sort_files();
  m_writing_sorted_files_thread.join();

  report_progress(100);

  return result::success;
}

void preparer::add_sha1_to_file(std::string_view sha1)
{
  const auto index = two_first_chars_to_byte(sha1.data());
  m_sha1_buffers[index].emplace_back(text_sha1_to_binary(sha1.data()));

  if (m_sha1_buffers[index].size() >= k_sha1_buffer_max_size) {
    write_sha1_buffer(index);
    m_sha1_buffers[index].clear();
  }
}

void preparer::sort_files()
{
  const auto sort_pred = [](const sha1_t& lhs, const sha1_t& rhs) {
    return std::memcmp(lhs.data(), rhs.data(), sizeof(sha1_t)) < 0;
  };

  auto sorter = [sort_pred, this](unsigned start_index) {
    std::vector<sha1_t> sha1s;

    for (auto i = start_index; i < k_intermediate_files_count; i += k_sorting_threads) {
      auto& file = *(std::next(m_intermediate_files.begin(), i));
      const std::streamsize file_size = file.tellp();
      const auto sha1_count = file_size / sizeof(sha1_t);
      sha1s.resize(sha1_count);
      file.seekg(0);
      file.read(reinterpret_cast<char*>(&sha1s[0]), file_size);

      std::sort(std::begin(sha1s), std::end(sha1s), sort_pred);

      file.seekp(0);
      file.write(reinterpret_cast<char*>(&sha1s[0]), file_size);

      std::lock_guard lock{ m_processing_sorted_files_mtx };
      m_sorted_files_ready_state[i] = true;
      m_sorted_files_cvs[i].notify_one();
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

void preparer::start_writing_sorted_files_thread()
{
  m_writing_sorted_files_thread = std::thread{ [this] {
    std::vector<sha1_t> sha1s;

    for (auto i = 0u; i < k_intermediate_files_count; ++i) {
      {
        std::unique_lock lock{ m_processing_sorted_files_mtx };
        m_sorted_files_cvs[i].wait(lock, [i, this] { return m_sorted_files_ready_state[i]; });
      }

      auto& file = m_intermediate_files[i];

      file.seekp(0, std::ios::end);
      const std::streamsize file_size = file.tellp();
      const auto sha1_count = file_size / sizeof(sha1_t);
      sha1s.resize(sha1_count);
      file.seekg(0);
      file.read(reinterpret_cast<char*>(&sha1s[0]), file_size);

      for (const auto& sha1 : sha1s) {
        m_btree.insert_sorted(sha1);

        ++m_sha1_written_to_tree_count;
        const auto progress = 100 * m_sha1_written_to_tree_count / m_total_sha1_count;
        report_progress(progress);
      }
    }

    m_btree.finalize_inserting();
  } };
}

void preparer::write_sha1_buffer(unsigned buffer_index)
{
  auto& file = m_intermediate_files[buffer_index];
  file.write(reinterpret_cast<const char*>(m_sha1_buffers[buffer_index][0].data()),
             sizeof(sha1_t) * m_sha1_buffers[buffer_index].size());
}

void preparer::report_progress(int progress)
{
  if (m_last_reported_progress == progress) {
    return;
  }

  m_progress_callback(progress);
  m_last_reported_progress = progress;
}
}
