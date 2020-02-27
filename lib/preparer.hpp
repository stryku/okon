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
  explicit preparer(std::string_view input_file_path, std::string_view working_directory_path,
                    std::string_view output_file_path);

  void prepare();

private:
  std::optional<std::string_view> get_next_sha1();
  void add_sha1_to_file(std::string_view sha1);

  sha1_tail_t sha1_tail(std::string_view sha1) const;

  void sort_files();
  void process_sorted_files();

  void write_sha1_buffer(unsigned buffer_index);

private:
  std::ifstream m_input_file;
  splitted_files m_output_files;
  fstream_wrapper m_file_wrapper;
  btree_sorted_keys_inserter<fstream_wrapper> m_btree;

  std::vector<std::vector<sha1_t>> m_sha1_buffers;
};
}
