#include <okon/okon.h>

#include "btree.hpp"
#include "fstream_wrapper.hpp"
#include "preparer.hpp"

okon_prepare_result okon_prepare(const char* input_db_file_path, const char* working_directory,
                                 const char* output_processed_file_path,
                                 okon_prepare_progress_callback_t user_progress_callback,
                                 void* progress_callback_user_data)
{
  std::ofstream{ output_processed_file_path };

  const auto progress_callback =
    [user_progress_callback, progress_callback_user_data]() -> okon::preparer::progress_callback_t {
    if (!user_progress_callback) {
      return [](int) {};
    }

    return [user_progress_callback, progress_callback_user_data](int progress) {
      user_progress_callback(progress_callback_user_data, progress);
    };
  }();

  okon::preparer preparer{ input_db_file_path, working_directory, output_processed_file_path,
                           progress_callback };
  const auto result = preparer.prepare();

  switch (result) {
    case okon::preparer::result ::success:
      return okon_prepare_result ::okon_prepare_result_success;
    case okon::preparer::result ::could_not_open_input_file:
      return okon_prepare_result ::okon_prepare_result_could_not_open_input_file;
    case okon::preparer::result ::could_not_open_intermediate_files:
      return okon_prepare_result ::okon_prepare_result_could_not_open_intermediate_files;
    case okon::preparer::result ::could_not_open_output:
      return okon_prepare_result ::okon_prepare_result_could_not_open_output;
  }
}

okon_exists_result okon_exists_text(const char* sha1, const char* processed_file_path)
{
  const auto sha1_bin = okon::simd_string_sha1_to_binary(sha1);
  return okon_exists_binary(sha1_bin.data(), processed_file_path);
}

okon_exists_result okon_exists_binary(const void* sha1, const char* processed_file_path)
{
  okon::fstream_wrapper file{ processed_file_path };

  if (!file.is_open()) {
    return okon_exists_result::okon_prepare_result_could_not_open_file;
  }

  okon::btree tree{ file };

  okon::sha1_t sha1_bin;
  std::memcpy(&sha1_bin[0], sha1, 20u);

  return tree.contains(sha1_bin) ? okon_exists_result::okon_exists_result_exists
                                 : okon_exists_result::okon_exists_result_doesnt_exist;
}
