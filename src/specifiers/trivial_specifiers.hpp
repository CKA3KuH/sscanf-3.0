#pragma once

class TrivialSpecifier : public Specifier
{
public:
	// cons
		TrivialSpecifier(char c)
	:
		Specifier(c)
	{
	};
	
	virtual error_t
		Clone(Specifier ** that)
	{
		// Flyweight pattern.
		*that = this;
		return OK;
	};
	
	virtual error_t
		ReadToken(char const * & input)
	{
		++input;
		return OK;
	};
	
	virtual error_t
		Run(char const * & input, Memory * memory, Delimiters * delimiters)
	{
		// This shouldn't ever by run.
		return ERROR_RAN_TRIVIAL;
	};
	
	virtual int
		GetMemoryUsage() { return 0; };
	
private:
	// cons
		TrivialSpecifier(TrivialSpecifier const & that)
	{
	};
	
	// dest
		~TrivialSpecifier()
	{
	};
};

