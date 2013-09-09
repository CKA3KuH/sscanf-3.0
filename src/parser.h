#pragma once

#include "errors.h"

class Specifier;

class Parser;

typedef
	error_t (* InitFunction_t)(Parser *);

class Parser
{
public:
	// cons
		Parser()
	{
		memset(m_specifiers, 0, sizeof (m_specifiers));
		AddAll();
	};
	
	// cons
		Parser(InitFunction_t init)
	{
		memset(m_specifiers, 0, sizeof (m_specifiers));
		if (init) (* init)(this);
		else AddAll();
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
		m_specifiers['~' - '!' + 1];
	
	CTEST(Parser1, { Parser p; return '~' - '!' < sizeof (p.m_specifiers) / sizeof (Specifier *); })
};

extern Parser
	gParser;

