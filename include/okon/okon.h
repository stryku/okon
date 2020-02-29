#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum okon_prepare_result
{
  okon_prepare_result_success,                           //!< Everything went well.
  okon_prepare_result_could_not_open_input_file,         //!< Issue while opening input file.
  okon_prepare_result_could_not_open_intermediate_files, //!< Issue while creating intermediate
                                                         //!< files.
  okon_prepare_result_could_not_open_output              //!< Issue while creating output file.
};

/** Prepares file based on input database.
 * Truncates 00-FF files in @param working_directory.
 * Truncates @param output_processed_file_path file.
 * The function does not delete intermediate files. User needs to do it on their own.
 *
 * @param input_db_file_path Path to text file with hashes:count.
 * @param working_directory Directory where intermediate files are going to be created.
 * @param output_processed_file_path Path to file where output data should be written to.
 */
okon_prepare_result okon_prepare(const char* input_db_file_path, const char* working_directory,
                                 const char* output_processed_file_path);

enum okon_exists_result
{
  okon_exists_result_doesnt_exist,         //!< Hash was not found.
  okon_exists_result_exists,               //!< Hash was found.
  okon_prepare_result_could_not_open_file, //!< Issue while opening processed file.
};

/** Checks whether given hash exists in @param processed_file_path.
 *
 * @param sha1 Text based hash. The behavior is undefined if (sha1 + 39) is not accessible.
 * @param prepared_file_path Path to a file prepared by okon_prepare() function.
 */
okon_exists_result okon_exists_text(const char* sha1, const char* prepared_file_path);

/** Checks whether given hash exists in @param processed_file_path.
 *
 * @param sha1 Binary based hash. The behavior is undefined if ((const uint8_t*)sha1 + 19) is not
 * accessible.
 * @param prepared_file_path Path to a file prepared by okon_prepare() function.
 */
okon_exists_result okon_exists_binary(const void* sha1, const char* processed_file_path);

#ifdef __cplusplus
}
#endif
