#pragma once

#include "sha1_utils.hpp"

#include <array>
#include <fstream>
#include <string>
#include <string_view>

namespace okon {
class splitted_files
{
public:
  explicit splitted_files(std::string_view path,
                          std::ios::openmode mode = std::ios::in | std::ios::out)
  {
    auto name = std::string{ "00" };

    for (auto& output_file : m_files) {
      output_file.open(path.data() + name, mode);
      increment_name(name);
    }
  }

  std::fstream& sha1_file(std::string_view sha1)
  {
    const auto file_index = two_first_chars_to_byte(sha1.data());
    return m_files[file_index];
  }

  std::fstream& operator[](unsigned index)
  {
    return m_files[index];
  }

  auto begin()
  {
    return std::begin(m_files);
  }

  auto end()
  {
    return std::end(m_files);
  }

private:
  void increment_name(std::string& current_name) const
  {
    if (current_name[1] == '9') {
      current_name[1] = 'A';
    } else if (current_name[1] == 'F') {
      current_name[1] = '0';

      if (current_name[0] == '9') {
        current_name[0] = 'A';
      } else {
        ++current_name[0];
      }
    } else {
      ++current_name[1];
    }
  }

private:
  std::array<std::fstream, 256u> m_files;
};
}
