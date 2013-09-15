#include "simple_specifiers.h"

int
	CountHyphens(char const * str)
{
	int
		ret = 0;
	while (*str) if (*str++ == '-') ++ret;
	return ret;
}

error_t
	GetRanges(Specifier * that, char const * pars, int hyphens, cell * lower, cell * higher)
{
	// This doesn't actually guarantee that the two values (lower and higher)
	// are comparable, only that they are readable.  If this was Haskell the
	// function would have constraint (Read a) but not (Ord a), despite the
	// inherently ordered nature of ranges.  This is because the function works
	// just as well for string ranges as numeric ranges.
	switch (hyphens)
	{
		case 1:
		{
			// Maybe missing.
			if (*pars == '-')
			{
				++pars;
				Utils::SkipWhitespace(pars);
				TRY(that->Run(pars, DefaultEnvironment::Get(higher)));
			}
			else
			{
				TRY(that->Run(pars, DefaultEnvironment::Get(lower)));
				NEXT(pars, '-', ERROR_INVALID_RANGE);
				if (*pars) TRY(that->Run(pars, DefaultEnvironment::Get(higher)));
			}
			break;
		}
		case 2:
		{
			// This is VASTLY simplified by the fact that if the first value is
			// present, then the second can't be negative.
			FAIL(*pars == '-', ERROR_INVALID_RANGE);
			char const *
				p = pars + 1;
			Utils::SkipWhitespace(p);
			// Negative upper bound.
			if (*p == '-') TRY(that->Run(p, DefaultEnvironment::Get(higher)));
			else
			{
				// Negative lower bound.
				TRY(that->Run(pars, DefaultEnvironment::Get(lower)));
				NEXT(pars, '-', ERROR_INVALID_RANGE);
				if (*pars) TRY(that->Run(pars, DefaultEnvironment::Get(higher)));
			}
			break;
		}
		case 3:
		{
			// Easy.
			TRY(that->Run(pars, DefaultEnvironment::Get(lower)));
			NEXT(pars, '-', ERROR_INVALID_RANGE);
			TRY(that->Run(pars, DefaultEnvironment::Get(higher)));
			break;
		}
	}
	return OK;
}

error_t
	SimpleSpecifier::
	ReadToken(char const * & input)
{
	// Check this has the correct specifier (shouldn't ha
	//FAIL((*input | 0x20) == GetSpecifier(), ERROR_EXPECTED_A_GOT_B_2, GetSpecifier(), *input);
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
		// We use temporary variables so that the ranges aren't applied in "Run"
		// while getting the new ranges.
		cell
			lower = m_lower,
			upper = m_upper;
		TRY(GetRanges(this, pars, hyphens, &lower, &upper));
		FAIL(CheckRange(lower, upper), ERROR_INVALID_RANGE);
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
	if (e == OK) FAIL(CheckRange(m_lower, dest) && CheckRange(dest, m_upper), ERROR_OUT_OF_RANGE);
	else if (GetOptional()) dest = m_default;
	else return e;
	return env.SetNextValue(dest);
}

