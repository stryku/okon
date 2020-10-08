#include "sha1_utils.hpp"

#include <gmock/gmock.h>

#include <string_view>

namespace okon::test {

using namespace std::literals;
using namespace testing;

struct TextSha1ToBinaryState
{
  std::string_view text;
  sha1_t expected;
};

TextSha1ToBinaryState values[] = {
  { "0000000000000000000000000000000000000000 --------------------"sv,
    sha1_t{ 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u } },
  { "0123456789ABCDEFFEDCBA987654321001234567 --------------------"sv,
    sha1_t{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC,
            0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67 } },
  { "0123456789abcdeffedcba987654321001234567 --------------------"sv,
    sha1_t{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC,
            0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67 } }
};

using StringSha1ToBinaryTest = testing::TestWithParam<TextSha1ToBinaryState>;

TEST_P(StringSha1ToBinaryTest, ProducesCorrectBinary)
{
  const auto& [text_sha1, expected] = GetParam();
  const auto result = details::string_sha1_to_binary(text_sha1.data());
  EXPECT_THAT(result, Eq(expected));
}

INSTANTIATE_TEST_SUITE_P(TextSha1ToBinary, StringSha1ToBinaryTest, testing::ValuesIn(values));

using SIMDSha1ToBinaryTest = testing::TestWithParam<TextSha1ToBinaryState>;

TEST_P(SIMDSha1ToBinaryTest, ProducesCorrectBinary)
{
  const auto& [text_sha1, expected] = GetParam();
  const auto result = details::simd_string_sha1_to_binary(text_sha1.data());
  EXPECT_THAT(result, Eq(expected));
}

INSTANTIATE_TEST_SUITE_P(TextSha1ToBinary, SIMDSha1ToBinaryTest, testing::ValuesIn(values));
}
