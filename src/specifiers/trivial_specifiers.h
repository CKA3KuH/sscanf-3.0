#pragma once

#include "../specifiers.h"

class TrivialSpecifier : public Specifier
{
PUBLIC:
	// cons
		TrivialSpecifier(char c)
	:
		Specifier(c)
	{
	};
	
	CLONE();
	
	virtual error_t
		ReadToken(char const * & input)
	{
		++input;
		return OK;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		// This shouldn't ever by run.
		return ERROR_RAN_TRIVIAL;
	};
	
	// cons
		TrivialSpecifier(TrivialSpecifier const & that)
	:
		Specifier(that)
	{
	};
	
	// dest
		~TrivialSpecifier()
	{
	};
	
	// This doesn't really have many failure modes - it is "trivial" after all!
};

