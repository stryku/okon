#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

namespace okon::test {
class memory_storage
{
public:
  using storage_size_t = unsigned;

  void write(const void* ptr, storage_size_t size)
  {
    if (size + m_out_pos > m_storage.size()) {
      m_storage.resize(size + m_out_pos);
    }

    std::memcpy(std::next(&m_storage[0], m_out_pos), ptr, size);
    m_out_pos += size;
  }

  void read(void* ptr, storage_size_t size)
  {
    std::memcpy(ptr, std::next(&m_storage[0], m_in_pos), size);
    m_in_pos += size;
  }

  void seek_in(storage_size_t pos) { m_in_pos = pos; }
  void seek_out(storage_size_t pos) { m_out_pos = pos; }

  storage_size_t tell_in() { return m_in_pos; }
  storage_size_t tell_out() { return m_out_pos; }

  storage_size_t total_size() const { return m_storage.size(); }

public:
  std::vector<uint8_t> m_storage;
  storage_size_t m_in_pos;
  storage_size_t m_out_pos;
};
}