#pragma once

class LiteralSpecifier : public Specifier
{
public:
	// cons
		LiteralSpecifier()
	:
		Specifier(),
		m_literal(0)
	{
	};
	
	error_t
		ReadToken(char const * & input)
	{
		char
			start = *input;
		static char * const
			sDelimList[] = {"?", nullptr};
		FAIL(start == '\'' || start == '"', ERROR_EXPECTED_A_GOT_B, "string literal", *input);
		sDelimList[0][0] = start; // Only end on the correct close.
		++input;
		// Note that BOTH these functions skip whitespace unless it is
		// EXPLICITLY escaped.
		size_t
			len = Utils::GetStringLength(input, sDelimList);
		FAIL(*(input + len) == start, ERROR_UNCLOSED_STRING_LITERAL);
		// This might throw, but we catch that generically later.
		m_literal = new char [len + 1];
		Utils::GetString(input, m_literal, len + 1);
		++m_literal;
		return OK;
	};
	
private:
	char *
		m_literal;
};

