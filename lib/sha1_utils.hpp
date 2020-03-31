#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>

#define VCL_NAMESPACE vcl
#include <vcl/vectorclass.h>

namespace okon {
using sha1_t = std::array<uint8_t, 20u>;
constexpr auto k_text_sha1_length{ 40u };
constexpr auto k_text_sha1_length_for_simd{ 64u };

inline constexpr uint8_t char_to_index(char c)
{
  switch (c) {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
      c -= 'A' - ':';
      [[fallthrough]];
    default:
      c -= '0';
  }

  return static_cast<uint8_t>(c);
}

inline constexpr uint8_t two_first_chars_to_byte(const char* cs)
{
  uint8_t byte = static_cast<uint8_t>(char_to_index(cs[0])) << 4;
  byte |= char_to_index(cs[1]);
  return byte;
}

inline sha1_t string_sha1_to_binary(std::string_view sha1_text)
{
  okon::sha1_t sha1;

  for (auto i = 0; i < 40; i += 2) {
    sha1[i / 2] = two_first_chars_to_byte(sha1_text.data() + i);
  }

  return sha1;
}

// The function assumes that ((const char*)text)[63] is accessible.
inline sha1_t simd_string_sha1_to_binary(const void* text)
{
  const auto is_little_endian = [] {
    int value{ 1u };
    return *(char*)&value == 1;
  }();
  assert(is_little_endian);

  vcl::Vec64uc v8;
  v8.load(text);

  v8 -= vcl::Vec64uc{ '0' };

  const auto gt_nine_mask = v8 > vcl::Vec64uc{ 9u };
  v8 = if_sub(gt_nine_mask, v8, vcl::Vec64uc{ 7u });

  const auto shifted_8_to_left = v8 << 4u;
  const auto shifted_16 = vcl::Vec32us{ shifted_8_to_left } << 8u;

  const auto v16 = vcl::Vec32us{ v8 };
  const auto result = (v16 | shifted_16 & vcl::Vec32us{ 0xff00u }) >> 8u;

  std::aligned_storage_t<sizeof(uint8_t) * 64u, 32u> result_storage;
  auto result_storage_ptr = reinterpret_cast<uint8_t*>(&result_storage);
  result.store_a(result_storage_ptr);

  sha1_t result_arr{};
  auto result_storage_16_ptr = reinterpret_cast<uint16_t*>(result_storage_ptr);
  for (auto i = 0; i < result_arr.size(); ++i) {
    result_arr[i] = result_storage_16_ptr[i];
  }

  return result_arr;
}

inline std::string binary_sha1_to_string(const sha1_t& sha1)
{
  std::string result;

  const auto to_char = [](uint8_t value) -> char {
    return value < 10 ? value + '0' : (value - 10 + 'A');
  };

  for (uint8_t byte : sha1) {
    const uint8_t first_char = (byte & 0xf0u) >> 4u;
    const uint8_t second_char = byte & 0x0fu;

    result += to_char(first_char);
    result += to_char(second_char);
  }

  return result;
}
}
