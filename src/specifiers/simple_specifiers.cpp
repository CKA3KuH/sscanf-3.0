#include "simple_specifiers.h"

TEST(Range0,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"5 - 7", 1, &l, &h) == OK && l == 5 && h == 7; })
TEST(Range1,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"-5 - 7", 2, &l, &h) == OK && l == -5 && h == 7; })
TEST(Range2,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"5 - -7", 2, &l, &h) == ERROR_INVALID_RANGE; })
TEST(Range3,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"-7 - -5", 3, &l, &h) == OK && l == -7 && h == -5; })

TEST(Range4,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"- 5", 1, &l, &h) == OK && l == 0x80000000 && h == 5; })
TEST(Range5,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"7 -", 1, &l, &h) == OK && l == 7 && h == 0x7FFFFFFF; })

TEST(Range6,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"- -5", 2, &l, &h) == OK && l == 0x80000000 && h == -5; })
TEST(Range7,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"-7 -", 2, &l, &h) == OK && l == -7 && h == 0x7FFFFFFF; })
TEST(Range8,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"-7 - 5", 2, &l, &h) == OK && l == -7 && h == 5; })
TEST(Range9,  { SimpleSpecifier s('i', &Utils::ReadDecimal); cell l = 0x80000000, h = 0x7FFFFFFF; return GetRanges(&s, S"-7 - -5", 3, &l, &h) == OK && l == -7 && h == -5; })

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

ITEST(SimpleSpecifier, Simple1a)
ITEST(SimpleSpecifier, Simple1b)
ITEST(SimpleSpecifier, Simple1c)
ITEST(SimpleSpecifier, Simple1d)
ITEST(SimpleSpecifier, Simple1e)
ITEST(SimpleSpecifier, Simple1f)
ITEST(SimpleSpecifier, Simple1g)
ITEST(SimpleSpecifier, Simple1h)
ITEST(SimpleSpecifier, Simple1i)
ITEST(SimpleSpecifier, Simple1j)
ITEST(SimpleSpecifier, Simple1k)
ITEST(SimpleSpecifier, Simple1l)
ITEST(SimpleSpecifier, Simple1m)
ITEST(SimpleSpecifier, Simple1n)
ITEST(SimpleSpecifier, Simple1o)
ITEST(SimpleSpecifier, Simple1p)
ITEST(SimpleSpecifier, Simple1q)
ITEST(SimpleSpecifier, Simple1r)
ITEST(SimpleSpecifier, Simple1s)
ITEST(SimpleSpecifier, Simple1t)
ITEST(SimpleSpecifier, Simple1u)
ITEST(SimpleSpecifier, Simple1v)
ITEST(SimpleSpecifier, Simple1w)

ITEST(SimpleSpecifier, Simple2a)
ITEST(SimpleSpecifier, Simple2b)
ITEST(SimpleSpecifier, Simple2c)

ITEST(SimpleSpecifier, Simple3a)
ITEST(SimpleSpecifier, Simple3b)
ITEST(SimpleSpecifier, Simple3c)
ITEST(SimpleSpecifier, Simple3f)
ITEST(SimpleSpecifier, Simple3g)

/*ITEST(SimpleSpecifier, Simple4a)
ITEST(SimpleSpecifier, Simple4b)
ITEST(SimpleSpecifier, Simple4c)
ITEST(SimpleSpecifier, Simple4d)
ITEST(SimpleSpecifier, Simple4e)
ITEST(SimpleSpecifier, Simple4f)
ITEST(SimpleSpecifier, Simple4g)
ITEST(SimpleSpecifier, Simple4h)
ITEST(SimpleSpecifier, Simple4i)*/
// ITEST(SimpleSpecifier, Simple4j)

ITEST(SimpleSpecifier, Simple5a)

ITEST(SimpleSpecifier, Simple9a)
ITEST(SimpleSpecifier, Simple9b)
ITEST(SimpleSpecifier, Simple9c)
ITEST(SimpleSpecifier, Simple9h)
ITEST(SimpleSpecifier, Simple9d)
ITEST(SimpleSpecifier, Simple9e)
ITEST(SimpleSpecifier, Simple9i)
ITEST(SimpleSpecifier, Simple9l)
ITEST(SimpleSpecifier, Simple9f)
ITEST(SimpleSpecifier, Simple9j)
ITEST(SimpleSpecifier, Simple9g)
ITEST(SimpleSpecifier, Simple9k)

ITEST(SimpleSpecifier, SimpleP00)
ITEST(SimpleSpecifier, SimpleP01)
ITEST(SimpleSpecifier, SimpleP02)
ITEST(SimpleSpecifier, SimpleP03)
ITEST(SimpleSpecifier, SimpleP04)
ITEST(SimpleSpecifier, SimpleP05)
ITEST(SimpleSpecifier, SimpleP06)
ITEST(SimpleSpecifier, SimpleP07)
ITEST(SimpleSpecifier, SimpleP08)
ITEST(SimpleSpecifier, SimpleP09)

ITEST(SimpleSpecifier, Simple6a)
ITEST(SimpleSpecifier, Simple6b)

// ITEST(OptionSpecifier, Opt1)
// ITEST(OptionSpecifier, Opt2)
// ITEST(OptionSpecifier, Opt3)
// ITEST(OptionSpecifier, Opt4)

ITEST(SimpleSpecifier, SimpleH00)
ITEST(SimpleSpecifier, SimpleH01)
ITEST(SimpleSpecifier, SimpleH02)
ITEST(SimpleSpecifier, SimpleH03)
ITEST(SimpleSpecifier, SimpleH04)
ITEST(SimpleSpecifier, SimpleH05)
ITEST(SimpleSpecifier, SimpleH06)
ITEST(SimpleSpecifier, SimpleH07)
ITEST(SimpleSpecifier, SimpleH08)
ITEST(SimpleSpecifier, SimpleH09)
ITEST(SimpleSpecifier, SimpleH10)
ITEST(SimpleSpecifier, SimpleH11)
ITEST(SimpleSpecifier, SimpleH12)
ITEST(SimpleSpecifier, SimpleH13)
ITEST(SimpleSpecifier, SimpleH14)
ITEST(SimpleSpecifier, SimpleH15)

ITEST(SimpleSpecifier, SimpleI00)
ITEST(SimpleSpecifier, SimpleI01)
ITEST(SimpleSpecifier, SimpleI02)
ITEST(SimpleSpecifier, SimpleI03)
ITEST(SimpleSpecifier, SimpleI04)
ITEST(SimpleSpecifier, SimpleI05)
ITEST(SimpleSpecifier, SimpleI06)
ITEST(SimpleSpecifier, SimpleI07)
ITEST(SimpleSpecifier, SimpleI08)
ITEST(SimpleSpecifier, SimpleI09)
ITEST(SimpleSpecifier, SimpleI10)
ITEST(SimpleSpecifier, SimpleI11)
ITEST(SimpleSpecifier, SimpleI12)
ITEST(SimpleSpecifier, SimpleI13)
ITEST(SimpleSpecifier, SimpleI14)
ITEST(SimpleSpecifier, SimpleI15)

ITEST(SimpleSpecifier, SimpleO00)
ITEST(SimpleSpecifier, SimpleO01)
ITEST(SimpleSpecifier, SimpleO02)
ITEST(SimpleSpecifier, SimpleO03)
ITEST(SimpleSpecifier, SimpleO04)
ITEST(SimpleSpecifier, SimpleO05)
ITEST(SimpleSpecifier, SimpleO06)
ITEST(SimpleSpecifier, SimpleO07)
ITEST(SimpleSpecifier, SimpleO08)
ITEST(SimpleSpecifier, SimpleO09)
ITEST(SimpleSpecifier, SimpleO10)
ITEST(SimpleSpecifier, SimpleO11)
ITEST(SimpleSpecifier, SimpleO12)
ITEST(SimpleSpecifier, SimpleO13)
ITEST(SimpleSpecifier, SimpleO14)
ITEST(SimpleSpecifier, SimpleO15)

ITEST(SimpleSpecifier, SimpleB00)
ITEST(SimpleSpecifier, SimpleB01)
ITEST(SimpleSpecifier, SimpleB02)
ITEST(SimpleSpecifier, SimpleB03)
ITEST(SimpleSpecifier, SimpleB04)
ITEST(SimpleSpecifier, SimpleB05)
ITEST(SimpleSpecifier, SimpleB06)
ITEST(SimpleSpecifier, SimpleB07)
ITEST(SimpleSpecifier, SimpleB08)
ITEST(SimpleSpecifier, SimpleB09)
ITEST(SimpleSpecifier, SimpleB10)
ITEST(SimpleSpecifier, SimpleB11)
ITEST(SimpleSpecifier, SimpleB12)
ITEST(SimpleSpecifier, SimpleB13)
ITEST(SimpleSpecifier, SimpleB14)
ITEST(SimpleSpecifier, SimpleB15)

ITEST(SimpleSpecifier, SimpleC00)
ITEST(SimpleSpecifier, SimpleC01)
ITEST(SimpleSpecifier, SimpleC02)
ITEST(SimpleSpecifier, SimpleC03)
ITEST(SimpleSpecifier, SimpleC04)
ITEST(SimpleSpecifier, SimpleC05)
ITEST(SimpleSpecifier, SimpleC06)
ITEST(SimpleSpecifier, SimpleC07)
ITEST(SimpleSpecifier, SimpleC08)
ITEST(SimpleSpecifier, SimpleC09)
ITEST(SimpleSpecifier, SimpleC10)
ITEST(SimpleSpecifier, SimpleC11)
ITEST(SimpleSpecifier, SimpleC12)
ITEST(SimpleSpecifier, SimpleC13)
ITEST(SimpleSpecifier, SimpleC14)
ITEST(SimpleSpecifier, SimpleC15)

ITEST(SimpleSpecifier, Simplec00)
ITEST(SimpleSpecifier, Simplec01)
ITEST(SimpleSpecifier, Simplec02)
ITEST(SimpleSpecifier, Simplec03)
ITEST(SimpleSpecifier, Simplec04)
ITEST(SimpleSpecifier, Simplec05)
ITEST(SimpleSpecifier, Simplec06)
ITEST(SimpleSpecifier, Simplec07)
ITEST(SimpleSpecifier, Simplec08)
ITEST(SimpleSpecifier, Simplec09)
ITEST(SimpleSpecifier, Simplec10)
ITEST(SimpleSpecifier, Simplec11)
ITEST(SimpleSpecifier, Simplec12)
ITEST(SimpleSpecifier, Simplec13)
ITEST(SimpleSpecifier, Simplec14)
ITEST(SimpleSpecifier, Simplec15)

/*ITEST(SimpleSpecifier, SimpleH16)
ITEST(SimpleSpecifier, SimpleH17)
ITEST(SimpleSpecifier, SimpleH18)
ITEST(SimpleSpecifier, SimpleH19)
ITEST(SimpleSpecifier, SimpleH20)
ITEST(SimpleSpecifier, SimpleH21)
ITEST(SimpleSpecifier, SimpleH22)
ITEST(SimpleSpecifier, SimpleH23)
ITEST(SimpleSpecifier, SimpleH24)
ITEST(SimpleSpecifier, SimpleH25)
ITEST(SimpleSpecifier, SimpleH26)
ITEST(SimpleSpecifier, SimpleH27)
ITEST(SimpleSpecifier, SimpleH28)
ITEST(SimpleSpecifier, SimpleH29)
ITEST(SimpleSpecifier, SimpleH30)
ITEST(SimpleSpecifier, SimpleH31)*/


ITEST(SimpleSpecifier, NumXm)
ITEST(SimpleSpecifier, NumXn)
ITEST(SimpleSpecifier, NumXo)
ITEST(SimpleSpecifier, NumXp)
ITEST(SimpleSpecifier, NumXq)
ITEST(SimpleSpecifier, NumXr)
ITEST(SimpleSpecifier, NumXs)
ITEST(SimpleSpecifier, NumXt)
ITEST(SimpleSpecifier, NumXu)

