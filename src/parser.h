#pragma once

#include "errors.h"

class Specifier;

class Parser
{
public:
	// cons
		Parser()
	{
		memset(m_specifiers, 0, 128 * sizeof (Specifier *));
		AddAll();
	};
	
	error_t
		Compile(char const * & input, Specifier ** dest);
	
	error_t
		GetNext(char const * & input, Specifier ** dest);
	
	error_t
		Add(Specifier * prototype);
	
	error_t
		AddAs(Specifier * prototype, char specifier);
	
	error_t
		AddAll();
	
private:
	Specifier *
		m_specifiers[128];
};

extern Parser
	gParser;

