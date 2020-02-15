#pragma once

namespace pwned {
class trie_utils
{
public:
  constexpr uint8_t char_to_index(char c) const
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
};
}
