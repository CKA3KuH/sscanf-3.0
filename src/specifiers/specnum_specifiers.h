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
		char const *
			start;
		TRY(Utils::GetDefaults(input, start));
		// Don't constrain the defaults to just 0 or 1.
		if (*start == '0' || *start == '1') TRY(Utils::ReadBinary(start, *target));
		else TRY(Utils::ReadLogical(start, *target));
		FAIL(*start == '\0', ERROR_INVALID_DEFAULT);
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
		float
			n;
		n = std::numeric_limits<float>::lowest();
		m_lower = amx_ftoc(n);
		n = (std::numeric_limits<float>::max)(); // Visual Studio macro hack.
		m_upper = amx_ftoc(n);
	};
	
	CLONE();
	
	virtual bool
		CheckRange(cell lower, cell upper) const
	{
		// Check the ranges as floats.
		return amx_ctof(lower) <= amx_ctof(upper);
	};
};

