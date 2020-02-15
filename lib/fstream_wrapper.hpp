#pragma once

#include <fstream>
#include <string_view>

namespace pwned {
class fstream_wrapper
{
public:
  using pos_type_t = std::fstream::pos_type;

  explicit fstream_wrapper(std::string_view path,
                           std::ios::openmode mode = std::ios::in | std::ios::out)
    : m_file{ path.data(), mode }
  {
  }

  void write(const void* ptr, pos_type_t size)
  {
    m_file.write(reinterpret_cast<const char*>(ptr), size);
  }

  void read(void* ptr, pos_type_t size)
  {
    m_file.read(reinterpret_cast<char*>(ptr), size);
  }

  void seek_in(pos_type_t pos)
  {
    m_file.seekg(pos);
  }
  void seek_out(pos_type_t pos)
  {
    m_file.seekp(pos);
  }

  pos_type_t tell_in()
  {
    return m_file.tellg();
  }
  pos_type_t tell_out()
  {
    return m_file.tellp();
  }

private:
  std::fstream m_file;
};
}
