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

#ifdef SSCANF_DEBUG
	#include "specifiers/trivial_specifiers.h"
	#include "specifiers/simple_specifiers.h"
	#include "specifiers/other_specifiers.h"
	#include "specifiers/group_specifiers.h"
	#include "specifiers/numeric_specifier.h"
	#include "specifiers/string_specifier.h"
	
	#include "../sdk/plugin.h"
#endif

TEST(Numeric1,  { NumericSpecifier nm; return nm.ReadToken(S"12i") == OK && nm.CountChildren() == 12;  })
TEST(Numeric2,  { NumericSpecifier nm; return nm.ReadToken(S"*i") == OK && nm.CountChildren() == -1;  })

//TEST(Numeric3,  { NumericSpecifier nm; return nm.ReadToken(S"55i") == OK && nm.Skip(Environment & env) == 55;  })
//TEST(Numeric4,  { NumericSpecifier nm; return nm.ReadToken(S"*i") == OK && nm.Skip(Environment & env) == 0;  })

TEST(Numeric5, 
	{
		NumericSpecifier nm;
		nm.ReadToken(S"3i");
		cell data[3] = {4, 42, 11};
		TestMemory
			tm(data, 3);
		Environment
			env(&tm);
		return
			nm.Run(S"", env) == ERROR_NAN;
	})

TEST(Numeric6, 
	{
		NumericSpecifier nm;
		nm.ReadToken(S"3i");
		cell data[3] = {4, 42, 11};
		TestMemory
			tm(data, 3);
		Environment
			env(&tm);
		return
			nm.Run(S"11 42 11", env) == ERROR_UNKNOWN_ERROR;
	})

TEST(Numeric7, 
	{
		NumericSpecifier nm;
		nm.ReadToken(S"3i");
		cell data[3] = {4, 42, 11};
		TestMemory
			tm(data, 3);
		Environment
			env(&tm);
		return
			nm.Run(S"4 42 11", env) == OK;
	})

TEST(Numeric8, 
	{
		NumericSpecifier nm;
		nm.ReadToken(S"6I(99)");
		cell data[6] = {7, -23, 99, 99, 99, 99};
		TestMemory
			tm(data, 6);
		Environment
			env(&tm);
		return
			nm.Run(S"7 -23", env) == OK;
	})

TEST(Numeric9, 
	{
		NumericSpecifier nm;
		nm.ReadToken(S"7I(99)");
		cell data[7] = {99, 99, 99, 99, 99, 99, 99};
		TestMemory
			tm(data, 7);
		Environment
			env(&tm);
		return
			nm.Run(S"", env) == OK;
	})

TEST(DefaultSimple1, {
	Specifier * spec; gParser.GetNext(S"I(22)", & spec);
	cell data[1] = {22}; TestMemory tm(data, 1); Environment env(&tm);
	return spec->Run(S"", env) == OK;
})

TEST(DefaultSimple2, {
	Specifier * spec; gParser.GetNext(S"D(11)", & spec);
	cell data[1] = {44}; TestMemory tm(data, 1); Environment env(&tm);
	return spec->Run(S"44", env) == OK;
})

TEST(DefaultSimple3, {
	Specifier * spec; gParser.GetNext(S"H(ADD)", & spec);
	cell data[1] = {0xADD}; TestMemory tm(data, 1); Environment env(&tm);
	return spec->Run(S"", env) == OK;
})

TEST(DefaultSimple4, {
	Specifier * spec; gParser.GetNext(S"X(11)", & spec);
	cell data[1] = {0x44}; TestMemory tm(data, 1); Environment env(&tm);
	return spec->Run(S"0x44", env) == OK;
})

TEST(DefaultSimple5, {
	Specifier * spec; gParser.GetNext(S"X(11)", & spec);
	cell data[1] = {0xFE}; TestMemory tm(data, 1); Environment env(&tm);
	return spec->Run(S"FE", env) == OK;
})

TEST(DefaultSimple6, {
	Specifier * spec; gParser.GetNext(S"C('@')", & spec);
	cell data[1] = {'@'}; TestMemory tm(data, 1); Environment env(&tm);
	return spec->Run(S"", env) == OK;
})

TEST(DefaultSimple7, {
	Specifier * spec; gParser.GetNext(S"C(' ')", & spec);
	cell data[1] = {'_'}; TestMemory tm(data, 1); Environment env(&tm);
	return spec->Run(S"_", env) == OK;
})

TEST(DefaultSimple8, {
	Specifier * spec; gParser.GetNext(S"C(q)", & spec);
	cell data[1] = {':'}; TestMemory tm(data, 1); Environment env(&tm);
	return spec->Run(S"':'", env) == OK;
})

ITEST(AltGroup, AltGroup1)
ITEST(AltGroup, AltGroup2)
ITEST(AltGroup, AltGroup4)
ITEST(AltGroup, AltGroup5)
ITEST(AltGroup, AltGroup6)

ITEST(AltGroup, GlobGroup1)
ITEST(AltGroup, GlobGroup2)
ITEST(AltGroup, GlobGroup3)
ITEST(AltGroup, GlobGroup6)
ITEST(AltGroup, GlobGroup7)
ITEST(AltGroup, GlobGroup4)
ITEST(AltGroup, GlobGroup5)

ITEST(MinusSpecifier, Minus0)
ITEST(MinusSpecifier, Minus1)

ITEST(StringSpecifier, StringSpec0)
ITEST(StringSpecifier, StringSpec1)
ITEST(StringSpecifier, StringSpec2)
ITEST(StringSpecifier, StringSpec3)
ITEST(StringSpecifier, StringSpec4)
ITEST(StringSpecifier, StringSpec5)
ITEST(StringSpecifier, StringSpec6)
ITEST(StringSpecifier, StringSpec7)
ITEST(StringSpecifier, StringSpec8)
ITEST(StringSpecifier, StringSpec9)
ITEST(StringSpecifier, StringSpec10)
ITEST(StringSpecifier, StringSpec11)
ITEST(StringSpecifier, StringSpec12)
ITEST(StringSpecifier, StringSpec13)
ITEST(StringSpecifier, StringSpec14)
ITEST(StringSpecifier, StringSpec15)
ITEST(StringSpecifier, StringSpec16)
ITEST(StringSpecifier, StringSpec17)
ITEST(StringSpecifier, StringSpec18)
ITEST(StringSpecifier, StringSpec19)
ITEST(StringSpecifier, StringSpec20)
ITEST(StringSpecifier, StringSpec21)
ITEST(StringSpecifier, StringSpec22)
ITEST(StringSpecifier, StringSpec23)
ITEST(StringSpecifier, StringSpec24)
ITEST(StringSpecifier, StringSpec25)

/*#include "specifiers/array_specifier.hpp"
#include "specifiers/delim_specifier.hpp"
#include "specifiers/group_specifiers.hpp"
#include "specifiers/enum_specifier.hpp" // Relies on group_specifiers.
#include "specifiers/numeric_specifier.hpp"
#include "specifiers/other_specifiers.hpp"
#include "specifiers/player_specifier.hpp"
#include "specifiers/simple_specifiers.hpp"
#include "specifiers/string_specifier.hpp"
#include "specifiers/trivial_specifiers.hpp"*/

ITEST(LiteralSpecifier, LitSpec0a)
ITEST(LiteralSpecifier, LitSpec0b)
ITEST(LiteralSpecifier, LitSpec1)
ITEST(LiteralSpecifier, LitSpec2)
ITEST(LiteralSpecifier, LitSpec3)
ITEST(LiteralSpecifier, LitSpec4)
ITEST(LiteralSpecifier, LitSpec5)
ITEST(LiteralSpecifier, LitSpec6)
ITEST(LiteralSpecifier, LitSpec7)
ITEST(LiteralSpecifier, LitSpec8)

ITEST(TrivialSpecifier, Trivial1)
ITEST(TrivialSpecifier, Trivial2a)
ITEST(TrivialSpecifier, Trivial2b)
ITEST(TrivialSpecifier, Trivial3a)
ITEST(TrivialSpecifier, Trivial3b)
ITEST(TrivialSpecifier, Trivial3c)
ITEST(TrivialSpecifier, Trivial3d)
ITEST(TrivialSpecifier, Trivial3e)
ITEST(TrivialSpecifier, Trivial4)
ITEST(TrivialSpecifier, Trivial5a)
ITEST(TrivialSpecifier, Trivial5b)
ITEST(TrivialSpecifier, Trivial5c)
ITEST(TrivialSpecifier, Trivial5d)
ITEST(TrivialSpecifier, Trivial6a)
ITEST(TrivialSpecifier, Trivial6b)
ITEST(TrivialSpecifier, Trivial6c)
ITEST(TrivialSpecifier, Trivial6d)

ITEST(TrivialSpecifier, Trivial7a)
ITEST(TrivialSpecifier, Trivial7b)
ITEST(TrivialSpecifier, Trivial7c)

ITEST(TrivialSpecifier, Trivial81)

TEST(TrueAlts1, {
	Specifier * spec; gParser.Compile(S"i|c", &spec);
	cell data[3] = { 42, '#', 0 }; TestMemory tm(data, 3); Environment env(&tm);
	return spec->Run(S"42", env) == OK;
});

TEST(TrueAlts2, {
	Specifier * spec; gParser.Compile(S"i|c", &spec);
	cell data[3] = { 42, '#', 1 }; TestMemory tm(data, 3); Environment env(&tm);
	return spec->Run(S"#", env) == OK;
});

TEST(TrueAlts3, {
	Specifier * spec; gParser.Compile(S"i|c", &spec);
	cell data[3] = { 42, '#', 1 }; TestMemory tm(data, 3); Environment env(&tm);
	return spec->Run(S"'#'", env) == OK;
});

// TEST(TrueAlts4, {
	// Specifier * spec; gParser.Compile(S"i|c", &spec);
	// cell data[3] = { 42, '#', 1 }; TestMemory tm(data, 3); Environment env(&tm);
	// return spec->Run(S"'#'", env) == OK;
// });

TEST(TrueAlts5, {
	// Alts with matching prefixes.
	Specifier * spec; gParser.Compile(S"ix|ic", &spec);
	cell data[5] = { 70, 0, 70, 'g', 1 }; TestMemory tm(data, 5); Environment env(&tm);
	return spec->Run(S"70 g", env) == OK;
});

// Test data rewinding.

TEST(TrueAlts6, {
	// Alts with matching prefixes.
	Specifier * spec; gParser.Compile(S"5i|5x", &spec);
	cell data[11] = { 11, 22, 33, 0, 0, 0x11, 0x22, 0x33, 0xFF, 0xAA, 1 }; TestMemory tm(data, 11); Environment env(&tm);
	return spec->Run(S"11 22 33 FF AA", env) == OK;
});


ITEST(StringSpecifier, StringSRun00)
ITEST(StringSpecifier, StringSRun01)
ITEST(StringSpecifier, StringSRun02)
ITEST(StringSpecifier, StringSRun03)
ITEST(StringSpecifier, StringSRun04)
ITEST(StringSpecifier, StringSRun05)
ITEST(StringSpecifier, StringSRun06)
ITEST(StringSpecifier, StringSRun07)

ITEST(StringSpecifier, StringSRun08)
ITEST(StringSpecifier, StringSRun09)
ITEST(StringSpecifier, StringSRun10)
ITEST(StringSpecifier, StringSRun11)
ITEST(StringSpecifier, StringSRun12)
ITEST(StringSpecifier, StringSRun13)
ITEST(StringSpecifier, StringSRun14)
ITEST(StringSpecifier, StringSRun15)
ITEST(StringSpecifier, StringSRun16)
ITEST(StringSpecifier, StringSRun17)
ITEST(StringSpecifier, StringSRun18)
ITEST(StringSpecifier, StringSRun19)

