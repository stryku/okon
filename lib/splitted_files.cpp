#include "splitted_files.hpp"

#include <algorithm>

namespace okon {

splitted_files::splitted_files(std::string_view path, std::ios::openmode mode)
{
  auto name = std::string{ "00" };

  for (auto& output_file : m_files) {
    output_file.open(path.data() + name, mode);
    increment_name(name);
  }
}

std::fstream& splitted_files::sha1_file(std::string_view sha1)
{
  const auto file_index = two_first_chars_to_byte(sha1.data());
  return m_files[file_index];
}

std::fstream& splitted_files::operator[](unsigned index)
{
  return m_files[index];
}

void splitted_files::increment_name(std::string& current_name) const
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

bool splitted_files::are_all_open() const
{
  return std::all_of(std::cbegin(m_files), std::cend(m_files),
                     [](const auto& file) { return file.is_open(); });
}
}
