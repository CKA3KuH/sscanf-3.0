#include "parser.h"
#include "utils.h"
#include "specifiers.h"

#include "specifiers/trivial_specifiers.h"
#include "specifiers/simple_specifiers.h"
#include "specifiers/other_specifiers.h"
#include "specifiers/group_specifiers.h"
#include "specifiers/numeric_specifier.h"

#ifdef SSCANF_DEBUG
	#include "../sdk/plugin.h"
	
	class TestMemory : public Memory
	{
	public:
		// cons
			TestMemory(cell * data, size_t len) : Memory(nullptr)
		{
			pos = 0;
			m_data = (cell *)malloc(len * sizeof (cell));
			memcpy(m_data, data, len * sizeof (cell));
		};
		
		// dest
			~TestMemory()
		{
			delete m_data;
		};
		
		virtual error_t
			GetNextPointer(cell ** const ret) { return OK; }
		
		virtual error_t
			GetNextValue(cell * const ret) { return OK; }
		
		virtual error_t
			GetNextString(char * ret, size_t len) { return OK; }
		
		virtual error_t
			SetNextValue(cell const val, size_t idx = 0)
		{
			if (val != m_data[pos++]) return ERROR_UNKNOWN_ERROR;
			return OK;
		}
		
		virtual error_t
			SetNextString(char const * val, size_t idx = 0, bool pack = false) { return OK; }
		
		virtual error_t
			Skip(size_t n) { pos += n; return OK; }
		
		int
			pos;
		
		cell *
			m_data;
	};
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

ITEST(SimpleSpecifier, Simple4a)
ITEST(SimpleSpecifier, Simple4b)
ITEST(SimpleSpecifier, Simple4c)
ITEST(SimpleSpecifier, Simple4d)
ITEST(SimpleSpecifier, Simple4e)
ITEST(SimpleSpecifier, Simple4f)
ITEST(SimpleSpecifier, Simple4g)
ITEST(SimpleSpecifier, Simple4h)
ITEST(SimpleSpecifier, Simple4i)
//ITEST(SimpleSpecifier, Simple4j)

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

ITEST(SimpleSpecifier, Simple6a)
ITEST(SimpleSpecifier, Simple6b)

ITEST(OptionSpecifier, Opt1)
ITEST(OptionSpecifier, Opt2)
ITEST(OptionSpecifier, Opt3)
ITEST(OptionSpecifier, Opt4)

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






