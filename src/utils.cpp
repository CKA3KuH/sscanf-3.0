#include "utils.h"
#include "errors.h"
#include "../sdk/plugin.h"

#include <stdlib.h>

// Replace this if I write my own version.  I still suspect this was the cause
// of the random "f" fails on Linux in version 2.x.
#define STRTOD strtod

int
	Utils::
	Strincmp(char const * a, char const * b, size_t n)
{
	// Cunning trick!  Store all the lower-case options here.
	static char const
		scLower[257] =
			"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" \
			"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F" \
			" !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopq"              \
			"rstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7F"               \
			"\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F" \
			"\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F" \
			"\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF" \
			"\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xBB\xBC\xBD\xBE\xBF" \
			"\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF" \
			"\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF" \
			"\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF" \
			"\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF";
	while (n--)
	{
		char
			d = scLower[*a] - scLower[*b];
		// Don't need to check "*b".  Either it is 0 and *a isn't, in which case
		// d will be non zero.  Or both it and *a are 0, in which case we
		// already check *a, or it isn't 0, in which case the return depends on
		// the value of d and *a.
		if (d) return d;
		else if (!*a)
		{
			if (*b) return d;
			else return 0x80000000;
		}
		++a, ++b;
	}
	return 0;
}

error_t
	Utils::
	ReadOctal(char const * & input, cell & n)
{
	n = 0;
	if (*input == '\0') return ERROR_NAN;
	if ('0' <= *input && *input <= '7')
	{
		do
		{
			n = n * 8 + (*input++ - '0');
		}
		while ('0' <= *input && *input <= '7');
	}
	else return ERROR_NAN;
	return OK;
}

error_t
	Utils::
	ReadDecimal(char const * & input, cell & n)
{
	bool
		neg = false;
	n = 0;
	if (*input == '\0') return ERROR_NAN;
	else if (*input == '-')
	{
		neg = true;
		++input;
	}
	if ('0' <= *input && *input <= '9')
	{
		do
		{
			n = n * 10 + (*input++ - '0');
		}
		while ('0' <= *input && *input <= '9');
	}
	else return ERROR_NAN;
	if (neg) n = -n;
	return OK;
}

cell
	GetHexCharacter(char const c)
{
	return
		('0' <= c && c <= '9') ? (c - '0') : (
		('a' <= c && c <= 'f') ? (c - 'a' + 10) : (
		('A' <= c && c <= 'F') ? (c - 'A' + 10) : (
		(-1))));
}

error_t
	Utils::
	ReadHex(char const * & input, cell & n)
{
	cell
		cur;
	n = 0;
	if (*input == '\0') return ERROR_NAN;
	else if (*input == '0')
	{
		++input;
		if (*input == 'x' || *input == 'X')
		{
			++input;
		}
		else if (GetHexCharacter(*input) == -1) return OK;
	}
	if ((cur = GetHexCharacter(*input++)) == -1) return ERROR_NAN;
	else
	{
		do
		{
			n = n * 16 + cur;
		}
		while ((cur = GetHexCharacter(*input++)) != -1);
	}
	--input;
	return OK;
}

error_t
	Utils::
	ReadBinary(char const * & input, cell & n)
{
	n = 0;
	if (*input == '\0') return ERROR_NAN;
	else if (*input == '0')
	{
		++input;
		if (*input == 'b' || *input == 'B')
		{
			++input;
		}
		else if (*input != '0' && *input != '1') return OK;
	}
	if (*input == '0' || *input == '1')
	{
		for ( ; ; )
		{
			switch (*input)
			{
				case '0':
					n <<= 1;
					break;
				case '1':
					n = (n << 1) | 1;
					break;
				default:
					return OK;
			}
			++input;
		}
	}
	return ERROR_NAN;
}

error_t
	Utils::
	ReadNum(char const * & input, cell & n)
{
	// This function takes an input of an unknown type, and returns an integer
	// based on trying to parse it as either oct, bool, hex, or decimal.  It can
	// try multiple different types.  First, determine the type:
	bool
		isBin = false,
		isHex = false,
		isOct = false;
	char const *
		cur = input;
	n = 0;
	if (*cur == '0')
	{
		isOct = true; // Possibly.
		++cur;
		if (*cur == 'x' || *cur == 'X') return Utils::ReadHex(input, n);
		// Special case = allows for binary numbers with "0b" prefix.
		else if (*cur == 'b' || *cur == 'B')
		{
			isBin = true;
			isOct = false;
			// Covers the corner-case of 0b5567 being HEX, but without any
			// identifying HEX characters after the first "b".  Note that the
			// order of the checks at the end is important to allow "0b0101" to
			// still be correctly interpreted as BIN, not HEX.
			isHex = true;
			// Don't check the "b" twice, but don't allow two "b"s.
			++cur;
		}
		else if (GetHexCharacter(*cur) == -1)
		{
			// Not ANY valid character remaining.
			++input;
			return OK;
		}
	}
	else if (*cur == '-') return Utils::ReadDecimal(input, n);
	else if (GetHexCharacter(*cur) == -1) return ERROR_NAN;
	// There is at least one valid character.
	for ( ; ; )
	{
		// Determine the highest valid set of characters.
		if (('a' <= *cur && *cur <= 'f') || ('A' <= *cur && *cur <= 'F')) return Utils::ReadHex(input, n);
		else if ('0' <= *cur && *cur <= '9')
		{
			n = (n * 10) + (*cur - '0');
			if (*cur > '1') isBin = false;
			if (*cur > '7') isOct = false;
		}
		else break;
		++cur;
	}
	// These two are mutually exclusive.
	if (isBin) return Utils::ReadBinary(input, n);
	else if (isOct) return Utils::ReadOctal(input, n);
	else if (isHex) return Utils::ReadHex(input, n);
	// Is decimal, but we inlined that function to the main loop of this
	// function (since it only took one line to do so for positive numbers).
	return OK;
};

error_t
	Utils::
	ReadLogical(char const * & input, cell & n)
{
	// This version is far more strict than the old version.  The input MUST be
	// either a boolean number (101, 0b001, 0, etc), "true", "false", "nil",
	// "no", or "yes".  Currently only does English - sorry :(.  Also just "n",
	// "f", "t", and "y".
	switch (*input++)
	{
		case '\0':
			break;
		case '0':
		case '1':
			--input;
			Utils::ReadBinary(input, n); // No need for "TRY".
			if (n) n = 1;
			return OK;
		case 'f':
		case 'F':
			// "f", "F", "false".
			if (!Utils::Strincmp(input, "alse", 4)) input += 4;
			goto ReadLogical_f;
		case 't':
		case 'T':
			// "t", "T", "true".
			if (!Utils::Strincmp(input, "rue", 3)) input += 3;
			goto ReadLogical_t;
		case 'y':
		case 'Y':
			// "y", "Y", "yes".
			if (!Utils::Strincmp(input, "es", 2)) input += 2;
			goto ReadLogical_t;
		case 'n':
		case 'N':
			// "n", "N", "no", "nil", "null".
			if ((*input | 0x20) == 'o') input += 1;
			else if (!Utils::Strincmp(input, "il", 2)) input += 2;
			else if (!Utils::Strincmp(input, "ull", 3)) input += 3;
			goto ReadLogical_f;
	}
	--input;
	return ERROR_NAN;
ReadLogical_t:
	n = 1;
	return OK;
ReadLogical_f:
	n = 0;
	return OK;
}

error_t
	Utils::
	ReadIEEE(char const * & input, cell & n)
{
	// STRTOD SHOULD pick these cases up, but doesn't seem to...  Actually, it
	// seems that the spec says it should, but VS 2010 doesn't.  Not tested on
	// GCC/Clang/Other yet.
	if (*input == '\0') return ERROR_NAN;
	else if ((*input | 0x20) == 'n')
	{
		if (!Utils::Strincmp(input, "NEGATIVE_INF", 12))
		{
			n = FLOAT_NEG_INFINITY;
			if (!Utils::Strincmp(input + 12, "INITY", 5)) input += 17;
			else input += 2;
			return OK;
		}
		else if (!Utils::Strincmp(input, "NEG_INF", 7))
		{
			n = FLOAT_NEG_INFINITY;
			if (!Utils::Strincmp(input + 7, "INITY", 5)) input += 12;
			else input += 7;
			return OK;
		}
		else if (!Utils::Strincmp(input, "NAN_E", 5))
		{
			input += 5;
			n = FLOAT_NAN_E;
			return OK;
		}
		else if (!Utils::Strincmp(input, "NAN", 3))
		{
			input += 3;
			n = FLOAT_NAN;
			return OK;
		}
	}
	else if (!Utils::Strincmp(input, "INF", 3))
	{
		n = FLOAT_INFINITY;
		if (!Utils::Strincmp(input + 3, "INITY", 5)) input += 8;
		else input += 3;
		return OK;
	}
	else if (*input == '-')
	{
		if (!Utils::Strincmp(input + 1, "INF", 3))
		{
			n = FLOAT_NEG_INFINITY;
			if (!Utils::Strincmp(input + 4, "INITY", 5)) input += 9;
			else input += 4;
			return OK;
		}
	}
	char *
		end;
	// Apparently this takes care of more sequences than I thought.
	float
		f = (float)STRTOD(input, &end);
	FAIL(end != input, ERROR_NAN);
	input = end;
	n = amx_ftoc(f);
	return OK;
}

error_t
	Utils::
	ReadFloat(char const * & input, cell & n)
{
	// "strtof" allows many formats we don't.  Detect and disallow them.
	if (*input == '\0') return ERROR_NAN;
	else if (*input == '0' && (*(input + 1) | 0x20) == 'x')
	{
		++input;
		n = 0;
		return OK;
	}
	else if (*input == '-' && !Utils::Strincmp(input + 1, "INF", 3)) return ERROR_NAN;
	else if (!Utils::Strincmp(input, "INF", 3) || !Utils::Strincmp(input, "NAN", 3)) return ERROR_NAN;
	char *
		end;
	// This function prototype seems wrong:
	//  
	//  double strtod(char const * str, char ** endptr);
	//  
	// It takes an input of "char const *" and returns "char **".  Given that
	// this points to the end of the number in the input string, the "const"
	// should not be removed!  There does seem to be documentation on this,
	// including mentioning that C++ overloads correctly, but I don't like it!
	// Should use "strtof", but that's not in Visual Studio 2010.  I could also
	// roll my own, which would avoid the complex checks above, and in fact I
	// have written my own in the past, but I'd rather not if I can avoid it.
	float
		f = (float)STRTOD(input, &end);
	FAIL(end != input, ERROR_NAN);
	// BAD BAD BAD  (see note above).
	input = end;
	n = amx_ftoc(f);
	return OK;
}

error_t
	Utils::
	ReadChar(char const * & input, cell & n)
{
	switch ((n = *input))
	{
		case '\0': return ERROR_NAN;
		case '\\':
		{
			signed char
				ch;
			++input;
			if ((ch = (signed char)*input) < 1) return ERROR_INVALID_ESCAPE;
			if (ch == '0')
			{
				// Leading zero.
				Utils::ReadOctal(input, n);
				if (*input == ';') ++input;
				return OK;
			}
			else if ('1' <= ch && ch <= '9')
			{
				// Can't fail, already validated.  This uses the DECIMAL digits
				// of PAWN, not the OCTAL digits of C.
				Utils::ReadDecimal(input, n);
				if (*input == ';') ++input;
				return OK;
			}
			else if (('A' <= ch && ch <= 'Z' && (ch |= 0x20)) || ('a' <= ch && ch <= 'z'))
			{
				switch (ch)
				{
					case 'a':  n = '\a'; break; // Alarm.
					case 'b':  n = '\b'; break; // Backspace.
					case 'e':  n = '\033;'; break; // Escape.
					case 'f':  n = '\f'; break; // Form feed.
					case 'n':  n = '\n'; break; // New line.
					case 'r':  n = '\r'; break; // Carriage Return.
					case 't':  n = '\t'; break; // Tab.
					case 'v':  n = '\v'; break; // Vertical tab.
					case 'x': case 'u':
					{
						++input;
						TRY(Utils::ReadHex(input, n));
						if (*input == ';') ++input;
						return OK;
					}
					default: return ERROR_INVALID_ESCAPE;
				}
			}
			else
			{
				// Just accept all others - there are too many characters that
				// MAY be escaped if users have different custom delimiters.
				n = ch;
			}
		}
	}
	++input;
	return OK;
}

error_t
	Utils::
	ReadCharEx(char const * & input, cell & n)
{
	// Detect optional enclosing quotes.
	bool
		quotes = false;
	if (*input == '\'')
	{
		quotes = true;
		++input;
	}
	TRY(Utils::ReadChar(input, n));
	if (quotes)
	{
		FAIL(*input == '\'', ERROR_UNCLOSED_CHARACTER_LIT);
		++input;
	}
	return OK;
}

void
	Utils::
	SkipWhitespace(char const * & input)
{
	while ((unsigned char)(*input - 1) < ' ') ++input;
}

error_t
	Utils::
	GetBounded(char const * & input, char const * & output, char start, char end, size_t * len)
{
	Utils::SkipWhitespace(input);
	if (*input++ != start) return ERROR_EXPECTED_A_GOT_B_1;
	Utils::SkipWhitespace(input);
	output = input;
	char const *
		ep = input;
	cell
		ch;
	int
		depth = 1;
	size_t
		posLen = 0,
		truLen = 0;
	while (*input)
	{
		if (*input == end)
		{
			if (--depth == 0) break;
			ep = ++input;
			truLen = ++posLen;
		}
		else if ((unsigned char)*input > ' ')
		{
			// Allow nested start/end pairs.  Note that if "start" and "end" are
			// the same, then the earlier branch will have been hit already.
			if (*input == start) ++depth, ++input;
			else TRY(Utils::ReadChar(input, ch));
			ep = input;
			truLen = ++posLen;
		}
		else
		{
			++input;
			++posLen;
		}
	}
	if (*input != end) return ERROR_EXPECTED_A_GOT_B_2;
	*const_cast<char *>(ep) = '\0';
	if (len) *len = truLen;
	++input;
	Utils::SkipWhitespace(input);
	return OK;
}

// These are mostly tested by the function above.
error_t
	Utils::
	GetDefaults(char const * & input, char const * & output, size_t * len)
{
	error_t
		error = Utils::GetBounded(input, output, '(', ')', len);
	switch (error)
	{
		case OK: return OK;
		case ERROR_EXPECTED_A_GOT_B_1: FAIL(false, ERROR_NO_DEFAULT_START);
		case ERROR_EXPECTED_A_GOT_B_2: FAIL(false, ERROR_NO_DEFAULT_END);
	}
	return error;
};

error_t
	Utils::
	GetParams(char const * & input, char const * & output, size_t * len)
{
	error_t
		error = Utils::GetBounded(input, output, '<', '>', len);
	switch (error)
	{
		case OK: return OK;
		case ERROR_EXPECTED_A_GOT_B_1: FAIL(false, ERROR_NO_PARAM_START);
		case ERROR_EXPECTED_A_GOT_B_2: FAIL(false, ERROR_NO_PARAM_END);
	}
	return error;
};

error_t
	Utils::
	GetSizes(char const * & input, char const * & output, size_t * len)
{
	error_t
		error = Utils::GetBounded(input, output, '[', ']', len);
	switch (error)
	{
		case OK: return OK;
		case ERROR_EXPECTED_A_GOT_B_1: FAIL(false, ERROR_NO_ARRAY_START);
		case ERROR_EXPECTED_A_GOT_B_2: FAIL(false, ERROR_NO_ARRAY_END);
	}
	return error;
};

error_t
	Utils::
	GetLength(char const * & input, int * s)
{
	char const *
		len;
	TRY(Utils::GetSizes(input, len));
	if (*len == '\0')
	{
		// No length given ([]).  For now just accept that, let the code
		// determine whether that is valid at some other point in the future.
		*s = 0;
		return OK;
	}
	else if (*len == '*')
	{
		// Look up the length.
		++len;
		*s = -1;
		Utils::SkipWhitespace(len);
		if (!Utils::Strincmp(len, "char", 4))
		{
			len += 4;
			*s = -2;
		}
		else if (!Utils::Strincmp(len, "bit", 3))
		{
			len += 3;
			*s = -3;
		}
		FAIL(*len == '\0', ERROR_NAN);
	}
	else
	{
		cell
			ret;
		TRY(Utils::ReadDecimal(len, ret));
		Utils::SkipWhitespace(len);
		if (!Utils::Strincmp(len, "char", 4))
		{
			len += 4;
			ret = (ret + 4 - 1) / 4;
		}
		else if (!Utils::Strincmp(len, "bit", 3))
		{
			len += 3;
			ret = (ret + 32 - 1) / 32;
		}
		*s = ret;
		FAIL(*len == '\0', ERROR_NAN);
		FAIL(ret > 0, ERROR_INVALID_ARRAY_SIZE);
	}
	return OK;
};

error_t
	Utils::
	GetString(char const * & input, char const * & output, char const t, size_t * len)
{
	error_t
		error = Utils::GetBounded(input, output, t, t, len);
	switch (error)
	{
		case OK: return OK;
		case ERROR_EXPECTED_A_GOT_B_1: FAIL(false, ERROR_EXPECTED_A_GOT_B_2, t, *input);
		case ERROR_EXPECTED_A_GOT_B_2: FAIL(false, ERROR_NO_STRING_END);
	}
	return error;
};

error_t
	Utils::
	CopyString(cell * dest, char const * src, size_t len, bool pad)
{
	FAIL(len, ERROR_MEMORY_ALLOCATION_FAIL);
	while (--len && *src)
	{
		TRY(Utils::ReadChar(src, *dest));
		++dest;
	}
	// Always has one character reserved for "NULL".
	*dest++ = '\0';
	if (pad)
	{
		while (len--) *dest++ = '\0';
	}
	return OK;
};

