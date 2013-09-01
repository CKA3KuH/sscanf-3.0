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
		if (*input | 0x20 == m_specifier)
		{
			// Skip it - simple as!
			if (*input++ != m_specifier)
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
		else FAIL(false, ERROR_EXPECTED_A_GOT_B, m_specifier, *input);
		return OK;
	};
	
	virtual error_t
		ReadValue(char const * & input, cell & result)
	{
		return (*m_read)(input, result);
	};
	
	virtual error_t
		Clone(Specifier ** dest)
	{
		*dest = new SimpleSpecifier(*this);
		return OK;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		cell
			dest;
		TRY(ReadValue(input, dest));
		env.SetNextValue(dest);
		return env.SkipDelimiters();
	};
	
	virtual int
		GetMemoryUsage() { return 1; };
	
	virtual // dest
		~SimpleSpecifier()
	{
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

