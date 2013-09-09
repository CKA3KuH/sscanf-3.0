#pragma once

#include "../specifiers.h"

class TrivialSpecifier : public Specifier
{
public:
	// cons
		TrivialSpecifier(char c)
	:
		Specifier(c)
	{
	};
	
	CLONE();
	
	virtual error_t
		ReadToken(char const * & input)
	{
		++input;
		return OK;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		// This shouldn't ever by run.
		return ERROR_RAN_TRIVIAL;
	};
	
	// cons
		TrivialSpecifier(TrivialSpecifier const & that)
	:
		Specifier(that)
	{
	};
	
	// dest
		~TrivialSpecifier()
	{
	};
	
private:
	// This doesn't really have many failure modes - it is "trivial" after all!
	CTEST(Trivial1,  { TrivialSpecifier that('|'); return that.ReadToken(S"|") == OK; })
	CTEST(Trivial2a, { TrivialSpecifier that('|'); that.ReadToken(S"|"); ss s; that.Render(s); return s.str() == "|"; })
	CTEST(Trivial2b, { TrivialSpecifier that('|'); that.ReadToken(S"|"); ss s; return dynamic_cast<ss &>(s << that).str() == "|"; })
	CTEST(Trivial3a, { TrivialSpecifier that('|'); ss s; that.Render(s); return s.str() == "|"; })
	CTEST(Trivial3b, { TrivialSpecifier that('}'); ss s; that.Render(s); return s.str() == "}"; })
	CTEST(Trivial3c, { TrivialSpecifier that(')'); ss s; that.Render(s); return s.str() == ")"; })
	CTEST(Trivial3d, { TrivialSpecifier that('?'); ss s; that.Render(s); return s.str() == "?"; })
	CTEST(Trivial3e, { TrivialSpecifier that('>'); ss s; that.Render(s); return s.str() == ">"; })
	CTEST(Trivial4,  { TrivialSpecifier that('|'); Specifier * two; return that.Clone(&two) == OK && &that != two; })
	CTEST(Trivial5a, { TrivialSpecifier that('#'); return !that.GetOptional(); })
	CTEST(Trivial5b, { TrivialSpecifier that('~'); return !that.GetSkip(); })
	CTEST(Trivial5c, { TrivialSpecifier that('~'); that.SetSkip(); return that.GetSkip(); })
	CTEST(Trivial5d, { TrivialSpecifier that('~'); that.SetOptional(); return that.GetOptional(); })
	CTEST(Trivial6a, { TrivialSpecifier that('X'); return that.GetSpecifier() == 'X'; })
	CTEST(Trivial6b, { TrivialSpecifier that('2'); return that.GetSpecifier() == '2'; })
	CTEST(Trivial6c, { TrivialSpecifier that('q'); return that.GetSpecifier() == 'q'; })
	CTEST(Trivial6d, { TrivialSpecifier that('+'); return that.GetSpecifier() == '+'; })
	
	CTEST(Trivial7a, { TrivialSpecifier that('+'); that.ReadToken(S"+"); return *CUR == '\0'; })
	CTEST(Trivial7b, { TrivialSpecifier that('+'); that.ReadToken(S"++"); return *CUR == '+'; })
	CTEST(Trivial7c, { TrivialSpecifier that('+'); that.ReadToken(S"+|"); return *CUR == '|'; })
	
	CTEST(Trivial81, { TrivialSpecifier that('+'); return that.Run(S"+|", DefaultEnvironment::Get(nullptr)) == ERROR_RAN_TRIVIAL; })
};

