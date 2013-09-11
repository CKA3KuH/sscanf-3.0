#include "../utils.h"


// Test valid formats.
TEST(Oct0,  { cell n; return Utils::ReadOctal(S"0", n) == OK && n == 0; })
TEST(Oct1,  { cell n; return Utils::ReadOctal(S"066", n) == OK && n == 066; })
TEST(Oct2,  { cell n; return Utils::ReadOctal(S"088", n) == OK && n == 0; })
TEST(Oct4,  { cell n; return Utils::ReadOctal(S"01234", n) == OK && n == 01234; })
TEST(Oct6,  { cell n; return Utils::ReadOctal(S"5k", n) == OK && n == 05; })
TEST(Oct7,  { cell n; return Utils::ReadOctal(S"7|", n) == OK && n == 07; })
TEST(Oct8,  { cell n; return Utils::ReadOctal(S"0044(", n) == OK && n == 044; })
// Test fails.
TEST(Oct5,  { cell n; return Utils::ReadOctal(S"-0", n) == ERROR_NAN; })
TEST(Oct3,  { cell n; return Utils::ReadOctal(S"99", n) == ERROR_NAN; })
TEST(Oct9,  { cell n; return Utils::ReadOctal(S"(3)", n) == ERROR_NAN; })
TEST(Oct10, { cell n; return Utils::ReadOctal(S"", n) == ERROR_NAN; })



// Test valid formats.
TEST(Dec0,  { cell n; return Utils::ReadDecimal(S"0", n) == OK && n == 0; })
TEST(Dec1,  { cell n; return Utils::ReadDecimal(S"24", n) == OK && n == 24; })
TEST(Dec1c, { cell n; return Utils::ReadDecimal(S"-55", n) == OK && n == -55; })
TEST(Dec1d, { cell n; return Utils::ReadDecimal(S"-193", n) == OK && n == -193; })
TEST(Dec1e, { cell n; return Utils::ReadDecimal(S"-067", n) == OK && n == -67; })
TEST(Dec5b, { cell n; return Utils::ReadDecimal(S"0000088", n) == OK && n == 88; })
// Test ends.
TEST(Dec6,  { cell n; return Utils::ReadDecimal(S"7!", n) == OK && n == 7; })
TEST(Dec7,  { cell n; return Utils::ReadDecimal(S"6)", n) == OK && n == 6; })
TEST(Dec8,  { cell n; return Utils::ReadDecimal(S"5G", n) == OK && n == 5; })
// Test not hex.
TEST(Dec2,  { cell n; return Utils::ReadDecimal(S"0x1", n) == OK && n == 0; })
TEST(Dec3,  { cell n; return Utils::ReadDecimal(S"FF", n) == ERROR_NAN; })
TEST(Dec4,  { cell n; return Utils::ReadDecimal(S"5C", n) == OK && n == 5; })
// Test invalid formats.
TEST(Dec9,  { cell n; return Utils::ReadDecimal(S"q", n) == ERROR_NAN; })
TEST(Dec10, { cell n; return Utils::ReadDecimal(S"(6)", n) == ERROR_NAN; })
TEST(Dec12, { cell n; return Utils::ReadDecimal(S"!", n) == ERROR_NAN; })
TEST(Dec17, { cell n; return Utils::ReadDecimal(S"-", n) == ERROR_NAN; })
TEST(Dec18, { cell n; return Utils::ReadDecimal(S"", n) == ERROR_NAN; })
// Test all digits.
TEST(Dec13, { cell n; return Utils::ReadDecimal(S"123", n) == OK && n == 123; })
TEST(Dec14, { cell n; return Utils::ReadDecimal(S"456", n) == OK && n == 456; })
TEST(Dec15, { cell n; return Utils::ReadDecimal(S"789", n) == OK && n == 789; })
TEST(Dec16, { cell n; return Utils::ReadDecimal(S"-0", n) == OK && n == 0; })



cell
	GetHexCharacter(char const c);

// Tests.
TEST(HexChar0, { return GetHexCharacter('0') == 0; })
TEST(HexChar1, { return GetHexCharacter('1') == 1; })
TEST(HexChar2, { return GetHexCharacter('2') == 2; })
TEST(HexChar3, { return GetHexCharacter('3') == 3; })
TEST(HexChar4, { return GetHexCharacter('4') == 4; })
TEST(HexChar5, { return GetHexCharacter('5') == 5; })
TEST(HexChar6, { return GetHexCharacter('6') == 6; })
TEST(HexChar7, { return GetHexCharacter('7') == 7; })
TEST(HexChar8, { return GetHexCharacter('8') == 8; })
TEST(HexChar9, { return GetHexCharacter('9') == 9; })
TEST(HexChara, { return GetHexCharacter('a') == 10; })
TEST(HexCharb, { return GetHexCharacter('b') == 11; })
TEST(HexCharc, { return GetHexCharacter('c') == 12; })
TEST(HexChard, { return GetHexCharacter('d') == 13; })
TEST(HexChare, { return GetHexCharacter('e') == 14; })
TEST(HexCharf, { return GetHexCharacter('f') == 15; })
TEST(HexCharA, { return GetHexCharacter('A') == 10; })
TEST(HexCharB, { return GetHexCharacter('B') == 11; })
TEST(HexCharC, { return GetHexCharacter('C') == 12; })
TEST(HexCharD, { return GetHexCharacter('D') == 13; })
TEST(HexCharE, { return GetHexCharacter('E') == 14; })
TEST(HexCharF, { return GetHexCharacter('F') == 15; })
TEST(HexCharg, { return GetHexCharacter('g') == -1; })
TEST(HexCharG, { return GetHexCharacter('G') == -1; })
TEST(HexCharo0, { return GetHexCharacter('\0') == -1; })
TEST(HexCharo1, { return GetHexCharacter('\\') == -1; })
TEST(HexCharo2, { return GetHexCharacter('z') == -1; })
TEST(HexCharo3, { return GetHexCharacter(':') == -1; })

// Test valid formats.
TEST(Hex0,  { cell n; return Utils::ReadHex(S"0", n) == OK && n == 0; })
TEST(Hex1,  { cell n; return Utils::ReadHex(S"24", n) == OK && n == 0x24; })
TEST(Hex2,  { cell n; return Utils::ReadHex(S"0x24", n) == OK && n == 0x24; })
TEST(Hex3,  { cell n; return Utils::ReadHex(S"0X24", n) == OK && n == 0x24; })
TEST(Hex4a, { cell n; return Utils::ReadHex(S"2f", n) == OK && n == 0x2F; })
TEST(Hex4b, { cell n; return Utils::ReadHex(S"2F", n) == OK && n == 0x2f; })
TEST(Hex5a, { cell n; return Utils::ReadHex(S"0x000088", n) == OK && n == 0x88; })
TEST(Hex5b, { cell n; return Utils::ReadHex(S"0000088", n) == OK && n == 0x88; })
TEST(Hex5c, { cell n; return Utils::ReadHex(S"00000x88", n) == OK && n == 0; })
TEST(Hex5d, { cell n; return Utils::ReadHex(S"000x88", n) == OK && n == 0; })
TEST(Hex5e, { cell n; return Utils::ReadHex(S"0X88", n) == OK && n == 0x88; })
// Test ends.
TEST(Hex6,  { cell n; return Utils::ReadHex(S"E!", n) == OK && n == 0xE; })
TEST(Hex7,  { cell n; return Utils::ReadHex(S"B)", n) == OK && n == 0xB; })
TEST(Hex8,  { cell n; return Utils::ReadHex(S"CG", n) == OK && n == 0xC; })
// Test invalid formats.
TEST(Hex9,  { cell n; return Utils::ReadHex(S"G", n) == ERROR_NAN; })
TEST(Hex10, { cell n; return Utils::ReadHex(S"(6)", n) == ERROR_NAN; })
TEST(Hex11, { cell n; return Utils::ReadHex(S"-8", n) == ERROR_NAN; })
TEST(Hex12, { cell n; return Utils::ReadHex(S"", n) == ERROR_NAN; })
// Test all digits.
TEST(Hex13, { cell n; return Utils::ReadHex(S"0x123", n) == OK && n == 0x123; })
TEST(Hex14, { cell n; return Utils::ReadHex(S"0x456", n) == OK && n == 0x456; })
TEST(Hex15, { cell n; return Utils::ReadHex(S"0x789", n) == OK && n == 0x789; })
TEST(Hex16, { cell n; return Utils::ReadHex(S"0x0AB", n) == OK && n == 0x0AB; })
TEST(Hex17, { cell n; return Utils::ReadHex(S"0xCDE", n) == OK && n == 0xCDE; })
TEST(Hex18, { cell n; return Utils::ReadHex(S"0xFab", n) == OK && n == 0xFab; })
TEST(Hex19, { cell n; return Utils::ReadHex(S"0xcde", n) == OK && n == 0xcde; })
TEST(Hex20, { cell n; return Utils::ReadHex(S"0xf77", n) == OK && n == 0xf77; })


// Test valid formats (annoyingly, C doesn't support 0b0 syntax).
TEST(Bin0,  { cell n; return Utils::ReadBinary(S"0", n) == OK && n == 0; })
TEST(Bin1,  { cell n; return Utils::ReadBinary(S"1", n) == OK && n == 1; })
TEST(Bin2,  { cell n; return Utils::ReadBinary(S"010101", n) == OK && n == 21; })
TEST(Bin3,  { cell n; return Utils::ReadBinary(S"111000", n) == OK && n == 56; })
TEST(Bin4a, { cell n; return Utils::ReadBinary(S"0b000111", n) == OK && n == 7; })
TEST(Bin4b, { cell n; return Utils::ReadBinary(S"0B11", n) == OK && n == 3; })
TEST(Bin5a, { cell n; return Utils::ReadBinary(S"10121", n) == OK && n == 5; })
// Test ends.
TEST(Bin6,  { cell n; return Utils::ReadBinary(S"1100!", n) == OK && n == 12; })
TEST(Bin7,  { cell n; return Utils::ReadBinary(S"0b1001)", n) == OK && n == 9; })
TEST(Bin7b, { cell n; return Utils::ReadBinary(S"0b0110)", n) == OK && n == 6; })
TEST(Bin8,  { cell n; return Utils::ReadBinary(S"0B1111G", n) == OK && n == 15; })
// Test invalid formats.
TEST(Bin9,  { cell n; return Utils::ReadBinary(S"2", n) == ERROR_NAN; })
TEST(Bin10, { cell n; return Utils::ReadBinary(S"(1)", n) == ERROR_NAN; })
TEST(Bin11, { cell n; return Utils::ReadBinary(S"-0", n) == ERROR_NAN; })
TEST(Bin12, { cell n; return Utils::ReadBinary(S"", n) == ERROR_NAN; })


TEST(Num0,  { cell n; return Utils::ReadNum(S"0", n) == OK && n == 0; })
TEST(Num1,  { cell n; return Utils::ReadNum(S"1", n) == OK && n == 1; })
TEST(Num2,  { cell n; return Utils::ReadNum(S"7", n) == OK && n == 7; })
TEST(Num3,  { cell n; return Utils::ReadNum(S"9", n) == OK && n == 9; })
TEST(Num4,  { cell n; return Utils::ReadNum(S"F", n) == OK && n == 0xF; })
TEST(Num5,  { cell n; return Utils::ReadNum(S"0A", n) == OK && n == 0xA; })
TEST(Num6,  { cell n; return Utils::ReadNum(S"0x1", n) == OK && n == 0x1; })
TEST(Num7,  { cell n; return Utils::ReadNum(S"0Xb", n) == OK && n == 0xB; })
TEST(Num8,  { cell n; return Utils::ReadNum(S"-0", n) == OK && n == -0; })
TEST(Num9,  { cell n; return Utils::ReadNum(S"-5", n) == OK && n == -5; })
TEST(Num10, { cell n; return Utils::ReadNum(S"01234", n) == OK && n == 01234; })
TEST(Num11, { cell n; return Utils::ReadNum(S"1234", n) == OK && n == 1234; })
TEST(Num12, { cell n; return Utils::ReadNum(S"06789", n) == OK && n == 6789; })
TEST(Num13, { cell n; return Utils::ReadNum(S"6789", n) == OK && n == 6789; })
TEST(Num22, { cell n; return Utils::ReadNum(S"0b1234", n) == OK && n == 0x0B1234; })
// SPECIAL CASE - looks like binary, but isn't!
TEST(Num14, { cell n; return Utils::ReadNum(S"010101", n) == OK && n == 010101; })
TEST(Num15, { cell n; return Utils::ReadNum(S"101010", n) == OK && n == 101010; })
TEST(Num16, { cell n; return Utils::ReadNum(S"0b101010", n) == OK && n == 42; })
TEST(Num17, { cell n; return Utils::ReadNum(S"0B010101", n) == OK && n == 21; })
TEST(Num18, { cell n; return Utils::ReadNum(S"0123F", n) == OK && n == 0x0123F; })
TEST(Num19, { cell n; return Utils::ReadNum(S"-A", n) == ERROR_NAN; })
TEST(Num20, { cell n; return Utils::ReadNum(S"G", n) == ERROR_NAN; })
TEST(Num21, { cell n; return Utils::ReadNum(S"(4)", n) == ERROR_NAN; })


TEST(Char0,  { cell n; return Utils::ReadChar(S"0", n) == OK && n == '0'; })
TEST(Char1,  { cell n; return Utils::ReadChar(S"x", n) == OK && n == 'x'; })
TEST(Char2a, { cell n; return Utils::ReadChar(S"\\\\", n) == OK && n == '\\'; })
TEST(Char2b, { cell n; return Utils::ReadChar(S"\\n", n) == OK && n == '\n'; })
TEST(Char2c, { cell n; return Utils::ReadChar(S"\\0", n) == OK && n == 0; })
TEST(Char2d, { cell n; return Utils::ReadChar(S"\\055", n) == OK && n == 055; })
TEST(Char2e, { cell n; return Utils::ReadChar(S"\\055;", n) == OK && n == 055; })
TEST(Char2f, { cell n; return Utils::ReadChar(S"\\'", n) == OK && n == '\''; })
TEST(Char2g, { cell n; return Utils::ReadChar(S"\\", n) == ERROR_INVALID_ESCAPE; })
TEST(Char2h, { cell n; return Utils::ReadChar(S"\\y", n) == ERROR_INVALID_ESCAPE; })
TEST(Char2i, { cell n; return Utils::ReadChar(S"\\9", n) == OK && n == 9; })
TEST(Char2j, { cell n; return Utils::ReadChar(S"\\x;", n) == ERROR_NAN; })
TEST(Char2k, { cell n; return Utils::ReadChar(S"\\xH;", n) == ERROR_NAN; })
TEST(Char3,  { cell n; return Utils::ReadChar(S"'g'", n) == OK && n == '\''; })
TEST(Char4a, { cell n; return Utils::ReadCharEx(S"'g'", n) == OK && n == 'g'; })
TEST(Char4b, { cell n; return Utils::ReadCharEx(S"g", n) == OK && n == 'g'; })
TEST(Char4c, { cell n; return Utils::ReadCharEx(S"", n) == ERROR_NAN; })
TEST(Char4d, { cell n; return Utils::ReadCharEx(S"\\'", n) == OK && n == '\''; })
TEST(Char4e, { cell n; return Utils::ReadCharEx(S"'\\''", n) == OK && n == '\''; })
TEST(Char4f, { cell n; return Utils::ReadCharEx(S"\\'j'", n) == OK && n == '\''; })


TEST(GetBound0,  { char const * c; return Utils::GetBounded(S"   hello\"", c, '"', '"') == ERROR_EXPECTED_A_GOT_B_1; })
TEST(GetBound1,  { char const * c; return Utils::GetBounded(S"   \"hello", c, '"', '"') == ERROR_EXPECTED_A_GOT_B_2; })
TEST(GetBound2,  { char const * c; return Utils::GetBounded(S"   \"hello  \"", c, '"', '"') == OK; })
TEST(GetBound3,  { char const * c; return Utils::GetBounded(S"   \"hello  \"", c, '"', '"') == OK && std::string(c) == "hello"; })
TEST(GetBound4,  { char const * c; return Utils::GetBounded(S"   \"   hello  \"", c, '"', '"') == OK && std::string(c) == "hello"; })
TEST(GetBound5,  { char const * c; return Utils::GetBounded(S"   \"hello\"", c, '"', '"') == OK && std::string(c) == "hello"; })
TEST(GetBound6,  { char const * c; return Utils::GetBounded(S"   (hello	)", c, '(', ')') == OK && std::string(c) == "hello"; })
TEST(GetBound13, { char const * c; return Utils::GetBounded(S"   (hel()lo	)", c, '(', ')') == OK && std::string(c) == "hel()lo"; })
TEST(GetBound14, { char const * c; return Utils::GetBounded(S"   (he<>llo	)", c, '(', ')') == OK && std::string(c) == "he<>llo"; })
TEST(GetBound7,  { char const * c; return Utils::GetBounded(S"   <	hello	>", c, '<', '>') == OK && std::string(c) == "hello"; })
TEST(GetBound8,  { char const * c; return Utils::GetBounded(S"   [hello]", c, '[', ']') == OK && std::string(c) == "hello"; })
TEST(GetBound9,  { char const * c; size_t len; return Utils::GetBounded(S"   [hello ]", c, '[', ']', &len) == OK && len == 5; })
TEST(GetBound10, { char const * c; size_t len; return Utils::GetBounded(S"   [ hello g]", c, '[', ']', &len) == OK && len == 7; })
TEST(GetBound11, { char const * c; size_t len; return Utils::GetBounded(S"   !h ello   543#", c, '!', '#', &len) == OK && len == 12; })
TEST(GetBound15, { char const * c; size_t len; return Utils::GetBounded(S"   \"!h ello   5\\\"3#\"", c, '"', '"', &len) == OK && len == 14 && std::string(c) == "!h ello   5\\\"3#"; })



TEST(GetLength0,  { int s; return Utils::GetLength(S"[]", &s) == OK && s == 0; })
TEST(GetLength1,  { int s; return Utils::GetLength(S"[0]", &s) == ERROR_INVALID_ARRAY_SIZE; })
TEST(GetLength2,  { int s; return Utils::GetLength(S"[ -14 ]", &s) == ERROR_INVALID_ARRAY_SIZE; })
TEST(GetLength3,  { int s; return Utils::GetLength(S"[ ]", &s) == OK && s == 0; })
TEST(GetLength4,  { int s; return Utils::GetLength(S"[44]", &s) == OK && s == 44; })
TEST(GetLength5,  { int s; return Utils::GetLength(S"[ * ]", &s) == OK && s == -1; })
TEST(GetLength6,  { int s; return Utils::GetLength(S"[ * ]", &s) == OK && s == -1; })
TEST(GetLength7,  { int s; return Utils::GetLength(S"[ 78 ", &s) == ERROR_NO_ARRAY_END; })
TEST(GetLength8,  { int s; return Utils::GetLength(S" 15 ]", &s) == ERROR_NO_ARRAY_START; })
TEST(GetLength9,  { int s; return Utils::GetLength(S" [ 1000 ] ", &s) == OK && s == 1000; })
TEST(GetLength10, { int s; return Utils::GetLength(S" [ 0x44 ] ", &s) == ERROR_NAN; })
TEST(GetLength11, { int s; return Utils::GetLength(S" [ HI ] ", &s) == ERROR_NAN; })



