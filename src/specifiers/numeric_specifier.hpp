#pragma once

class NumericSpecifier : public Specifier
{
public:
	virtual error_t
		ReadToken(char const * & input)
	{
		char
			c = *input;
		FAIL('0' <= c && c <= '9', ERROR_EXPECTED_A_GOT_B, "digit", c);
		int
			num;
		//error_t
		//	error =
		Utils::ReadDecimal(input, num);
		// These can't happen thanks to the assert above.
		//if (error != OK) return error;
		//if (num < 0) return ERROR_INVALID_COUNT;
		if (num < 2) sWARN(WARNING_UNUSUAL_COUNT);
		m_count = num;
		Utils::SkipWhitespace(input);
		return Parser::GetToken(input, m_children);
	};
	
private:
	int
		m_count;
};

