#include "simple_specifiers.h"

error_t
	SimpleSpecifier::
	ReadToken(char const * & input)
{
	// Check this has the correct specifier.
	FAIL((*input | 0x20) == GetSpecifier(), ERROR_EXPECTED_A_GOT_B_2, GetSpecifier(), *input);
	// Check if this is upper-case (optional).
	bool
		opt = false;
	if (*input++ != GetSpecifier()) opt = true;
	Utils::SkipWhitespace(input);
	char const *
		pars = nullptr;
	int
		hyphens;
	// Check for any specified ranges.
	if (*input == '<')
	{
		TRY(Utils::GetParams(input, pars));
		hyphens = CountHyphens(pars);
		FAIL(0 < hyphens && hyphens < 4, ERROR_INVALID_RANGE);
	}
	// NOW get the defaults.
	if (opt) TRY(GetDefault(input, &m_default));
	// THEN read the ranges (so that they aren't applied to the defaults).
	if (pars)
	{
		cell
			lower = 0x80000000,
			upper = 0x7FFFFFFF;
		TRY(GetRanges(this, pars, hyphens, &lower, &upper));
		FAIL(upper >= lower, ERROR_INVALID_RANGE);
		m_lower = lower;
		m_upper = upper;
	}
	return OK;
}

error_t
	SimpleSpecifier::
	Run(char const * & input, Environment & env)
{
	cell
		dest;
	error_t
		e = (*m_read)(input, dest);
	// Don't subject default values to the range checks - assume the coder
	// knows what they are doing.
	if (e == OK) FAIL(dest >= m_lower && dest <= m_upper, ERROR_OUT_OF_RANGE);
	else if (GetOptional()) dest = m_default;
	else return e;
	return env.SetNextValue(dest);
}

