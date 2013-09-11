#pragma once

#include "../specifiers.h"

#ifdef SSCANF_DEBUG
	#include "../utils.h"
#endif

typedef
	error_t (* ReadFunction_t)(char const * &, cell &);

class SimpleSpecifier : public Specifier
{
public:
	// cons
		SimpleSpecifier(char c, ReadFunction_t f)
	:
		Specifier(c),
		m_default(0),
		m_read(f),
		m_lower(0x80000000),
		m_upper(0x7FFFFFFF)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input);
	
	CLONE();
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
	// cons
		SimpleSpecifier(SimpleSpecifier const & that)
	:
		Specifier(that),
		m_default(that.m_default),
		m_read(that.m_read),
		m_lower(that.m_lower),
		m_upper(that.m_upper)
	{
	};
	
	virtual cell
		Skip(Environment & env)
	{
		env.Skip(1);
		return 1;
	};
	
private:
	cell
		m_lower,
		m_upper,
		m_default;
	
	ReadFunction_t
		m_read;
	
	CTEST(SimpleP00, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<-5 - -1>") == OK; })
	CTEST(SimpleP01, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<   - -1>") == OK; })
	CTEST(SimpleP02, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<-5 -   >") == OK; })
	CTEST(SimpleP03, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i< 5 - -1>") == ERROR_INVALID_RANGE; })
	CTEST(SimpleP04, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<-5 -  1>") == OK; })
	CTEST(SimpleP05, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i< 6 - 2 >") == ERROR_INVALID_RANGE; })
	CTEST(SimpleP06, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i< 2 -6  >") == OK; })
	CTEST(SimpleP07, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<5 1>") == ERROR_INVALID_RANGE; })
	CTEST(SimpleP08, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<>") == ERROR_INVALID_RANGE; })
	CTEST(SimpleP09, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i<---->") == ERROR_INVALID_RANGE; })
	
	CTEST(Simple9a, { cell dest; SimpleSpecifier that('i', &Utils::ReadDecimal); return that.Run(S"44", DefaultEnvironment::Get(&dest)) == OK && dest == 44; })
	CTEST(Simple9b, { cell dest; SimpleSpecifier that('d', &Utils::ReadDecimal); return that.Run(S"00001", DefaultEnvironment::Get(&dest)) == OK && dest == 1; })
	CTEST(Simple9c, { cell dest; SimpleSpecifier that('c', &Utils::ReadCharEx);  return that.Run(S"'t'", DefaultEnvironment::Get(&dest)) == OK && dest == 't'; })
	CTEST(Simple9h, { cell dest; SimpleSpecifier that('c', &Utils::ReadCharEx);  return that.Run(S"u", DefaultEnvironment::Get(&dest)) == OK && dest == 'u'; })
	CTEST(Simple9d, { cell dest; SimpleSpecifier that('h', &Utils::ReadHex);     return that.Run(S"123", DefaultEnvironment::Get(&dest)) == OK && dest == 0x123; })
	CTEST(Simple9e, { cell dest; SimpleSpecifier that('x', &Utils::ReadHex);     return that.Run(S"0x123", DefaultEnvironment::Get(&dest)) == OK && dest == 0x123; })
	CTEST(Simple9i, { cell dest; SimpleSpecifier that('h', &Utils::ReadHex);     return that.Run(S"FF", DefaultEnvironment::Get(&dest)) == OK && dest == 0xFF; })
	CTEST(Simple9l, { cell dest; SimpleSpecifier that('x', &Utils::ReadHex);     return that.Run(S"0xAD", DefaultEnvironment::Get(&dest)) == OK && dest == 0xAD; })
	CTEST(Simple9f, { cell dest; SimpleSpecifier that('o', &Utils::ReadOctal);   return that.Run(S"76", DefaultEnvironment::Get(&dest)) == OK && dest == 076; })
	CTEST(Simple9j, { cell dest; SimpleSpecifier that('o', &Utils::ReadOctal);   return that.Run(S"054", DefaultEnvironment::Get(&dest)) == OK && dest == 054; })
	CTEST(Simple9g, { cell dest; SimpleSpecifier that('b', &Utils::ReadBinary);  return that.Run(S"010101", DefaultEnvironment::Get(&dest)) == OK && dest == 21; })
	CTEST(Simple9k, { cell dest; SimpleSpecifier that('b', &Utils::ReadBinary);  return that.Run(S"0b101010", DefaultEnvironment::Get(&dest)) == OK && dest == 42; })
	
	// Valid formats.
	CTEST(Simple1a, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i") == OK; })
	CTEST(Simple1b, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"d") == ERROR_EXPECTED_A_GOT_B_2; })
	CTEST(Simple1v, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"") == ERROR_EXPECTED_A_GOT_B_2; })
	CTEST(Simple1w, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"(") == ERROR_EXPECTED_A_GOT_B_2; })
	CTEST(Simple1c, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I") == ERROR_NO_DEFAULT_START; })
	
	// These don't fail.
	CTEST(SimpleH04, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H(55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleH05, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H (55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleH06, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H(55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH07, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H (55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH12, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H( 55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleH13, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H ( 55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleH14, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H( 55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH15, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H ( 55 ) ") == OK && *CUR == '\0'; })
	// These DO for some reason...  They all DON'T have a space before the close
	// bracket :/.
	CTEST(SimpleH00, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H(55)") == OK && *CUR == '\0'; })
	CTEST(SimpleH01, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H (55)") == OK && *CUR == '\0'; })
	CTEST(SimpleH02, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H(55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH03, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H (55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH08, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H( 55)") == OK && *CUR == '\0'; })
	CTEST(SimpleH09, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H ( 55)") == OK && *CUR == '\0'; })
	CTEST(SimpleH10, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H( 55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH11, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S"H ( 55) ") == OK && *CUR == '\0'; })
	
	CTEST(SimpleI00, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(55)") == OK && *CUR == '\0'; })
	CTEST(SimpleI01, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55)") == OK && *CUR == '\0'; })
	CTEST(SimpleI02, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleI03, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleI04, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleI05, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleI06, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleI07, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleI08, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( 55)") == OK && *CUR == '\0'; })
	CTEST(SimpleI09, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I ( 55)") == OK && *CUR == '\0'; })
	CTEST(SimpleI10, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( 55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleI11, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I ( 55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleI12, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( 55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleI13, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I ( 55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleI14, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( 55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleI15, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I ( 55 ) ") == OK && *CUR == '\0'; })
	
	CTEST(SimpleO00, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O(55)") == OK && *CUR == '\0'; })
	CTEST(SimpleO01, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O (55)") == OK && *CUR == '\0'; })
	CTEST(SimpleO02, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O(55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleO03, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O (55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleO04, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O(55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleO05, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O (55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleO06, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O(55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleO07, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O (55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleO08, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O( 55)") == OK && *CUR == '\0'; })
	CTEST(SimpleO09, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O ( 55)") == OK && *CUR == '\0'; })
	CTEST(SimpleO10, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O( 55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleO11, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O ( 55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleO12, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O( 55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleO13, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O ( 55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleO14, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O( 55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleO15, { SimpleSpecifier that('o', &Utils::ReadOctal); return that.ReadToken(S"O ( 55 ) ") == OK && *CUR == '\0'; })
	
	CTEST(SimpleB00, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B(101010)") == OK && *CUR == '\0'; })
	CTEST(SimpleB01, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B (101010)") == OK && *CUR == '\0'; })
	CTEST(SimpleB02, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B(101010) ") == OK && *CUR == '\0'; })
	CTEST(SimpleB03, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B (101010) ") == OK && *CUR == '\0'; })
	CTEST(SimpleB04, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B(101010 )") == OK && *CUR == '\0'; })
	CTEST(SimpleB05, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B (101010 )") == OK && *CUR == '\0'; })
	CTEST(SimpleB06, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B(101010 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleB07, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B (101010 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleB08, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B( 101010)") == OK && *CUR == '\0'; })
	CTEST(SimpleB09, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B ( 101010)") == OK && *CUR == '\0'; })
	CTEST(SimpleB10, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B( 101010) ") == OK && *CUR == '\0'; })
	CTEST(SimpleB11, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B ( 101010) ") == OK && *CUR == '\0'; })
	CTEST(SimpleB12, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B( 101010 )") == OK && *CUR == '\0'; })
	CTEST(SimpleB13, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B ( 101010 )") == OK && *CUR == '\0'; })
	CTEST(SimpleB14, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B( 101010 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleB15, { SimpleSpecifier that('b', &Utils::ReadBinary); return that.ReadToken(S"B ( 101010 ) ") == OK && *CUR == '\0'; })
	
	CTEST(SimpleC00, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C(@)") == OK && *CUR == '\0'; })
	CTEST(SimpleC01, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C (@)") == OK && *CUR == '\0'; })
	CTEST(SimpleC02, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C(@) ") == OK && *CUR == '\0'; })
	CTEST(SimpleC03, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C (@) ") == OK && *CUR == '\0'; })
	CTEST(SimpleC04, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C(@ )") == OK && *CUR == '\0'; })
	CTEST(SimpleC05, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C (@ )") == OK && *CUR == '\0'; })
	CTEST(SimpleC06, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C(@ ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleC07, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C (@ ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleC08, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( @)") == OK && *CUR == '\0'; })
	CTEST(SimpleC09, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( @)") == OK && *CUR == '\0'; })
	CTEST(SimpleC10, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( @) ") == OK && *CUR == '\0'; })
	CTEST(SimpleC11, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( @) ") == OK && *CUR == '\0'; })
	CTEST(SimpleC12, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( @ )") == OK && *CUR == '\0'; })
	CTEST(SimpleC13, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( @ )") == OK && *CUR == '\0'; })
	CTEST(SimpleC14, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( @ ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleC15, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( @ ) ") == OK && *CUR == '\0'; })
	
	CTEST(Simplec00, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C('@')") == OK && *CUR == '\0'; })
	CTEST(Simplec01, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ('@')") == OK && *CUR == '\0'; })
	CTEST(Simplec02, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C('@') ") == OK && *CUR == '\0'; })
	CTEST(Simplec03, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ('@') ") == OK && *CUR == '\0'; })
	CTEST(Simplec04, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C('@' )") == OK && *CUR == '\0'; })
	CTEST(Simplec05, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ('@' )") == OK && *CUR == '\0'; })
	CTEST(Simplec06, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C('@' ) ") == OK && *CUR == '\0'; })
	CTEST(Simplec07, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ('@' ) ") == OK && *CUR == '\0'; })
	CTEST(Simplec08, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( '@')") == OK && *CUR == '\0'; })
	CTEST(Simplec09, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( '@')") == OK && *CUR == '\0'; })
	CTEST(Simplec10, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( '@') ") == OK && *CUR == '\0'; })
	CTEST(Simplec11, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( '@') ") == OK && *CUR == '\0'; })
	CTEST(Simplec12, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( '@' )") == OK && *CUR == '\0'; })
	CTEST(Simplec13, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( '@' )") == OK && *CUR == '\0'; })
	CTEST(Simplec14, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C( '@' ) ") == OK && *CUR == '\0'; })
	CTEST(Simplec15, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ( '@' ) ") == OK && *CUR == '\0'; })
	// These all fail for the same reason - leading spaces...
	/*CTEST(SimpleH16, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H(55)") == OK && *CUR == '\0'; })
	CTEST(SimpleH17, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H (55)") == OK && *CUR == '\0'; })
	CTEST(SimpleH18, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H(55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH19, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H (55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH20, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H(55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleH21, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H (55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleH22, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H(55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH23, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H (55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH24, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H( 55)") == OK && *CUR == '\0'; })
	CTEST(SimpleH25, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H ( 55)") == OK && *CUR == '\0'; })
	CTEST(SimpleH26, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H( 55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH27, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H ( 55) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH28, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H( 55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleH29, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H ( 55 )") == OK && *CUR == '\0'; })
	CTEST(SimpleH30, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H( 55 ) ") == OK && *CUR == '\0'; })
	CTEST(SimpleH31, { SimpleSpecifier that('h', &Utils::ReadHex); return that.ReadToken(S" H ( 55 ) ") == OK && *CUR == '\0'; })*/
	
	CTEST(Simple1d, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(55)") == OK && *CUR == '\0'; })
	CTEST(Simple1e, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55)") == OK && *CUR == '\0'; })
	CTEST(Simple1f, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (  55  )") == OK && *CUR == '\0'; })
	CTEST(Simple1g, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55  )") == OK && *CUR == '\0'; })
	CTEST(Simple1h, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(  55)  ") == OK && *CUR == '\0'; })
	CTEST(Simple1i, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( )") != OK; })
	CTEST(Simple1j, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I ( )  ") != OK; })
	CTEST(Simple1k, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(3") == ERROR_NO_DEFAULT_END; })
	CTEST(Simple1l, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( 4") == ERROR_NO_DEFAULT_END; })
	CTEST(Simple1m, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I   (  5") == ERROR_NO_DEFAULT_END; })
	CTEST(Simple1n, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(    7") == ERROR_NO_DEFAULT_END; })
	CTEST(Simple1o, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(") == ERROR_NO_DEFAULT_END; })
	CTEST(Simple1p, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(E)") == ERROR_NAN; })
	CTEST(Simple1q, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(    -6)") == OK && that.m_default == -6; })
	CTEST(Simple1r, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(78)") == OK && that.m_default == 78; })
	CTEST(Simple1s, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(1234)") == OK && that.m_default == 1234; })
	CTEST(Simple1t, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(595)") == OK && that.m_default == 595; })
	CTEST(Simple1u, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(0x11)") == ERROR_INVALID_DEFAULT; })
	
	CTEST(Simple2a, { SimpleSpecifier that('d', &Utils::ReadDecimal); return that.ReadToken(S"d") == OK; })
	CTEST(Simple2b, { SimpleSpecifier that('d', &Utils::ReadDecimal); return that.ReadToken(S"i") == ERROR_EXPECTED_A_GOT_B_2; })
	CTEST(Simple2c, { SimpleSpecifier that('d', &Utils::ReadDecimal); return that.ReadToken(S"D") == ERROR_NO_DEFAULT_START; })
	
	CTEST(Simple3a, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"c") == OK; })
	CTEST(Simple3b, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"v") == ERROR_EXPECTED_A_GOT_B_2; })
	CTEST(Simple3c, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C") == ERROR_NO_DEFAULT_START; })
	CTEST(Simple3f, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ") == ERROR_NO_DEFAULT_START; })
	CTEST(Simple3g, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C  ") == ERROR_NO_DEFAULT_START; })
	
	// Valid copies.
	// Note that some of these leak memory - don't use in production...  "p" is
	// frequently allocated, and never freed.
	CTEST(Simple4a, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); return p != &that; })
	CTEST(Simple4b, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); return p->GetSpecifier() == 'f'; })
	CTEST(Simple4c, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); return !p->GetSkip(); })
	CTEST(Simple4d, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); that.SetSkip(); return !p->GetSkip(); })
	CTEST(Simple4e, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.SetSkip(); that.Clone(&p); return p->GetSkip(); })
	CTEST(Simple4f, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); return !p->GetOptional(); })
	CTEST(Simple4g, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); that.SetOptional(); return !p->GetOptional(); })
	CTEST(Simple4h, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.SetOptional(); that.Clone(&p); return p->GetOptional(); })
	CTEST(Simple4i, { SimpleSpecifier that('f', &Utils::ReadChar); Specifier * p; that.Clone(&p); return dynamic_cast<SimpleSpecifier *>(p)->m_read == &Utils::ReadChar; })
	// CTEST(Simple4j, { SimpleSpecifier that('f', &Utils::ReadChar); return that.Skip(env) == 1; })
	
	// Valid renders.
	CTEST(Simple5a, { SimpleSpecifier that('x', &Utils::ReadHex); ss s; return dynamic_cast<ss &>(s << that).str() == "x"; })
	
	// Test chains (sort of).
	CTEST(Simple6a,
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
	
	CTEST(Simple6b,
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
	
	CTEST(NumXr, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"1234", DefaultEnvironment::Get(&dest)) == OK && dest == 1234; })
	CTEST(NumXm, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"01234", DefaultEnvironment::Get(&dest)) == OK && dest == 668; })
	CTEST(NumXn, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"0b9876", DefaultEnvironment::Get(&dest)) == OK && dest == 0xB9876; })
	CTEST(NumXo, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"0x01234", DefaultEnvironment::Get(&dest)) == OK && dest == 0x1234; })
	CTEST(NumXp, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"0b1110", DefaultEnvironment::Get(&dest)) == OK && dest == 14; })
	CTEST(NumXq, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"0b0", DefaultEnvironment::Get(&dest)) == OK && dest == 0; })
	CTEST(NumXs, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"-56", DefaultEnvironment::Get(&dest)) == OK && dest == -56; })
	CTEST(NumXt, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"G", DefaultEnvironment::Get(&dest)) == ERROR_NAN; })
	CTEST(NumXu, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum); return that.Run(S"-a", DefaultEnvironment::Get(&dest)) == ERROR_NAN; })
};

