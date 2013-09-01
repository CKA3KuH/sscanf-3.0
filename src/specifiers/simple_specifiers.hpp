#pragma once

class SimpleSpecifier : public Specifier
{
public:
	// cons
		SimpleSpecifier(char c)
	:
		Specifier(c),
		m_default(0)
	{
	};
	
	error_t
		Get(char const * & input, cell & result)
	{
		Utils::SkipWhitespace(input);
		ReadValue(input, result);
		Utils::SkipWhitespace(input);
		if (*input == '\0') return OK;
		while (*delimiters)
		{
			if (Matches(input, *delimiters, input))
			{
				return OK;
			}
			++delimiters;
		}
		return ERROR_NO_DELIMITER;
	};
	
	virtual error_t
		ReadToken(char const * & input)
	{
		// Check this has the correct specifier.
		if (*input | 0x20 == m_specifier)
		{
			// Skip it - simple as!
			if (*input++ != m_specifier)
			{
				SetOptional();
				// Capital letter - read in the deafult.
				// Skip the opening bracket.
				NEXT(input, '(', ERROR_NO_DEAFULT_START);
				// Now read in a value.  This function is ALWAYS pure virtual.
				error_t
					error = ReadValue(input, m_default);
				// Skip the closing bracket.
				NEXT(input, ')', ERROR_NO_DEAFULT_END);
			}
		}
		else FAIL(false, ERROR_EXPECTED_A_GOT_B, m_specifier, *input);
		return OK;
	};
	
	virtual error_t
		ReadValue(char const * & input, cell & result)
	{
		return (*m_read)(input, result);
	};
	
	error_t
		Run(char const * & input, Memory * memory, Delimiters * delimiters)
	{
		Utils::SkipWhitespace(input);
		TRY(ReadValue(input, dest));
		Utils::SkipWhitespace(input);
		return delimiters->Skip();
	};
	
	int
		GetMemoryUsage() { return 1; };
	
private:
	ReadFunction_t
		m_read;
	
	cell
		m_default;
};

class CharSpecifier : public SimpleSpecifier
{
public:
	// cons
		Specifier_c
	:
		SimpleSpecifier('c', nullptr)
	{
		
	};
	
	error_t
		ReadValue(char const * & input, cell & result)
	{
		// Detect enclosing quotes.
		bool
			quotes = false;
		if (*input == '\'')
		{
			quotes = true;
			++input;
		}
		error_t
			error = Utils::ReadChar(input, result);
		if (error != OK) return error;
		if (quotes)
		{
			FAIL(*input == '\'', ERROR_UNCLOSED_CHARACTER_LITERAL);
			++input;
		}
		return OK;
	};
};

