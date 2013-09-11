#include "../specifiers/trivial_specifiers.h"

TEST(Trivial1,  { TrivialSpecifier that('|'); return that.ReadToken(S"|") == OK; })
TEST(Trivial2a, { TrivialSpecifier that('|'); that.ReadToken(S"|"); ss s; that.Render(s); return s.str() == "|"; })
TEST(Trivial2b, { TrivialSpecifier that('|'); that.ReadToken(S"|"); ss s; return dynamic_cast<ss &>(s << that).str() == "|"; })
TEST(Trivial3a, { TrivialSpecifier that('|'); ss s; that.Render(s); return s.str() == "|"; })
TEST(Trivial3b, { TrivialSpecifier that('}'); ss s; that.Render(s); return s.str() == "}"; })
TEST(Trivial3c, { TrivialSpecifier that(')'); ss s; that.Render(s); return s.str() == ")"; })
TEST(Trivial3d, { TrivialSpecifier that('?'); ss s; that.Render(s); return s.str() == "?"; })
TEST(Trivial3e, { TrivialSpecifier that('>'); ss s; that.Render(s); return s.str() == ">"; })
TEST(Trivial4,  { TrivialSpecifier that('|'); Specifier * two; return that.Clone(&two) == OK && &that != two; })
TEST(Trivial5a, { TrivialSpecifier that('#'); return !that.GetOptional(); })
TEST(Trivial5b, { TrivialSpecifier that('~'); return !that.GetSkip(); })
TEST(Trivial5c, { TrivialSpecifier that('~'); that.SetSkip(); return that.GetSkip(); })
TEST(Trivial5d, { TrivialSpecifier that('~'); that.SetOptional(); return that.GetOptional(); })
TEST(Trivial6a, { TrivialSpecifier that('X'); return that.GetSpecifier() == 'X'; })
TEST(Trivial6b, { TrivialSpecifier that('2'); return that.GetSpecifier() == '2'; })
TEST(Trivial6c, { TrivialSpecifier that('q'); return that.GetSpecifier() == 'q'; })
TEST(Trivial6d, { TrivialSpecifier that('+'); return that.GetSpecifier() == '+'; })

TEST(Trivial7a, { TrivialSpecifier that('+'); that.ReadToken(S"+"); return *CUR == '\0'; })
TEST(Trivial7b, { TrivialSpecifier that('+'); that.ReadToken(S"++"); return *CUR == '+'; })
TEST(Trivial7c, { TrivialSpecifier that('+'); that.ReadToken(S"+|"); return *CUR == '|'; })

TEST(Trivial81, { TrivialSpecifier that('+'); return that.Run(S"+|", DefaultEnvironment::Get(nullptr)) == ERROR_RAN_TRIVIAL; })

