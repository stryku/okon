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
                          std::ios::openmode mode = std::ios::in | std::ios::out);

  std::fstream& sha1_file(std::string_view sha1);

  std::fstream& operator[](unsigned index);

  auto begin()
  {
    return std::begin(m_files);
  }

  auto end()
  {
    return std::end(m_files);
  }

private:
  void increment_name(std::string& current_name) const;

private:
  std::array<std::fstream, 256u> m_files;
};
}
