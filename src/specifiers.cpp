#include "parser.h"
#include "utils.h"
#include "specifiers.h"

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

