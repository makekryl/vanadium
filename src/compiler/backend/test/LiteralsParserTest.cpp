#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <string_view>

#include "vanadium/compiler/LiteralsParser.h"

using namespace vanadium;
using namespace vanadium::compiler;

// TODO: below is a modified copypaste from lib/testing/macro.h
//       ^ clang-tidy suggests internal linkage for inline fn for some reason (?!)
#define DEFINE_NAMED_TEST_PARAM_PRINTER(TPARAM, MEMBER)                 \
  friend std::ostream& operator<<(std::ostream& out, const TPARAM& p) { \
    out << p.MEMBER;                                                    \
    return out;                                                         \
  }

//

struct CharstringParam {
  std::string_view input;
  std::string_view expected;
  DEFINE_NAMED_TEST_PARAM_PRINTER(CharstringParam, input);
};

struct CharstringLiteralParserTest : public ::testing::TestWithParam<CharstringParam> {};

TEST_P(CharstringLiteralParserTest, Parse) {
  const auto& tc = GetParam();
  EXPECT_EQ(literals::ParseCharstring(tc.input), tc.expected);
}

INSTANTIATE_TEST_SUITE_P(Charstring, CharstringLiteralParserTest,
                         ::testing::Values(CharstringParam{"\"\"", ""},            //
                                           CharstringParam{"\"Test\"", "Test"},    //
                                           CharstringParam{"\"12345\"", "12345"},  //
                                           CharstringParam{"\"Multiple words\"", "Multiple words"}));

//

struct OctetstringParam {
  std::string_view input;
  std::vector<std::uint8_t> expected;
  DEFINE_NAMED_TEST_PARAM_PRINTER(OctetstringParam, input);
};

struct OctetstringLiteralParserTest : public ::testing::TestWithParam<OctetstringParam> {};

TEST_P(OctetstringLiteralParserTest, Parse) {
  const auto& tc = GetParam();
  EXPECT_EQ(literals::ParseOctetstring(tc.input), tc.expected);
}

INSTANTIATE_TEST_SUITE_P(
    Octetstring, OctetstringLiteralParserTest,
    ::testing::Values(OctetstringParam{"''O", {}},                                                    //
                      OctetstringParam{"'00'O", {0}},                                                 //
                      OctetstringParam{"'01'O", {1}},                                                 //
                      OctetstringParam{"'0A'O", {10}},                                                //
                      OctetstringParam{"'0F'O", {15}},                                                //
                      OctetstringParam{"'10'O", {16}},                                                //
                      OctetstringParam{"'7F'O", {127}},                                               //
                      OctetstringParam{"'80'O", {128}},                                               //
                      OctetstringParam{"'FF'O", {255}},                                               //
                      OctetstringParam{"'0001'O", {0, 1}},                                            //
                      OctetstringParam{"'0102'O", {1, 2}},                                            //
                      OctetstringParam{"'0A0B'O", {10, 11}},                                          //
                      OctetstringParam{"'A1B2C3'O", {161, 178, 195}},                                 //
                      OctetstringParam{"'00010203'O", {0, 1, 2, 3}},                                  //
                      OctetstringParam{"'DEADBEEF'O", {222, 173, 190, 239}},                          //
                      OctetstringParam{"'FFFFFFFF'O", {255, 255, 255, 255}},                          //
                      OctetstringParam{"'00000000'O", {0, 0, 0, 0}},                                  //
                      OctetstringParam{"'7FFFFFFF'O", {127, 255, 255, 255}},                          //
                      OctetstringParam{"'80000000'O", {128, 0, 0, 0}},                                //
                      OctetstringParam{"'0123456789ABCDEF'O", {1, 35, 69, 103, 137, 171, 205, 239}},  //
                      OctetstringParam{"'00010203040506070809'O", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}}));

//

struct BitstringParam {
  std::string_view input;
  std::vector<std::uint8_t> expected;
  std::uint32_t length;
  DEFINE_NAMED_TEST_PARAM_PRINTER(BitstringParam, input);
};

struct BitstringLiteralParserTest : public ::testing::TestWithParam<BitstringParam> {};

TEST_P(BitstringLiteralParserTest, Parse) {
  const auto& tc = GetParam();
  const auto& [res, len] = literals::ParseBitstring(tc.input);
  EXPECT_EQ(res, tc.expected);
  EXPECT_EQ(len, tc.length);
}

INSTANTIATE_TEST_SUITE_P(Bitstring, BitstringLiteralParserTest,
                         ::testing::Values(BitstringParam{"''B", {}, 0},                                     //
                                                                                                             //
                                           BitstringParam{"'00'B", {0x00}, 2},                               //
                                           BitstringParam{"'01'B", {0x40}, 2},                               //
                                           BitstringParam{"'10'B", {0x80}, 2},                               //
                                           BitstringParam{"'11'B", {0xC0}, 2},                               //
                                           BitstringParam{"'0001'B", {0x10}, 4},                             //
                                           BitstringParam{"'0010'B", {0x20}, 4},                             //
                                           BitstringParam{"'1111'B", {0xF0}, 4},                             //
                                           BitstringParam{"'00000000'B", {0x00}, 8},                         //
                                           BitstringParam{"'11111111'B", {0xFF}, 8},                         //
                                           BitstringParam{"'10101010'B", {0xAA}, 8},                         //
                                           BitstringParam{"'01010101'B", {0x55}, 8},                         //
                                           BitstringParam{"'0000000011111111'B", {0x00, 0xFF}, 16},          //
                                           BitstringParam{"'1111000010101010'B", {0xF0, 0xAA}, 16},          //
                                           BitstringParam{"'0000111100001111'B", {0x0F, 0x0F}, 16},          //
                                                                                                             //
                                           BitstringParam{"'0'B", {0x00}, 1},                                //
                                           BitstringParam{"'1'B", {0x80}, 1},                                //
                                           BitstringParam{"'000'B", {0x00}, 3},                              //
                                           BitstringParam{"'101'B", {0xA0}, 3},                              //
                                           BitstringParam{"'0000001'B", {0x02}, 7},                          //
                                           BitstringParam{"'1111111'B", {0xFE}, 7},                          //
                                           BitstringParam{"'1010101'B", {0xAA}, 7},                          //
                                           BitstringParam{"'10101'B", {0xA8}, 5},                            //
                                           BitstringParam{"'10101010'B", {0xAA}, 8},                         //
                                           BitstringParam{"'101010101'B", {0xAA, 0x80}, 9},                  //
                                           BitstringParam{"'1010101010'B", {0xAA, 0x80}, 10},                //
                                           BitstringParam{"'101010101010'B", {0xAA, 0xA0}, 12},              //
                                                                                                             //
                                           BitstringParam{"'1111000011110000'B", {0xF0, 0xF0}, 16},          //
                                           BitstringParam{"'0000111100001111'B", {0x0F, 0x0F}, 16},          //
                                           BitstringParam{"'1111111100000001'B", {0xFF, 0x01}, 16},          //
                                           BitstringParam{"'1000000000000001'B", {0x80, 0x01}, 16},          //
                                           BitstringParam{"'111111110000000011'B", {0xFF, 0x00, 0xC0}, 18},  //
                                           BitstringParam{"'10101010101010101'B", {0xAA, 0xAA, 0x80}, 17},   //
                                           BitstringParam{"'000000001111111100000001'B", {0x00, 0xFF, 0x01}, 24}));

//

struct HexstringParam {
  std::string_view input;
  std::vector<std::uint8_t> expected;
  std::uint32_t length;
  DEFINE_NAMED_TEST_PARAM_PRINTER(HexstringParam, input);
};

struct HexstringLiteralParserTest : public ::testing::TestWithParam<HexstringParam> {};

TEST_P(HexstringLiteralParserTest, Parse) {
  const auto& tc = GetParam();
  const auto& [res, len] = literals::ParseHexstring(tc.input);
  EXPECT_EQ(res, tc.expected);
  EXPECT_EQ(len, tc.length);
}

INSTANTIATE_TEST_SUITE_P(Hexstring, HexstringLiteralParserTest,
                         ::testing::Values(HexstringParam{"''H", {}, 0},                                         //
                                                                                                                 //
                                           HexstringParam{"'0'H", {0x00}, 1},                                    //
                                           HexstringParam{"'1'H", {0x10}, 1},                                    //
                                           HexstringParam{"'A'H", {0xA0}, 1},                                    //
                                           HexstringParam{"'F'H", {0xF0}, 1},                                    //
                                           HexstringParam{"'00'H", {0x00}, 2},                                   //
                                           HexstringParam{"'01'H", {0x01}, 2},                                   //
                                           HexstringParam{"'0A'H", {0x0A}, 2},                                   //
                                           HexstringParam{"'10'H", {0x10}, 2},                                   //
                                           HexstringParam{"'7F'H", {0x7F}, 2},                                   //
                                           HexstringParam{"'80'H", {0x80}, 2},                                   //
                                           HexstringParam{"'FF'H", {0xFF}, 2},                                   //
                                           HexstringParam{"'0001'H", {0x00, 0x01}, 4},                           //
                                           HexstringParam{"'0A0B'H", {0x0A, 0x0B}, 4},                           //
                                           HexstringParam{"'DEAD'H", {0xDE, 0xAD}, 4},                           //
                                           HexstringParam{"'BEEF'H", {0xBE, 0xEF}, 4},                           //
                                           HexstringParam{"'CAFE'H", {0xCA, 0xFE}, 4},                           //
                                                                                                                 //
                                           HexstringParam{"'01234567'H", {0x01, 0x23, 0x45, 0x67}, 8},           //
                                           HexstringParam{"'89ABCDEF'H", {0x89, 0xAB, 0xCD, 0xEF}, 8},           //
                                                                                                                 //
                                           HexstringParam{"'A'H", {0xA0}, 1},                                    //
                                           HexstringParam{"'1A3'H", {0x1A, 0x30}, 3},                            //
                                           HexstringParam{"'ABC'H", {0xAB, 0xC0}, 3},                            //
                                           HexstringParam{"'F0D9E'H", {0xF0, 0xD9, 0xE0}, 5},                    //
                                           HexstringParam{"'0011223344'H", {0x00, 0x11, 0x22, 0x33, 0x44}, 10},  //
                                                                                                                 //
                                           HexstringParam{"'FFFF00FF'H", {0xFF, 0xFF, 0x00, 0xFF}, 8},           //
                                           HexstringParam{"'0F0F0F0F'H", {0x0F, 0x0F, 0x0F, 0x0F}, 8},           //
                                           HexstringParam{"'F0F0F0F0'H", {0xF0, 0xF0, 0xF0, 0xF0}, 8},           //
                                           HexstringParam{"'00010203'H", {0x00, 0x01, 0x02, 0x03}, 8},           //
                                           HexstringParam{"'10203040'H", {0x10, 0x20, 0x30, 0x40}, 8},           //
                                           HexstringParam{"'A1B2C3D4'H", {0xA1, 0xB2, 0xC3, 0xD4}, 8},           //
                                                                                                                 //
                                           HexstringParam{"'1A2B3C4D5E'H", {0x1A, 0x2B, 0x3C, 0x4D, 0x5E}, 10},  //
                                           HexstringParam{"'ABCDEF0123'H", {0xAB, 0xCD, 0xEF, 0x01, 0x23}, 10},  //

                                           HexstringParam{"'00112233445566778899AABBCCDDEEFF'H",
                                                          {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99,
                                                           0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},
                                                          32}));
