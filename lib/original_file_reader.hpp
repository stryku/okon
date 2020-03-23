#pragma once

#include "sha1_utils.hpp"

#include <array>
#include <fstream>
#include <optional>
#include <string_view>
#include <vector>

namespace okon {

template <typename DataStorage>
class original_file_reader
{
public:
  explicit original_file_reader(DataStorage& storage, unsigned buffer_size)
    : m_storage{ storage }
    , m_buffer(buffer_size)
  {
  }

  std::optional<std::string_view> next_sha1();

  bool is_open() const;

private:
  std::optional<std::string_view> read_split_sha1();
  void read_chunk();

  void advance_view(unsigned n);
  void advance_till_next_sha1();

private:
  DataStorage& m_storage;
  std::vector<char> m_buffer;
  std::string_view m_buffer_view;
  std::array<char, k_text_sha1_length> m_backup_buffer;
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

  return std::string_view{ m_backup_buffer.data(), m_backup_buffer.size() };
}

template <typename DataStorage>
void original_file_reader<DataStorage>::read_chunk()
{
  const auto read_size = m_storage.read(&m_buffer[0], m_buffer.size());
  if (read_size == 0) {
    m_buffer_view = std::string_view{};
    m_has_more_input = false;
    return;
  }

  if (read_size < m_buffer.size()) {
    m_buffer.resize(read_size);
  }

  m_buffer_view = std::string_view{ m_buffer.data(), m_buffer.size() };
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

  advance_view(new_line_pos);
}

}
