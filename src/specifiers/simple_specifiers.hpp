#pragma once

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
		if (*input | 0x20 == GetSpecifier())
		{
			// Skip it - simple as!
			if (*input++ != GetSpecifier())
			{
				SetOptional();
				// Capital letter - read in the deafult.
				// Skip the opening bracket.
				NEXT(input, '(', ERROR_NO_DEAFULT_START);
				TRY(Run(input, gDefaultEnvironment(&m_default)));
				// Skip the closing bracket.
				NEXT(input, ')', ERROR_NO_DEAFULT_END);
			}
		}
		else FAIL(false, ERROR_EXPECTED_A_GOT_B, GetSpecifier(), *input);
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
	
protected:
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
};

