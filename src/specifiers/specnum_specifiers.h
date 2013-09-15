#pragma once

#include "simple_specifiers.h"
#include <limits>

class LogicalSpecifier : public SimpleSpecifier
{
public:
	// cons
		LogicalSpecifier()
	:
		SimpleSpecifier('l', &Utils::ReadLogical)
	{
		float
			n;
		n = std::numeric_limits<float>::lowest();
		m_lower = amx_ftoc(n);
		n = (std::numeric_limits<float>::max)(); // Visual Studio macro hack.
		m_upper = amx_ftoc(n);
	};
	
	// // cons
		// LogicalSpecifier(LogicalSpecifier const & that)
	// :
		// SimpleSpecifier(that)
	// {
	// };
	
	CLONE();
	
	virtual error_t
		GetDefault(char const * & input, cell * target)
	{
		// Don't constrain the defaults to just 0 or 1.
		if (*input == '0' || *input == '1') TRY(Utils::ReadBinary(input, *target));
		else TRY(Utils::ReadLogical(input, *target));
		FAIL(*input == '\0', ERROR_INVALID_DEFAULT);
		SetOptional();
		return OK;
	};
};

class FloatSpecifier : public SimpleSpecifier
{
public:
	// cons
		FloatSpecifier(char c, ReadFunction_t f)
	:
		SimpleSpecifier(c, f)
	{
	};
	
	CLONE();
	
	virtual bool
		CheckRange(cell lower, cell upper) const
	{
		// Check the ranges as floats.
		return amx_ctof(lower) <= amx_ctof(upper);
	};
};

