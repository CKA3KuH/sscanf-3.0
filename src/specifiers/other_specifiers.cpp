#include "other_specifiers.h"

cell
	GetHexCharacter(char const c);

error_t
	LiteralSpecifier::
	ReadToken(char const * & input)
{
	// Will either be ' or ".  The former is the preferred syntax as it
	// doesn't require escaping in PAWN literal strings, but " is equally as
	// acceptable if you want to put in the extra effort.
	char
		start = *input++;
	// This MUST be done because the reader ALWAYS skips leading spaces, so
	// there's no way to detect them because the have already been passed
	// over by the time this class' "Run" method is called.  I could back-
	// track, but that could in theory go in to memory located before the
	// start of the string and give a buffer underflow (or worse, crash).
	// Shame C strings aren't doubly NULL terminated (maybe mine will be)...
	// Whitespace should be EXPLICITLY escaped.
	char
		delims[2] = { start, '\0' };
	TRY(Utils::GetStringLength(input, delims, &m_length));
	FAIL(m_length, ERROR_NO_STRING_LITERAL);
	// This might throw, but we catch that generically later.
	m_literal = new cell [m_length + 1];
	// Currently has no failure modes when called after "GetStringLength".
	Utils::GetString(m_literal, input, m_length);
	// Skip the end character and whitespace.
	NEXT(input, start, ERROR_NO_STRING_END);
	return OK;
}

bool
	Matches(char const * one, cell const * two, size_t len)
{
	while (len--)
	{
		if (*one++ != *two++) return false;
	}
	return true;
}

error_t
	LiteralSpecifier::
	Run(char const * & input, Environment & env)
{
	if (!m_literal) return OK;
	char const *
		end = input + strlen(input) - m_length;
	while (input <= end)
	{
		if (Matches(input, m_literal, m_length))
		{
			input += m_length;
			return OK;
		}
		++input;
	}
	return ERROR_NO_STRING_MATCH;
}

error_t
	OptionSpecifier::
	ReadToken(char const * & input)
{
	// Skip the "?".
	++input;
	// Skip the "<".
	NEXT(input, '<', ERROR_NO_PARAM_START);
	size_t
		lenName;
	if (Utils::GetStringLength(input, ">=", &lenName) == ERROR_NO_STRING_END) return ERROR_NO_PARAM_END;
	// Check if there is a value for the option.
	m_option = new char [lenName + 1];
	cell *
		dest;
	try
	{
		dest = new cell [lenName + 1];
	}
	catch (...)
	{
		delete [] m_option;
		throw;
	}
	// Copy the data.
	Utils::GetString(dest, input, lenName);
	Utils::SkipWhitespace(input);
	m_option[lenName] = '\0';
	while (lenName--)
	{
		m_option[lenName] = (char)dest[lenName];
	}
	delete [] dest;
	// Finish (skip the ">").
	if (*input == '=')
	{
		// Get the length of the value.
		++input;
		Utils::SkipWhitespace(input);
		Utils::ReadDecimal(input, m_value);
	}
	NEXT(input, '>', ERROR_NO_PARAM_END);
	// Option neatly stored for later!
	return OK;
}

error_t
	OptionSpecifier::
	Run(char const * & input, Environment & env)
{
	// So that the environment doesn't try to skip two sets of delimiters.
	//env.ZeroRead();
	env.SetOption(m_option, m_value);
	return OK;
}

error_t
	MinusSpecifier::
	ReadToken(char const * & input)
{
	++input;
	// Get the child.
	TRY(gParser.GetNext(input, &m_child));
	FAIL(m_child, ERROR_NO_CHILD);
	// Lie about what we are, and pass all other ops through.  This allows
	// other specifiers that only check the specifier type to not need to
	// know anything about possible "minus" operators.
	m_specifier = m_child->GetSpecifier();
	m_child->SetSkip();
	return OK;
}

// This needs to count all READS, but not WRITES.
cell
	MinusSpecifier::
	Skip(Environment & env)
{
	if (m_child) return m_child->Skip(env);
	return 0;
}

error_t
	MinusSpecifier::
	Run(char const * & input, Environment & env)
{
	// So that the environment doesn't try to skip two sets of delimiters.
	//env.ZeroRead();
	// Skips don't do anything but "skip".
	//return env.Skip(Skip(env));
	if (m_child) m_child->Skip(env);
	return OK;
}

#ifndef SSCANF_DEBUG
inline
#endif
error_t
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

TEST(Num0,  { cell n; return ReadNum(S"0", n) == OK && n == 0; })
TEST(Num1,  { cell n; return ReadNum(S"1", n) == OK && n == 1; })
TEST(Num2,  { cell n; return ReadNum(S"7", n) == OK && n == 7; })
TEST(Num3,  { cell n; return ReadNum(S"9", n) == OK && n == 9; })
TEST(Num4,  { cell n; return ReadNum(S"F", n) == OK && n == 0xF; })
TEST(Num5,  { cell n; return ReadNum(S"0A", n) == OK && n == 0xA; })
TEST(Num6,  { cell n; return ReadNum(S"0x1", n) == OK && n == 0x1; })
TEST(Num7,  { cell n; return ReadNum(S"0Xb", n) == OK && n == 0xB; })
TEST(Num8,  { cell n; return ReadNum(S"-0", n) == OK && n == -0; })
TEST(Num9,  { cell n; return ReadNum(S"-5", n) == OK && n == -5; })
TEST(Num10, { cell n; return ReadNum(S"01234", n) == OK && n == 01234; })
TEST(Num11, { cell n; return ReadNum(S"1234", n) == OK && n == 1234; })
TEST(Num12, { cell n; return ReadNum(S"06789", n) == OK && n == 6789; })
TEST(Num13, { cell n; return ReadNum(S"6789", n) == OK && n == 6789; })
TEST(Num22, { cell n; return ReadNum(S"0b1234", n) == OK && n == 0x0B1234; })
// SPECIAL CASE - looks like binary, but isn't!
TEST(Num14, { cell n; return ReadNum(S"010101", n) == OK && n == 010101; })
TEST(Num15, { cell n; return ReadNum(S"101010", n) == OK && n == 101010; })
TEST(Num16, { cell n; return ReadNum(S"0b101010", n) == OK && n == 42; })
TEST(Num17, { cell n; return ReadNum(S"0B010101", n) == OK && n == 21; })
TEST(Num18, { cell n; return ReadNum(S"0123F", n) == OK && n == 0x0123F; })
TEST(Num19, { cell n; return ReadNum(S"-A", n) == ERROR_NAN; })
TEST(Num20, { cell n; return ReadNum(S"G", n) == ERROR_NAN; })
TEST(Num21, { cell n; return ReadNum(S"(4)", n) == ERROR_NAN; })

ITEST(NumSpecifier, NumXm)
ITEST(NumSpecifier, NumXn)
ITEST(NumSpecifier, NumXo)
ITEST(NumSpecifier, NumXp)
ITEST(NumSpecifier, NumXq)
ITEST(NumSpecifier, NumXr)
ITEST(NumSpecifier, NumXs)
ITEST(NumSpecifier, NumXt)
ITEST(NumSpecifier, NumXu)

error_t
	NumSpecifier::
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
}

error_t
	NumSpecifier::
	Run(char const * & input, Environment & env)
{
	cell
		dest;
	TRY(ReadNum(input, dest));
	return env.SetNextValue(dest);
	//return env.SkipDelimiters();
}

