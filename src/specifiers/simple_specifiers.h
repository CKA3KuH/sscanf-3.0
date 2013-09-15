#pragma once

#include "../specifiers.h"

class SimpleSpecifier : public Specifier
{
PUBLIC:
	// cons
		SimpleSpecifier(char c, ReadFunction_t f)
	:
		Specifier(c),
		m_default(0),
		m_read(f),
		m_lower(0x80000000),
		m_upper(0x7FFFFFFF)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input);
	
	CLONE();
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
	// cons
		SimpleSpecifier(SimpleSpecifier const & that)
	:
		Specifier(that),
		m_default(that.m_default),
		m_read(that.m_read),
		m_lower(that.m_lower),
		m_upper(that.m_upper)
	{
	};
	
	virtual bool
		CheckRange(cell lower, cell upper) const
	{
		return lower <= upper;
	};
	
	virtual cell
		Skip(Environment & env)
	{
		env.Skip(1);
		return 1;
	};
	
PROTECTED:
	cell
		m_lower,
		m_upper,
		m_default;
	
	ReadFunction_t
		m_read;
};

