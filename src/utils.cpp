#include "utils.h"
#include "errors.h"

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
	if (*len == '*')
	{
		// Look up the length.
		*s = -1;
		return OK;
	}
	if (*len == '\0')
	{
		// No length given ([]).  For now just accept that, let the code
		// determine whether that is valid at some other point in the future.
		*s = 0;
		return OK;
	}
	cell
		ret;
	TRY(Utils::ReadDecimal(len, ret));
	FAIL(*len == '\0', ERROR_NAN);
	FAIL(ret > 0, ERROR_INVALID_ARRAY_SIZE);
	*s = ret;
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

