#include "../specifiers/simple_specifiers.h"
#include "../specifiers/specnum_specifiers.h"

error_t
	GetRanges(Specifier * that, char const * pars, int hyphens, cell * lower, cell * higher);

TEST(Range0,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"5 - 7", 1, &l, &h) == OK && l == 5 && h == 7; })
TEST(Range1,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"-5 - 7", 2, &l, &h) == OK && l == -5 && h == 7; })
TEST(Range2,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"5 - -7", 2, &l, &h) == ERROR_INVALID_RANGE; })
TEST(Range3,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"-7 - -5", 3, &l, &h) == OK && l == -7 && h == -5; })

TEST(Range4,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"- 5", 1, &l, &h) == OK && l == 0x80000000 && h == 5; })
TEST(Range5,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"7 -", 1, &l, &h) == OK && l == 7 && h == 0x7FFFFFFF; })

TEST(Range6,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"- -5", 2, &l, &h) == OK && l == 0x80000000 && h == -5; })
TEST(Range7,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"-7 -", 2, &l, &h) == OK && l == -7 && h == 0x7FFFFFFF; })
TEST(Range8,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"-7 - 5", 2, &l, &h) == OK && l == -7 && h == 5; })
TEST(Range9,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"-7 - -5", 3, &l, &h) == OK && l == -7 && h == -5; })


TEST(SimpleP00, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<-5 - -1>") == OK; })
TEST(SimpleP01, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<   - -1>") == OK; })
TEST(SimpleP02, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<-5 -   >") == OK; })
TEST(SimpleP03, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i< 5 - -1>") == ERROR_INVALID_RANGE; })
TEST(SimpleP04, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<-5 -  1>") == OK; })
TEST(SimpleP05, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i< 6 - 2 >") == ERROR_INVALID_RANGE; })
TEST(SimpleP06, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i< 2 -6  >") == OK; })
TEST(SimpleP07, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<5 1>") == ERROR_INVALID_RANGE; })
TEST(SimpleP08, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<>") == ERROR_INVALID_RANGE; })
TEST(SimpleP09, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<---->") == ERROR_INVALID_RANGE; })

TEST(Simple9a, { cell dest; SimpleSpecifier that('i', &Utils::ReadDecimal); return that.Run(S"44", DefaultEnvironment::Get(&dest)) == OK && dest == 44; })
TEST(Simple9b, { cell dest; SimpleSpecifier that('d', &Utils::ReadDecimal); return that.Run(S"00001", DefaultEnvironment::Get(&dest)) == OK && dest == 1; })
TEST(Simple9c, { cell dest; SimpleSpecifier that('c', &Utils::ReadCharEx);  return that.Run(S"'t'", DefaultEnvironment::Get(&dest)) == OK && dest == 't'; })
TEST(Simple9h, { cell dest; SimpleSpecifier that('c', &Utils::ReadCharEx);  return that.Run(S"u", DefaultEnvironment::Get(&dest)) == OK && dest == 'u'; })
TEST(Simple9d, { cell dest; SimpleSpecifier that('h', &Utils::ReadHex);     return that.Run(S"123", DefaultEnvironment::Get(&dest)) == OK && dest == 0x123; })
TEST(Simple9e, { cell dest; SimpleSpecifier that('x', &Utils::ReadHex);     return that.Run(S"0x123", DefaultEnvironment::Get(&dest)) == OK && dest == 0x123; })
TEST(Simple9i, { cell dest; SimpleSpecifier that('h', &Utils::ReadHex);     return that.Run(S"FF", DefaultEnvironment::Get(&dest)) == OK && dest == 0xFF; })
TEST(Simple9l, { cell dest; SimpleSpecifier that('x', &Utils::ReadHex);     return that.Run(S"0xAD", DefaultEnvironment::Get(&dest)) == OK && dest == 0xAD; })
TEST(Simple9f, { cell dest; SimpleSpecifier that('o', &Utils::ReadOctal);   return that.Run(S"76", DefaultEnvironment::Get(&dest)) == OK && dest == 076; })
TEST(Simple9j, { cell dest; SimpleSpecifier that('o', &Utils::ReadOctal);   return that.Run(S"054", DefaultEnvironment::Get(&dest)) == OK && dest == 054; })
TEST(Simple9g, { cell dest; SimpleSpecifier that('b', &Utils::ReadBinary);  return that.Run(S"010101", DefaultEnvironment::Get(&dest)) == OK && dest == 21; })
TEST(Simple9k, { cell dest; SimpleSpecifier that('b', &Utils::ReadBinary);  return that.Run(S"0b101010", DefaultEnvironment::Get(&dest)) == OK && dest == 42; })

// Valid formats.
TEST(Simple1a, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i") == OK; })
// These tests have been removed - they are valid, but I no longer check for
// this case because it can't actually happen.
//TEST(Simple1b, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"d") == ERROR_EXPECTED_A_GOT_B_2; })
//TEST(Simple1v, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"") == ERROR_EXPECTED_A_GOT_B_2; })
//TEST(Simple1w, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"(") == ERROR_EXPECTED_A_GOT_B_2; })
TEST(Simple1c, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I") == ERROR_NO_DEFAULT_START; })

// These don't fail.
TEST(SimpleH04, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H(55 )") == OK && *CUR == '\0'; })
TEST(SimpleH05, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H (55 )") == OK && *CUR == '\0'; })
TEST(SimpleH06, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H(55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleH07, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H (55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleH12, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H( 55 )") == OK && *CUR == '\0'; })
TEST(SimpleH13, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H ( 55 )") == OK && *CUR == '\0'; })
TEST(SimpleH14, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H( 55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleH15, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H ( 55 ) ") == OK && *CUR == '\0'; })
// These DO for some reason...  They all DON'T have a space before the close
// bracket :/.
TEST(SimpleH00, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H(55)") == OK && *CUR == '\0'; })
TEST(SimpleH01, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H (55)") == OK && *CUR == '\0'; })
TEST(SimpleH02, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H(55) ") == OK && *CUR == '\0'; })
TEST(SimpleH03, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H (55) ") == OK && *CUR == '\0'; })
TEST(SimpleH08, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H( 55)") == OK && *CUR == '\0'; })
TEST(SimpleH09, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H ( 55)") == OK && *CUR == '\0'; })
TEST(SimpleH10, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H( 55) ") == OK && *CUR == '\0'; })
TEST(SimpleH11, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H ( 55) ") == OK && *CUR == '\0'; })

TEST(SimpleI00, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(55)") == OK && *CUR == '\0'; })
TEST(SimpleI01, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55)") == OK && *CUR == '\0'; })
TEST(SimpleI02, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(55) ") == OK && *CUR == '\0'; })
TEST(SimpleI03, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55) ") == OK && *CUR == '\0'; })
TEST(SimpleI04, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(55 )") == OK && *CUR == '\0'; })
TEST(SimpleI05, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55 )") == OK && *CUR == '\0'; })
TEST(SimpleI06, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleI07, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleI08, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( 55)") == OK && *CUR == '\0'; })
TEST(SimpleI09, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I ( 55)") == OK && *CUR == '\0'; })
TEST(SimpleI10, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( 55) ") == OK && *CUR == '\0'; })
TEST(SimpleI11, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I ( 55) ") == OK && *CUR == '\0'; })
TEST(SimpleI12, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( 55 )") == OK && *CUR == '\0'; })
TEST(SimpleI13, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I ( 55 )") == OK && *CUR == '\0'; })
TEST(SimpleI14, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( 55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleI15, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I ( 55 ) ") == OK && *CUR == '\0'; })

TEST(SimpleO00, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O(55)") == OK && *CUR == '\0'; })
TEST(SimpleO01, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O (55)") == OK && *CUR == '\0'; })
TEST(SimpleO02, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O(55) ") == OK && *CUR == '\0'; })
TEST(SimpleO03, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O (55) ") == OK && *CUR == '\0'; })
TEST(SimpleO04, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O(55 )") == OK && *CUR == '\0'; })
TEST(SimpleO05, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O (55 )") == OK && *CUR == '\0'; })
TEST(SimpleO06, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O(55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleO07, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O (55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleO08, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O( 55)") == OK && *CUR == '\0'; })
TEST(SimpleO09, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O ( 55)") == OK && *CUR == '\0'; })
TEST(SimpleO10, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O( 55) ") == OK && *CUR == '\0'; })
TEST(SimpleO11, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O ( 55) ") == OK && *CUR == '\0'; })
TEST(SimpleO12, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O( 55 )") == OK && *CUR == '\0'; })
TEST(SimpleO13, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O ( 55 )") == OK && *CUR == '\0'; })
TEST(SimpleO14, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O( 55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleO15, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O ( 55 ) ") == OK && *CUR == '\0'; })

TEST(SimpleB00, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B(101010)") == OK && *CUR == '\0'; })
TEST(SimpleB01, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B (101010)") == OK && *CUR == '\0'; })
TEST(SimpleB02, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B(101010) ") == OK && *CUR == '\0'; })
TEST(SimpleB03, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B (101010) ") == OK && *CUR == '\0'; })
TEST(SimpleB04, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B(101010 )") == OK && *CUR == '\0'; })
TEST(SimpleB05, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B (101010 )") == OK && *CUR == '\0'; })
TEST(SimpleB06, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B(101010 ) ") == OK && *CUR == '\0'; })
TEST(SimpleB07, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B (101010 ) ") == OK && *CUR == '\0'; })
TEST(SimpleB08, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B( 101010)") == OK && *CUR == '\0'; })
TEST(SimpleB09, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B ( 101010)") == OK && *CUR == '\0'; })
TEST(SimpleB10, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B( 101010) ") == OK && *CUR == '\0'; })
TEST(SimpleB11, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B ( 101010) ") == OK && *CUR == '\0'; })
TEST(SimpleB12, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B( 101010 )") == OK && *CUR == '\0'; })
TEST(SimpleB13, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B ( 101010 )") == OK && *CUR == '\0'; })
TEST(SimpleB14, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B( 101010 ) ") == OK && *CUR == '\0'; })
TEST(SimpleB15, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B ( 101010 ) ") == OK && *CUR == '\0'; })

TEST(SimpleC00, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C(@)") == OK && *CUR == '\0'; })
TEST(SimpleC01, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C (@)") == OK && *CUR == '\0'; })
TEST(SimpleC02, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C(@) ") == OK && *CUR == '\0'; })
TEST(SimpleC03, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C (@) ") == OK && *CUR == '\0'; })
TEST(SimpleC04, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C(@ )") == OK && *CUR == '\0'; })
TEST(SimpleC05, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C (@ )") == OK && *CUR == '\0'; })
TEST(SimpleC06, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C(@ ) ") == OK && *CUR == '\0'; })
TEST(SimpleC07, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C (@ ) ") == OK && *CUR == '\0'; })
TEST(SimpleC08, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( @)") == OK && *CUR == '\0'; })
TEST(SimpleC09, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( @)") == OK && *CUR == '\0'; })
TEST(SimpleC10, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( @) ") == OK && *CUR == '\0'; })
TEST(SimpleC11, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( @) ") == OK && *CUR == '\0'; })
TEST(SimpleC12, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( @ )") == OK && *CUR == '\0'; })
TEST(SimpleC13, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( @ )") == OK && *CUR == '\0'; })
TEST(SimpleC14, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( @ ) ") == OK && *CUR == '\0'; })
TEST(SimpleC15, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( @ ) ") == OK && *CUR == '\0'; })

TEST(Simplec00, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C('@')") == OK && *CUR == '\0'; })
TEST(Simplec01, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ('@')") == OK && *CUR == '\0'; })
TEST(Simplec02, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C('@') ") == OK && *CUR == '\0'; })
TEST(Simplec03, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ('@') ") == OK && *CUR == '\0'; })
TEST(Simplec04, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C('@' )") == OK && *CUR == '\0'; })
TEST(Simplec05, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ('@' )") == OK && *CUR == '\0'; })
TEST(Simplec06, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C('@' ) ") == OK && *CUR == '\0'; })
TEST(Simplec07, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ('@' ) ") == OK && *CUR == '\0'; })
TEST(Simplec08, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( '@')") == OK && *CUR == '\0'; })
TEST(Simplec09, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( '@')") == OK && *CUR == '\0'; })
TEST(Simplec10, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( '@') ") == OK && *CUR == '\0'; })
TEST(Simplec11, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( '@') ") == OK && *CUR == '\0'; })
TEST(Simplec12, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( '@' )") == OK && *CUR == '\0'; })
TEST(Simplec13, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( '@' )") == OK && *CUR == '\0'; })
TEST(Simplec14, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( '@' ) ") == OK && *CUR == '\0'; })
TEST(Simplec15, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( '@' ) ") == OK && *CUR == '\0'; })
// These all fail for the same reason - leading spaces...
/*TEST(SimpleH16, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H(55)") == OK && *CUR == '\0'; })
TEST(SimpleH17, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H (55)") == OK && *CUR == '\0'; })
TEST(SimpleH18, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H(55) ") == OK && *CUR == '\0'; })
TEST(SimpleH19, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H (55) ") == OK && *CUR == '\0'; })
TEST(SimpleH20, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H(55 )") == OK && *CUR == '\0'; })
TEST(SimpleH21, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H (55 )") == OK && *CUR == '\0'; })
TEST(SimpleH22, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H(55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleH23, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H (55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleH24, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H( 55)") == OK && *CUR == '\0'; })
TEST(SimpleH25, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H ( 55)") == OK && *CUR == '\0'; })
TEST(SimpleH26, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H( 55) ") == OK && *CUR == '\0'; })
TEST(SimpleH27, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H ( 55) ") == OK && *CUR == '\0'; })
TEST(SimpleH28, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H( 55 )") == OK && *CUR == '\0'; })
TEST(SimpleH29, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H ( 55 )") == OK && *CUR == '\0'; })
TEST(SimpleH30, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H( 55 ) ") == OK && *CUR == '\0'; })
TEST(SimpleH31, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H ( 55 ) ") == OK && *CUR == '\0'; })*/

TEST(Simple1d, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(55)") == OK && *CUR == '\0'; })
TEST(Simple1e, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55)") == OK && *CUR == '\0'; })
TEST(Simple1f, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (  55  )") == OK && *CUR == '\0'; })
TEST(Simple1g, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55  )") == OK && *CUR == '\0'; })
TEST(Simple1h, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(  55)  ") == OK && *CUR == '\0'; })
TEST(Simple1i, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( )") != OK; })
TEST(Simple1j, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I ( )  ") != OK; })
TEST(Simple1k, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(3") == ERROR_NO_DEFAULT_END; })
TEST(Simple1l, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( 4") == ERROR_NO_DEFAULT_END; })
TEST(Simple1m, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I   (  5") == ERROR_NO_DEFAULT_END; })
TEST(Simple1n, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(    7") == ERROR_NO_DEFAULT_END; })
TEST(Simple1o, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(") == ERROR_NO_DEFAULT_END; })
TEST(Simple1p, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(E)") == ERROR_NAN; })
TEST(Simple1q, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(    -6)") == OK && that.m_default == -6; })
TEST(Simple1r, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(78)") == OK && that.m_default == 78; })
TEST(Simple1s, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(1234)") == OK && that.m_default == 1234; })
TEST(Simple1t, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(595)") == OK && that.m_default == 595; })
TEST(Simple1u, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(0x11)") == ERROR_INVALID_DEFAULT; })

TEST(Simple2a, { SimpleSpecifier that('d', &Utils::ReadDecimal); return that.ReadToken(S"d") == OK; })
//TEST(Simple2b, { SimpleSpecifier that('d', &Utils::ReadDecimal); return that.ReadToken(S"i") == ERROR_EXPECTED_A_GOT_B_2; })
TEST(Simple2c, { SimpleSpecifier that('d', &Utils::ReadDecimal); return that.ReadToken(S"D") == ERROR_NO_DEFAULT_START; })

TEST(Simple3a, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"c") == OK; })
//TEST(Simple3b, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"v") == ERROR_EXPECTED_A_GOT_B_2; })
TEST(Simple3c, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C") == ERROR_NO_DEFAULT_START; })
TEST(Simple3f, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ") == ERROR_NO_DEFAULT_START; })
TEST(Simple3g, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C  ") == ERROR_NO_DEFAULT_START; })

// Valid copies.
// Note that some of these leak memory - don't use in production...  "p" is
// frequently allocated, and never freed.
/*	TEST(Simple4a, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); return p != &that; })
TEST(Simple4b, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); return p->GetSpecifier() == 'f'; })
TEST(Simple4c, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); return !p->GetSkip(); })
TEST(Simple4d, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); that.SetSkip(); return !p->GetSkip(); })
TEST(Simple4e, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.SetSkip(); that.Clone(&p); return p->GetSkip(); })
TEST(Simple4f, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); return !p->GetOptional(); })
TEST(Simple4g, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); that.SetOptional(); return !p->GetOptional(); })
TEST(Simple4h, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.SetOptional(); that.Clone(&p); return p->GetOptional(); })
TEST(Simple4i, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); return dynamic_cast<SimpleSpecifier *>(p)->m_read == &Utils::ReadChar; })*/
// TEST(Simple4j, { SimpleSpecifier that('f', &Utils::ReadChar); return that.Skip(env) == 1; })

// Valid renders.
TEST(Simple5a, { SimpleSpecifier that('x', &Utils::ReadHex); ss s; return dynamic_cast<ss &>(s << that).str() == "x"; })

// Test chains (sort of).
TEST(Simple6a,
{
	SimpleSpecifier that('x', &Utils::ReadHex);
	cell
		d0,
		d1;
	char
		str[] = "0x543 \t 78AA";
	char const *
		s = str;
	return
		that.Run(s, DefaultEnvironment::Get(&d0)) == OK &&
		Utils::SkipWhitespaceOK(s) == OK &&
		that.Run(s, DefaultEnvironment::Get(&d1)) == OK &&
		d0 == 0x543 && d1 == 0x78AA;
})

TEST(Simple6b,
{
	SimpleSpecifier that('c', &Utils::ReadCharEx);
	cell
		d[2];
	char
		str[] = "'-'\r\n\\\\";
	char const *
		s = str;
	DefaultEnvironment &
		env = DefaultEnvironment::Get(d);
	return
		that.Run(s, env) == OK &&
		Utils::SkipWhitespaceOK(s) == OK &&
		that.Run(s, env) == OK &&
		d[0] == '-' && d[1] == '\\';
})

TEST(NumXr, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"1234", DefaultEnvironment::Get(&dest)) == OK && dest == 1234; })
TEST(NumXm, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"01234", DefaultEnvironment::Get(&dest)) == OK && dest == 668; })
TEST(NumXn, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"0b9876", DefaultEnvironment::Get(&dest)) == OK && dest == 0xB9876; })
TEST(NumXo, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"0x01234", DefaultEnvironment::Get(&dest)) == OK && dest == 0x1234; })
TEST(NumXp, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"0b1110", DefaultEnvironment::Get(&dest)) == OK && dest == 14; })
TEST(NumXq, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"0b0", DefaultEnvironment::Get(&dest)) == OK && dest == 0; })
TEST(NumXs, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"-56", DefaultEnvironment::Get(&dest)) == OK && dest == -56; })
TEST(NumXt, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"G", DefaultEnvironment::Get(&dest)) == ERROR_NAN; })
TEST(NumXu, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"-a", DefaultEnvironment::Get(&dest)) == ERROR_NAN; })


//TEST(Logical0, { cel})
TEST(LogicSpec00, { LogicalSpecifier that; return that.ReadToken(S"L(NULL)") == OK && *CUR == '\0'; })
TEST(LogicSpec01, { cell dest; LogicalSpecifier that; return that.ReadToken(S"L(NULL)") == OK && that.Run(S"", DefaultEnvironment::Get(&dest)) == OK && dest == 0; })
TEST(LogicSpec02, { cell dest; LogicalSpecifier that; return that.ReadToken(S"L(0)") == OK && that.Run(S"", DefaultEnvironment::Get(&dest)) == OK && dest == 0; })
TEST(LogicSpec03, { cell dest; LogicalSpecifier that; return that.ReadToken(S"L(10101)") == OK && that.Run(S"", DefaultEnvironment::Get(&dest)) == OK && dest == 21; })
TEST(LogicSpec04, { cell dest; LogicalSpecifier that; return that.ReadToken(S"L(truE)") == OK && that.Run(S"", DefaultEnvironment::Get(&dest)) == OK && dest == 1; })

TEST(LogicSpec05, { cell dest; LogicalSpecifier that; return that.ReadToken(S"L(NULL)") == OK && that.Run(S"10101", DefaultEnvironment::Get(&dest)) == OK && dest == 1; })
TEST(LogicSpec06, { cell dest; LogicalSpecifier that; return that.ReadToken(S"L(0)") == OK && that.Run(S"1", DefaultEnvironment::Get(&dest)) == OK && dest == 1; })
TEST(LogicSpec07, { cell dest; LogicalSpecifier that; return that.ReadToken(S"L(10101)") == OK && that.Run(S"faLSE", DefaultEnvironment::Get(&dest)) == OK && dest == 0; })
TEST(LogicSpec08, { cell dest; LogicalSpecifier that; return that.ReadToken(S"L(truE)") == OK && that.Run(S"0", DefaultEnvironment::Get(&dest)) == OK && dest == 0; })



