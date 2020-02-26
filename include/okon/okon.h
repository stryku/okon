#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int okon_prepare(const char* input_db_file_path, const char* output_processed_file_path);
int okon_exists_text(const char* sha1, const char* processed_file_path);
int okon_exists_binary(const void* sha1, const char* processed_file_path);

#ifdef __cplusplus
}
#endif
