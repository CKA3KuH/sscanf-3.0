#pragma once

#include "..\specifiers.h"

#ifdef SSCANF_DEBUG
	#include "..\utils.h"
#endif

class SimpleSpecifier : public Specifier
{
public:
	// cons
		SimpleSpecifier(char c, ReadFunction_t f)
	:
		Specifier(c),
		m_default(0),
		m_read(f)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input)
	{
		// Check this has the correct specifier.
		FAIL((*input | 0x20) == GetSpecifier(), ERROR_EXPECTED_A_GOT_B, GetSpecifier(), *input);
		// Check if this is upper-case (optional).
		if (*input++ != GetSpecifier())
		{
			SetOptional();
			// Capital letter - read in the deafult.
			// Skip the opening bracket.
			NEXT(input, '(', ERROR_NO_DEAFULT_START);
			TRY(Run(input, DefaultEnvironment::Get(&m_default)));
			// Skip the closing bracket.
			NEXT(input, ')', ERROR_NO_DEAFULT_END);
		}
		return OK;
	};
	
	CLONE();
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		cell
			dest;
		TRY((*m_read)(input, dest));
		return env.SetNextValue(dest);
		//return env.SkipDelimiters();
	};
	
	// cons
		SimpleSpecifier(SimpleSpecifier const & that)
	:
		Specifier(that),
		m_default(that.m_default),
		m_read(that.m_read)
	{
	};
	
private:
	cell
		m_default;
	
	ReadFunction_t
		m_read;
	
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
	CTEST(Simple9r, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum);  return that.Run(S"1234", DefaultEnvironment::Get(&dest)) == OK && dest == 1234; })
	CTEST(Simple9m, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum);  return that.Run(S"01234", DefaultEnvironment::Get(&dest)) == OK && dest == 668; })
	CTEST(Simple9n, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum);  return that.Run(S"0b9876", DefaultEnvironment::Get(&dest)) == OK && dest == 0xB9876; })
	CTEST(Simple9o, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum);  return that.Run(S"0x01234", DefaultEnvironment::Get(&dest)) == OK && dest == 0x1234; })
	CTEST(Simple9p, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum);  return that.Run(S"0b1110", DefaultEnvironment::Get(&dest)) == OK && dest == 14; })
	CTEST(Simple9q, { cell dest; SimpleSpecifier that('n', &Utils::ReadNum);  return that.Run(S"0b0", DefaultEnvironment::Get(&dest)) == OK && dest == 0; })
	
	// Valid formats.
	CTEST(Simple1a, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"i") == OK; })
	CTEST(Simple1b, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"d") == ERROR_EXPECTED_A_GOT_B; })
	CTEST(Simple1v, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"") == ERROR_EXPECTED_A_GOT_B; })
	CTEST(Simple1w, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"(") == ERROR_EXPECTED_A_GOT_B; })
	CTEST(Simple1c, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I") == ERROR_NO_DEAFULT_START; })
	
	CTEST(Simple1d, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(55)") == OK && *CUR == '\0'; })
	CTEST(Simple1e, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55)") == OK && *CUR == '\0'; })
	CTEST(Simple1f, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (  55  )") == OK && *CUR == '\0'; })
	CTEST(Simple1g, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I (55  )") == OK && *CUR == '\0'; })
	CTEST(Simple1h, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(  55)  ") == OK && *CUR == '\0'; })
	CTEST(Simple1i, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( )") != OK; })
	CTEST(Simple1j, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I ( )  ") != OK; })
	CTEST(Simple1k, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(3") == ERROR_NO_DEAFULT_END; })
	CTEST(Simple1l, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I( 4") == ERROR_NO_DEAFULT_END; })
	CTEST(Simple1m, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I   (  5") == ERROR_NO_DEAFULT_END; })
	CTEST(Simple1n, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(    7") == ERROR_NO_DEAFULT_END; })
	CTEST(Simple1o, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(") == ERROR_NAN; })
	CTEST(Simple1p, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(E)") == ERROR_NAN; })
	CTEST(Simple1q, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(    -6)") == OK && that.m_default == -6; })
	CTEST(Simple1r, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(78)") == OK && that.m_default == 78; })
	CTEST(Simple1s, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(1234)") == OK && that.m_default == 1234; })
	CTEST(Simple1t, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(595)") == OK && that.m_default == 595; })
	CTEST(Simple1u, { SimpleSpecifier that('i', &Utils::ReadDecimal); return that.ReadToken(S"I(0x11)") == ERROR_NO_DEAFULT_END; })
	
	CTEST(Simple2a, { SimpleSpecifier that('d', &Utils::ReadDecimal); return that.ReadToken(S"d") == OK; })
	CTEST(Simple2b, { SimpleSpecifier that('d', &Utils::ReadDecimal); return that.ReadToken(S"i") == ERROR_EXPECTED_A_GOT_B; })
	CTEST(Simple2c, { SimpleSpecifier that('d', &Utils::ReadDecimal); return that.ReadToken(S"D") == ERROR_NO_DEAFULT_START; })
	
	CTEST(Simple3a, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"c") == OK; })
	CTEST(Simple3b, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"v") == ERROR_EXPECTED_A_GOT_B; })
	CTEST(Simple3c, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C") == ERROR_NO_DEAFULT_START; })
	CTEST(Simple3f, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C ") == ERROR_NO_DEAFULT_START; })
	CTEST(Simple3g, { SimpleSpecifier that('c', &Utils::ReadCharEx); return that.ReadToken(S"C  ") == ERROR_NO_DEAFULT_START; })
	
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
	CTEST(Simple4j, { SimpleSpecifier that('f', &Utils::ReadChar); return that.GetMemoryUsage() == 1; })
	
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
};

ITEST(SimpleSpecifier, Simple1a)
ITEST(SimpleSpecifier, Simple1b)
ITEST(SimpleSpecifier, Simple1c)
ITEST(SimpleSpecifier, Simple1d)
ITEST(SimpleSpecifier, Simple1e)
ITEST(SimpleSpecifier, Simple1f)
ITEST(SimpleSpecifier, Simple1g)
ITEST(SimpleSpecifier, Simple1h)
ITEST(SimpleSpecifier, Simple1i)
ITEST(SimpleSpecifier, Simple1j)
ITEST(SimpleSpecifier, Simple1k)
ITEST(SimpleSpecifier, Simple1l)
ITEST(SimpleSpecifier, Simple1m)
ITEST(SimpleSpecifier, Simple1n)
ITEST(SimpleSpecifier, Simple1o)
ITEST(SimpleSpecifier, Simple1p)
ITEST(SimpleSpecifier, Simple1q)
ITEST(SimpleSpecifier, Simple1r)
ITEST(SimpleSpecifier, Simple1s)
ITEST(SimpleSpecifier, Simple1t)
ITEST(SimpleSpecifier, Simple1u)
ITEST(SimpleSpecifier, Simple1v)
ITEST(SimpleSpecifier, Simple1w)

ITEST(SimpleSpecifier, Simple2a)
ITEST(SimpleSpecifier, Simple2b)
ITEST(SimpleSpecifier, Simple2c)

ITEST(SimpleSpecifier, Simple3a)
ITEST(SimpleSpecifier, Simple3b)
ITEST(SimpleSpecifier, Simple3c)
ITEST(SimpleSpecifier, Simple3f)
ITEST(SimpleSpecifier, Simple3g)

ITEST(SimpleSpecifier, Simple4a)
ITEST(SimpleSpecifier, Simple4b)
ITEST(SimpleSpecifier, Simple4c)
ITEST(SimpleSpecifier, Simple4d)
ITEST(SimpleSpecifier, Simple4e)
ITEST(SimpleSpecifier, Simple4f)
ITEST(SimpleSpecifier, Simple4g)
ITEST(SimpleSpecifier, Simple4h)
ITEST(SimpleSpecifier, Simple4i)
ITEST(SimpleSpecifier, Simple4j)

ITEST(SimpleSpecifier, Simple5a)

ITEST(SimpleSpecifier, Simple9a)
ITEST(SimpleSpecifier, Simple9b)
ITEST(SimpleSpecifier, Simple9c)
ITEST(SimpleSpecifier, Simple9h)
ITEST(SimpleSpecifier, Simple9d)
ITEST(SimpleSpecifier, Simple9e)
ITEST(SimpleSpecifier, Simple9i)
ITEST(SimpleSpecifier, Simple9l)
ITEST(SimpleSpecifier, Simple9f)
ITEST(SimpleSpecifier, Simple9j)
ITEST(SimpleSpecifier, Simple9g)
ITEST(SimpleSpecifier, Simple9k)
ITEST(SimpleSpecifier, Simple9m)
ITEST(SimpleSpecifier, Simple9n)
ITEST(SimpleSpecifier, Simple9o)
ITEST(SimpleSpecifier, Simple9p)
ITEST(SimpleSpecifier, Simple9q)
ITEST(SimpleSpecifier, Simple9r)

ITEST(SimpleSpecifier, Simple6a)
ITEST(SimpleSpecifier, Simple6b)

