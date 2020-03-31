#pragma once

#include "buffers_queue.hpp"
#include "sha1_utils.hpp"

#include <array>
#include <fstream>
#include <optional>
#include <string_view>
#include <thread>
#include <vector>

namespace okon {

template <typename DataStorage>
class original_file_reader
{
public:
  explicit original_file_reader(DataStorage& storage, unsigned buffer_size,
                                unsigned size_to_read_from_storage, unsigned number_of_buffers)
    : m_storage{ storage }
    , m_buffers{ buffer_size, number_of_buffers }
    , m_size_to_read_from_storage{ size_to_read_from_storage }
  {
    start_reader_thread();
  }

  std::optional<std::string_view> next_sha1();

  bool is_open() const;

private:
  std::optional<std::string_view> read_split_sha1();
  void read_chunk();

  void advance_view(unsigned n);
  void advance_till_next_sha1();

  void start_reader_thread();

private:
  DataStorage& m_storage;
  buffers_queue m_buffers;
  unsigned m_size_to_read_from_storage;
  std::vector<uint8_t>* m_buffer{ nullptr };
  std::string_view m_buffer_view;
  std::array<char, k_text_sha1_length_for_simd> m_backup_buffer{};
  bool m_need_to_read_and_advance_till_next_sha1{ false };
  bool m_has_more_input{ true };
};

template <typename DataStorage>
std::optional<std::string_view> original_file_reader<DataStorage>::next_sha1()
{
  if (m_need_to_read_and_advance_till_next_sha1) {
    read_chunk();
    if (!m_has_more_input) {
      return std::nullopt;
    }
    advance_till_next_sha1();
    m_need_to_read_and_advance_till_next_sha1 = false;
  }

  if (m_buffer_view.size() < k_text_sha1_length) {
    return read_split_sha1();
  }

  const auto sha1_view = std::string_view{ m_buffer_view.data(), k_text_sha1_length };

  advance_till_next_sha1();
  return sha1_view;
}

template <typename DataStorage>
bool original_file_reader<DataStorage>::is_open() const
{
  return m_storage.is_open();
}

template <typename DataStorage>
std::optional<std::string_view> original_file_reader<DataStorage>::read_split_sha1()
{
  const auto first_part_size = m_buffer_view.size();
  std::memcpy(&m_backup_buffer[0], m_buffer_view.data(), first_part_size);

  read_chunk();
  if (!m_has_more_input) {
    return std::nullopt;
  }

  const auto second_part_size = k_text_sha1_length - first_part_size;
  std::memcpy(std::next(&m_backup_buffer[0], first_part_size), m_buffer_view.data(),
              second_part_size);

  advance_view(second_part_size);
  advance_till_next_sha1();

  return std::string_view{ m_backup_buffer.data(), k_text_sha1_length };
}

template <typename DataStorage>
void original_file_reader<DataStorage>::read_chunk()
{
  const auto buffer_index = m_buffers.take_for_processing();
  if (!buffer_index) {
    m_buffer_view = std::string_view{};
    m_has_more_input = false;
    return;
  }

  if (m_buffer != nullptr) {
    m_buffers.processing_ready();
  }

  m_buffer = &m_buffers.access_buffer(*buffer_index);

  m_buffer_view = std::string_view{ reinterpret_cast<const char*>(m_buffer->data()),
                                    m_size_to_read_from_storage };
}

template <typename DataStorage>
void original_file_reader<DataStorage>::advance_view(unsigned n)
{
  m_buffer_view = std::string_view{ std::next(m_buffer_view.data(), n), m_buffer_view.size() - n };
}

template <typename DataStorage>
void original_file_reader<DataStorage>::advance_till_next_sha1()
{
  const auto new_line_pos = m_buffer_view.find('\n');
  if (new_line_pos == std::string_view::npos) {
    m_need_to_read_and_advance_till_next_sha1 = true;
    return;
  }

  advance_view(new_line_pos + 1u);
}
template <typename DataStorage>
void original_file_reader<DataStorage>::start_reader_thread()
{
  const auto fun = [this] {
    while (true) {
      const auto buffer_index = m_buffers.take_for_data_storing();
      auto& buffer = m_buffers.access_buffer(buffer_index);

      const auto read_size = m_storage.read(&buffer[0], m_size_to_read_from_storage);
      if (read_size == 0) {
        m_buffers.notify_no_more_data();
        return;
      }

      if (read_size < buffer.size()) {
        buffer.resize(read_size);
      }

      m_buffers.data_storing_ready();
    }
  };

  std::thread{ fun }.detach();
}
}
