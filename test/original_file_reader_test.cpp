#include "original_file_reader.hpp"

#include "memory_storage.hpp"

#include <gmock/gmock.h>

namespace okon::test {
using ::testing::Eq;

constexpr std::string_view k_zero_hash{ "0000000000000000000000000000000000000000" };
constexpr std::string_view k_one_hash{ "1111111111111111111111111111111111111111" };
constexpr auto k_number_of_read_buffers{ 4u };

auto to_storage(std::string_view content)
{
  memory_storage storage{};
  storage.m_storage.resize(content.size());
  std::copy(std::cbegin(content), std::cend(content), std::begin(storage.m_storage));
  return storage;
}

auto make_original_file_reader(memory_storage& storage, unsigned buffer_size)
{
  return original_file_reader<memory_storage>{ storage, buffer_size + k_text_sha1_length_for_simd,
                                               buffer_size, k_number_of_read_buffers };
}

TEST(OriginalFileReader, NextSha1_Empty_ReturnsNullopt)
{
  memory_storage storage{};
  auto reader = make_original_file_reader(storage, /*buffer_size=*/1024u);

  const auto result = reader.next_sha1();
  EXPECT_THAT(result, Eq(std::nullopt));
}

TEST(OriginalFileReader, NextSha1_ContainsExactlyOneHash_ReturnsHashAndThenNullopt)
{
  auto storage = to_storage(k_zero_hash);
  auto reader = make_original_file_reader(storage, /*buffer_size=*/1024u);

  const auto result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_zero_hash));

  const auto next_result = reader.next_sha1();
  EXPECT_THAT(next_result, Eq(std::nullopt));
}

TEST(OriginalFileReader, NextSha1_ContainsOneHashPerLine_ReturnsHashAndThenNullopt)
{
  auto storage = to_storage(std::string{ k_zero_hash } + '\n' + std::string{ k_one_hash });
  auto reader = make_original_file_reader(storage, /*buffer_size=*/1024u);

  const auto result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_zero_hash));

  const auto next_result = reader.next_sha1();
  EXPECT_THAT(next_result, Eq(k_one_hash));

  const auto next_next_result = reader.next_sha1();
  EXPECT_THAT(next_next_result, Eq(std::nullopt));
}

TEST(OriginalFileReader, NextSha1_ContainsOneHashWithCounter_ReturnsHashAndThenNullopt)
{
  auto storage = to_storage(std::string{ k_zero_hash } + ":1234");
  auto reader = make_original_file_reader(storage, /*buffer_size=*/1024u);

  const auto result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_zero_hash));

  const auto next_result = reader.next_sha1();
  EXPECT_THAT(next_result, Eq(std::nullopt));
}

TEST(OriginalFileReader, NextSha1_ContainsOneHashBufferOfHashSize_ReturnsHashAndThenNullopt)
{
  auto storage = to_storage(std::string{ k_zero_hash });
  auto reader = make_original_file_reader(storage, /*buffer_size=*/40u);

  const auto result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_zero_hash));

  const auto next_result = reader.next_sha1();
  EXPECT_THAT(next_result, Eq(std::nullopt));
}

TEST(OriginalFileReader, NextSha1_ContainsHashAfterHashBufferOfHashSize_ReturnsHashesAndThenNullopt)
{
  auto storage = to_storage(std::string{ k_zero_hash } + '\n' + std::string{ k_one_hash });
  auto reader = make_original_file_reader(storage, /*buffer_size=*/40u);

  const auto result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_zero_hash));
  const auto next_result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_one_hash));

  const auto next_next_result = reader.next_sha1();
  EXPECT_THAT(next_next_result, Eq(std::nullopt));
}

TEST(OriginalFileReader, NextSha1_BufferEndsAtTheBeginingOfNextLine_ReturnsHashesAndThenNullopt)
{
  auto storage = to_storage(std::string{ k_zero_hash } + '\n' + std::string{ k_one_hash });
  auto reader = make_original_file_reader(storage, 41u);

  const auto result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_zero_hash));
  const auto next_result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_one_hash));

  const auto next_next_result = reader.next_sha1();
  EXPECT_THAT(next_next_result, Eq(std::nullopt));
}

TEST(OriginalFileReader, NextSha1_BufferEndsInTheMiddleOfCounter_ReturnsHashesAndThenNullopt)
{
  auto storage = to_storage(std::string{ k_zero_hash } + ":1234\n" + std::string{ k_one_hash });
  auto reader = make_original_file_reader(storage, /*buffer_size=*/42u);

  const auto result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_zero_hash));
  const auto next_result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_one_hash));

  const auto next_next_result = reader.next_sha1();
  EXPECT_THAT(next_next_result, Eq(std::nullopt));
}

TEST(OriginalFileReader, NextSha1_BufferEndsInTheMiddleOfHashCounter_ReturnsHashesAndThenNullopt)
{
  auto storage = to_storage(std::string{ k_zero_hash } + ":1234\n" + std::string{ k_one_hash });
  auto reader = make_original_file_reader(storage, /*buffer_size=*/50u);

  const auto result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_zero_hash));
  const auto next_result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_one_hash));

  const auto next_next_result = reader.next_sha1();
  EXPECT_THAT(next_next_result, Eq(std::nullopt));
}

TEST(OriginalFileReader, NextSha1_BufferEndsExactlyAtTheEndOfInput_ReturnsHashesAndThenNullopt)
{
  auto storage = to_storage(std::string{ k_zero_hash } + ":1234\n" + std::string{ k_one_hash });
  auto reader = make_original_file_reader(storage, /*buffer_size=*/86u);

  const auto result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_zero_hash));

  const auto next_result = reader.next_sha1();
  EXPECT_THAT(next_result, Eq(k_one_hash));

  const auto next_next_result = reader.next_sha1();
  EXPECT_THAT(next_next_result, Eq(std::nullopt));
}

TEST(OriginalFileReader, NextSha1_BufferBiggerThanInput_ReturnsHashesAndThenNullopt)
{
  auto storage = to_storage(std::string{ k_zero_hash } + ":1234\n" + std::string{ k_one_hash });
  auto reader = make_original_file_reader(storage, /*buffer_size=*/128u);

  const auto result = reader.next_sha1();
  EXPECT_THAT(result, Eq(k_zero_hash));

  const auto next_result = reader.next_sha1();
  EXPECT_THAT(next_result, Eq(k_one_hash));

  const auto next_next_result = reader.next_sha1();
  EXPECT_THAT(next_next_result, Eq(std::nullopt));
}
}
