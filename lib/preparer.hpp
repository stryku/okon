#pragma once

#include "btree_sorted_keys_inserter.hpp"
#include "fstream_wrapper.hpp"
#include "sha1_utils.hpp"
#include "splitted_files.hpp"

#include <string_view>

#include <array>
#include <fstream>
#include <optional>

namespace okon {
class preparer
{
public:
  enum class result
  {
    success,
    could_not_open_input_file,
    could_not_open_intermediate_files,
    could_not_open_output
  };

  explicit preparer(std::string_view input_file_path, std::string_view working_directory_path,
                    std::string_view output_file_path);

  result prepare();

private:
  std::optional<std::string_view> get_next_sha1();
  void add_sha1_to_file(std::string_view sha1);

  void sort_files();
  void process_sorted_files();

  void write_sha1_buffer(unsigned buffer_index);

private:
  std::ifstream m_input_file;
  splitted_files m_intermediate_files;
  fstream_wrapper m_output_file_wrapper;
  btree_sorted_keys_inserter<fstream_wrapper> m_btree;
  std::vector<std::vector<sha1_t>> m_sha1_buffers;
};
}
