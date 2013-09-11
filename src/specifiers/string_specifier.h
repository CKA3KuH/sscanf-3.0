#pragma once

#include "../specifiers.h"

#ifdef SSCANF_DEBUG
	bool TestCompare(char const * one, cell const * two);
#endif

class StringSpecifier : public Specifier
{
public:
	// cons
		StringSpecifier()
	:
		Specifier('s'),
		m_ssize(0),
		m_length(0),
		m_default(nullptr),
		m_last(false)
	{
	};
	
	// cons
		StringSpecifier(StringSpecifier const & that)
	:
		Specifier('s'),
		m_ssize(that.m_ssize),
		m_length(that.m_length),
		m_default(that.m_default),
		m_last(that.m_last)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input)
	{
		// Check this has the correct specifier.
		FAIL((*input | 0x20) == GetSpecifier(), ERROR_EXPECTED_A_GOT_B_2, GetSpecifier(), *input);
		// Check if this is upper-case (optional).
		if (*input++ != GetSpecifier())
		{
			char const *
				str;
			TRY(Utils::GetDefaults(input, str, &m_length));
			m_default = new cell [m_length + 1];
			Utils::CopyString(m_default, str, m_length + 1);
			SetOptional();
		}
		TRY(Utils::GetLength(input, &m_ssize));
		// If the last specifier is a string, it is handled specially.  Note
		// that the old definitions allowed "S(hi) " to override this behaviour,
		// but the new system is coded to frequently skip spaces.  As a result
		// we must look backwards to preserve the old operation.  Look for the
		// NULL that we replaced the close square bracket with.
		m_last = *input == '\0' && *(input - 1) == '\0';
		return OK;
	};
	
	CLONE();
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		cell
			dump;
		size_t
			size = 0;
		if (m_ssize < 0)
		{
			// Get it from "env".
			TRY(env.GetNextValue(&dump));
			FAIL(dump > 0, ERROR_INVALID_ARRAY_SIZE);
			size = dump;
		}
		else if (m_ssize > 0) size = m_ssize;
		else FAIL(env.GetMemory()->GetZeroLengthValid(), ERROR_INVALID_ARRAY_SIZE);
		if (size)
		{
			// Reserve one character for the "NULL" terminator always.
			--size;
			size_t
				idx = 0;
			if (*input == '\0')
			{
				FAIL(GetOptional(), ERROR_NO_STRING_LITERAL);
				// This would be less complex if it wasn't so backwards
				// compatible.  The old version REQUIRED sizes even on quiet
				// strings.  This new version would prefer none to be given.
				if (size >= m_length)
				{
					// Greater than, or exactly enough, space available.
					for ( ; idx <= m_length; ++idx) TRY(env.SetNextValue(m_default[idx], idx));
					TRY(env.Skip(0, size - m_length));
				}
				else
				{
					// Not enough space.
					for ( ; idx < size; ++idx) TRY(env.SetNextValue(m_default[idx], idx));
					TRY(env.SetNextValue('\0', size));
					logprintf("sscanf warning: String buffer overflow. at %s:%d.", __FILE__, __LINE__);
				}
				return OK;
			}
			if (m_last)
			{
				size_t
					lnws = 0; // Last Not White Space.
				while (idx < size && !env.AtDelimiter(input))
				{
					// Don't use "dump" as "\ " is valid whitespace.
					if (*input > ' ') lnws = idx + 1;
					TRY(Utils::ReadChar(input, dump));
					TRY(env.SetNextValue(dump, idx++));
				}
				// No valid string actually read!
				FAIL(lnws != 0, ERROR_NO_STRING_LITERAL);
				// Pad the remainder of any array memory.  End this string at
				// the last space found, not the VERY end.
				TRY(env.SetNextValue('\0', lnws));
				// Currently only "enum" uses this.  But it tells the memory
				// system how big the array SHOULD be, if that's ever useful.
				if (lnws != idx) --idx;
				TRY(env.Skip(0, size - idx));
				if (env.AtDelimiter(input)) return OK;
			}
			else
			{
				while (idx < size && *input > ' ')
				{
					TRY(Utils::ReadChar(input, dump));
					TRY(env.SetNextValue(dump, idx++));
				}
				// Both "idx" and "size" are OBO at this point, so cancel out.
				TRY(env.SetNextValue('\0', idx));
				TRY(env.Skip(0, size - idx));
				if (*input <= ' ') return OK;
			}
			// This is not a fatal error - the remainder is being dumped.
			logprintf("sscanf warning: String buffer overflow. at %s:%d.", __FILE__, __LINE__);
		}
		else if (*input == '\0')
		{
			// Don't even bother saving the non-existent data to nowhere.
			FAIL(GetOptional(), ERROR_NO_STRING_LITERAL);
			return OK;
		}
		// Loop to an explicit delimiter (NULL counts).  Use "ReadChar" so that
		// we can still do "\ " to include spaces when we shouldn't otherwise.
		if (m_last) while (!env.AtDelimiter(input)) TRY(Utils::ReadChar(input, dump));
		else while (*input > ' ') TRY(Utils::ReadChar(input, dump));
		// Discard the result.
		return OK;
	};
	
	virtual cell
		Skip(Environment & env)
	{
		env.Skip(1);
		return 1;
	};
	
private:
	bool
		m_last;
	
	int
		m_ssize;
	
	size_t
		m_length;
	
	cell *
		m_default;
	
	CTEST(StringSpec0,  { StringSpecifier sts; return sts.ReadToken(S"") == ERROR_EXPECTED_A_GOT_B_2; })
	CTEST(StringSpec1,  { StringSpecifier sts; return sts.ReadToken(S"q") == ERROR_EXPECTED_A_GOT_B_2; })
	CTEST(StringSpec2,  { StringSpecifier sts; return sts.ReadToken(S"s") == ERROR_NO_ARRAY_START; })
	CTEST(StringSpec3,  { StringSpecifier sts; return sts.ReadToken(S"s]") == ERROR_NO_ARRAY_START; })
	CTEST(StringSpec4,  { StringSpecifier sts; return sts.ReadToken(S"s[") == ERROR_NO_ARRAY_END; })
	CTEST(StringSpec5,  { StringSpecifier sts; return sts.ReadToken(S"s[]") == OK && sts.m_ssize == 0; })
	CTEST(StringSpec6,  { StringSpecifier sts; return sts.ReadToken(S"s[1]") == OK && sts.m_ssize == 1; })
	CTEST(StringSpec7,  { StringSpecifier sts; return sts.ReadToken(S"s[10]") == OK && sts.m_ssize == 10; })
	CTEST(StringSpec8,  { StringSpecifier sts; return sts.ReadToken(S"s[10]") == OK && sts.m_last == true; })
	CTEST(StringSpec9,  { StringSpecifier sts; return sts.ReadToken(S"s[10] ") == OK && sts.m_last == false; })
	CTEST(StringSpec10, { StringSpecifier sts; return sts.ReadToken(S"s[-] ") == ERROR_NAN; })
	CTEST(StringSpec11, { StringSpecifier sts; return sts.ReadToken(S"s[e] ") == ERROR_NAN; })
	CTEST(StringSpec12, { StringSpecifier sts; return sts.ReadToken(S"s[1k] ") == ERROR_NAN; })
	CTEST(StringSpec13, { StringSpecifier sts; return sts.ReadToken(S"s[0] ") == ERROR_INVALID_ARRAY_SIZE; })
	CTEST(StringSpec14, { StringSpecifier sts; return sts.ReadToken(S"s[-84] ") == ERROR_INVALID_ARRAY_SIZE; })
	CTEST(StringSpec15, { StringSpecifier sts; return sts.ReadToken(S"s[*]") == OK && sts.m_ssize == -1; })
	CTEST(StringSpec16, { StringSpecifier sts; return sts.ReadToken(S"s [ ] ") == OK; })
	CTEST(StringSpec17, { StringSpecifier sts; return sts.ReadToken(S"S[1]") == ERROR_NO_DEFAULT_START; })
	CTEST(StringSpec18, { StringSpecifier sts; return sts.ReadToken(S"S([1]") == ERROR_NO_DEFAULT_END; })
	CTEST(StringSpec19, { StringSpecifier sts; return sts.ReadToken(S"S([1])") == ERROR_NO_ARRAY_START; })
	CTEST(StringSpec20, { StringSpecifier sts; return sts.ReadToken(S"S(\\(d)[3]") == OK; })
	CTEST(StringSpec21, { StringSpecifier sts; return sts.ReadToken(S"S(())[4]") == OK; })
	
	
	CTEST(StringSpec22, { StringSpecifier sts; return sts.ReadToken(S"S(  hello there  )[7]") == OK && sts.m_ssize == 7 && sts.m_length == 11 && TestCompare("hello there", sts.m_default); })
	CTEST(StringSpec23, { StringSpecifier sts; return sts.ReadToken(S"S(  hello \\)  )[8]") == OK && sts.m_ssize == 8 && sts.m_length == 7 && TestCompare("hello )", sts.m_default); })
	CTEST(StringSpec24, { StringSpecifier sts; return sts.ReadToken(S"S(  \\\"  )[9]") == OK && sts.m_ssize == 9 && sts.m_length == 1 && TestCompare("\"", sts.m_default); })
	
	CTEST(StringSpec25, { StringSpecifier sts; return sts.ReadToken(S"s[4]") == OK && sts.m_default == nullptr && sts.m_length == 0; })
	
	
	CTEST(StringSRun00, { StringSpecifier sts; sts.ReadToken(S"s[20]");
		cell dest[20];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hello", e) == OK;
	})
	CTEST(StringSRun01, { StringSpecifier sts; sts.ReadToken(S"s[20]");
		cell dest[20];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hello", e) == OK && TestCompare("hello", dest);
	})
	CTEST(StringSRun02, { StringSpecifier sts; sts.ReadToken(S"s[20]");
		cell dest[20];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hello there", e) == OK && TestCompare("hello there", dest);
	})
	CTEST(StringSRun03, { StringSpecifier sts; sts.ReadToken(S"s[20] ");
		cell dest[20];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hello there", e) == OK && TestCompare("hello", dest);
	})
	CTEST(StringSRun04, { StringSpecifier sts; sts.ReadToken(S"s[20]");
		cell dest[20];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hel\\(lo there  ", e) == OK && TestCompare("hel(lo there", dest);
	})
	CTEST(StringSRun05, { StringSpecifier sts; sts.ReadToken(S"s[20] ");
		cell dest[20];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hello\\ there", e) == OK && TestCompare("hello there", dest);
	})
	CTEST(StringSRun06, { StringSpecifier sts; sts.ReadToken(S"s[10]");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hello there   ", e) == OK && TestCompare("hello the", dest);
	})
	CTEST(StringSRun07, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
	})
	
	
	
	CTEST(StringSRun08, { StringSpecifier sts; sts.ReadToken(S"s[*] ");
		cell dest[11] = {10};
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest + 1);
	})
	CTEST(StringSRun09, { StringSpecifier sts; sts.ReadToken(S"s[*]");
		cell dest[11] = {10};
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hello there   ", e) == OK && TestCompare("hello the", dest + 1);
	})
	CTEST(StringSRun10, { StringSpecifier sts; sts.ReadToken(S"s[*] ");
		cell dest[11] = {5};
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hell", dest + 1);
	})
	CTEST(StringSRun11, { StringSpecifier sts; sts.ReadToken(S"s[] ");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == ERROR_INVALID_ARRAY_SIZE;
	})
	CTEST(StringSRun12, { StringSpecifier sts; sts.ReadToken(S"s[*] ");
		cell dest[11] = {-5};
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == ERROR_INVALID_ARRAY_SIZE;
	})
	CTEST(StringSRun13, { StringSpecifier sts; sts.ReadToken(S"s[*] ");
		cell dest[11] = {0};
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == ERROR_INVALID_ARRAY_SIZE;
	})
	CTEST(StringSRun14, { StringSpecifier sts; sts.ReadToken(S"s[*] ");
		cell dest[11] = {1, 42};
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && dest[1] == '\0';
	})
	
	
	
	CTEST(StringSRun15, { StringSpecifier sts; sts.ReadToken(S"S(fine)[5] ");
		cell dest[5];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"", e) == OK && TestCompare("fine", dest);
	})
	CTEST(StringSRun16, { StringSpecifier sts; sts.ReadToken(S"S(bad)[] ");
		cell dest[11];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"", e) == ERROR_INVALID_ARRAY_SIZE;
	})
	CTEST(StringSRun17, { StringSpecifier sts; sts.ReadToken(S"S(longer)[5]");
		cell dest[5];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"", e) == OK && TestCompare("long", dest);
	})
	CTEST(StringSRun18, { StringSpecifier sts; sts.ReadToken(S"S(has space)[*]");
		cell dest[11] = {10};
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"", e) == OK && TestCompare("has space", dest + 1);
	})
	CTEST(StringSRun19, { StringSpecifier sts; sts.ReadToken(S"S(has space)[*] ");
		cell dest[11] = {10};
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"", e) == OK && TestCompare("has space", dest + 1);
	})
/*	CTEST(StringSRun13, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
	})
	CTEST(StringSRun14, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
	})
	CTEST(StringSRun15, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
	})
	CTEST(StringSRun16, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
	})
	CTEST(StringSRun17, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
	})
	CTEST(StringSRun18, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
	})
	CTEST(StringSRun19, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
	})
	CTEST(StringSRun20, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
	})
	CTEST(StringSRun21, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
		cell dest[10];
		TempMemory mem(dest, sizeof (dest) / sizeof (cell));
		Environment e(&mem);
		return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
	})*/
	//CTEST(StringSpec26, { StringSpecifier sts; return sts.ReadToken(S"S(  \\\"  )[4]") == OK && TestCompare("\"", sts.m_default); })
	//CTEST(StringSpec27, { StringSpecifier sts; return sts.ReadToken(S"S(  \\\"  )[4]") == OK && TestCompare("\"", sts.m_default); })
};

