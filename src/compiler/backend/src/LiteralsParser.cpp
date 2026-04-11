#include "vanadium/compiler/LiteralsParser.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

#include "vanadium/compiler/RuntimeBindings.h"

namespace vanadium::compiler::literals {

std::variant<RuntimeBindings::NativeIntType, std::string_view> ParseInt(std::string_view s) {
  RuntimeBindings::NativeIntType result;
  std::from_chars(s.data(), s.data() + s.size(), result);

  // TODO: return s on out of range
  return result;
}

double ParseFloat(std::string_view s) {
  double result;
  std::from_chars(s.data(), s.data() + s.size(), result);
  return result;
}

std::string_view ParseCharstring(std::string_view s) {
  s.remove_prefix(1);  // "
  s.remove_suffix(1);  // "
  return s;
}

std::string ParseOctetstring(std::string_view s) {
  s.remove_prefix(1);  // '
  s.remove_suffix(2);  // 'O

  // TODO: it will be needed in runtime, extract, cover with tests
  constexpr auto kChar2Hex = [](char c) {
    if (c >= '0' && c <= '9') {
      return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
      return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
      return c - 'a' + 10;
    }
    assert(false);
  };

  // TODO(lexer): verify length
  assert(s.length() % 2 == 0);

  std::string result;
  result.reserve(s.length() / 2);
  //
  for (std::size_t i = 0; i < s.length(); i += 2) {  // TODO: ignore whitespace
    const std::uint8_t high = kChar2Hex(s[i]);
    const std::uint8_t low = kChar2Hex(s[i + 1]);
    result.push_back((high << 4) | low);
  }

  return result;
}

std::pair<std::string, std::uint32_t> ParseBitstring(std::string_view s) {
  s.remove_prefix(1);  // '
  s.remove_suffix(2);  // 'B

  const auto max_bits = s.size();
  const auto max_bytes = (max_bits + 7) / 8;

  std::string result(max_bytes, 0);

  std::uint32_t bit_count = 0;
  std::size_t byte_index = 0;
  int bit_pos = 7;

  for (char c : s) {
    assert(c == '0' || c == '1');  // TODO: ignore whitespace

    if (c == '1') {
      result[byte_index] |= (1 << bit_pos);
    }

    ++bit_count;

    if (--bit_pos < 0) {
      bit_pos = 7;
      ++byte_index;
    }
  }

  const auto used_bytes = (bit_count + 7) / 8;
  result.resize(used_bytes);

  return {std::move(result), bit_count};
}

}  // namespace vanadium::compiler::literals
