#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

namespace okon::test {
class memory_storage
{
public:
  using size_type_t = unsigned;

  void write(const void* ptr, size_type_t size)
  {
    if (size + m_out_pos > m_storage.size()) {
      m_storage.resize(size + m_out_pos);
    }

    std::memcpy(std::next(&m_storage[0], m_out_pos), ptr, size);
    m_out_pos += size;
  }

  size_type_t read(void* ptr, size_type_t size)
  {
    const auto size_to_read = std::min(static_cast<size_type_t>(m_storage.size() - m_in_pos), size);
    std::memcpy(ptr, std::next(&m_storage[0], m_in_pos), size_to_read);
    m_in_pos += size_to_read;
    return size_to_read;
  }

  void seek_in(size_type_t pos)
  {
    m_in_pos = pos;
  }
  void seek_out(size_type_t pos)
  {
    m_out_pos = pos;
  }

  size_type_t tell_in()
  {
    return m_in_pos;
  }
  size_type_t tell_out()
  {
    return m_out_pos;
  }

  size_type_t total_size() const
  {
    return m_storage.size();
  }

public:
  std::vector<uint8_t> m_storage;
  size_type_t m_in_pos;
  size_type_t m_out_pos;
};
}