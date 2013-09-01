#pragma once

class TrivialSpecifier : public Specifier
{
public:
	// cons
		EndingSpecifier(char c)
	:
		Specifier(c)
	{
	};
	
	error_t
		Get(char const * & input, cell & result)
	{
		return OK;
	};
	
	error_t
		Clone(
			Specifier ** that)
	{
		// Flyweight pattern.
		*that = this;
		return OK;
	};
	
	virtual error_t
		ReadToken(char const * & input)
	{
		NEXT(input, m_specifier, ERROR_EXPECTED_A_GOT_B, m_specifier, *input);
		return OK;
	};
	
	virtual error_t
		ReadValue(char const * & input, cell & result)
	{
		return OK;
	};
	
	error_t
		Run(char const * & input, Memory * memory, Delimiters * delimiters)
	{
		return OK;
	};
	
	int
		GetMemoryUsage() { return 0; };
	
private:
	// dest
		~EndingSpecifier()
	{
	};
};

