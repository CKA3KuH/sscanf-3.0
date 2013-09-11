#pragma once

#include "../specifiers.h"
#include "../utils.h"
#include "../parser.h"
#include "../options.h"

class LiteralSpecifier : public Specifier
{
PUBLIC:
	// cons
		LiteralSpecifier()
	:
		Specifier('"'),
		m_length(0),
		m_literal(nullptr)
	{
	};
	
	// cons
		LiteralSpecifier(LiteralSpecifier const & that)
	:
		Specifier(that),
		m_length(that.m_length),
		m_literal(nullptr)
	{
		if (that.m_literal)
		{
			m_literal = new cell [m_length + 1];
			memcpy(m_literal, that.m_literal, (m_length + 1) * sizeof (cell));
		}
	};
	
	virtual // cons
		~LiteralSpecifier()
	{
		delete m_literal;
	};
	
	CLONE();
	
	virtual error_t
		ReadToken(char const * & input);
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
PRIVATE:
	size_t
		m_length;
	
	cell *
		m_literal;
};

class OptionSpecifier : public Specifier
{
PUBLIC:
	// cons
		OptionSpecifier()
	:
		Specifier('?'),
		m_option(OPTION_NONE),
		m_value(0)
	{
	};
	
	// cons
		OptionSpecifier(OptionSpecifier const & that)
	:
		Specifier(that),
		m_option(that.m_option),
		m_value(that.m_value)
	{
	};
	
	virtual // cons
		~OptionSpecifier()
	{
	};
	
	CLONE();
	
	virtual error_t
		ReadToken(char const * & input);
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
PRIVATE:
	//char *
	//	m_option;
	option_t
		m_option;
	
	cell
		m_value;
};

class MinusSpecifier : public Specifier
{
PUBLIC:
	// cons
		MinusSpecifier()
	:
		Specifier('-'), // Actually has 10 valid specifiers.
		m_child(nullptr)
	{
		// Always "skip".
		SetSkip();
	};
	
	// cons
		MinusSpecifier(MinusSpecifier const & that)
	:
		Specifier(that),
		m_child(that.m_child)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input);
	
	CLONE();
	
	// This needs to count all READS, but not WRITES.
	virtual cell
		Skip(Environment & env);
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
	virtual // dest
		~MinusSpecifier()
	{
		delete m_child;
	};
	
	virtual int
		CountChildren() const { return m_child ? 1 : 0; };
	
PRIVATE:
	Specifier *
		m_child;
};

