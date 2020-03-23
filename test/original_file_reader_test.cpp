#include "original_file_reader.hpp"

#include "memory_storage.hpp"

#include <gmock/gmock.h>

namespace okon::test {
using ::testing::Eq;

TEST(OriginalFileReader, NextSha1_Empty_ReturnsNullopt)
{
  memory_storage storage{};
  original_file_reader<memory_storage> reader{ storage, 1024u };
  const auto result = reader.next_sha1();

  EXPECT_THAT(result, Eq(std::nullopt));
}
}
