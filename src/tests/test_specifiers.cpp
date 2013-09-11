#include "../specifiers/trivial_specifiers.h"
#include "../specifiers/simple_specifiers.h"
#include "../specifiers/other_specifiers.h"
#include "../specifiers/group_specifiers.h"
#include "../specifiers/numeric_specifier.h"
#include "../specifiers/string_specifier.h"

#include "../../sdk/plugin.h"

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

