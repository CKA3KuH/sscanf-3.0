#include "../specifiers/group_specifiers.h"

TEST(AltGroup1,  { AltGroup gg; return gg.ReadToken(S"(ii|dd)") == OK && *CUR == '\0'; })
TEST(AltGroup2,  { AltGroup gg; gg.ReadToken(S"(ii|xh)");ss s; return dynamic_cast<ss &>(s << gg).str() == "(ii|xh)"; })
TEST(AltGroup4,  { AltGroup gg; if (gg.ReadToken(S"(ii|dd|cc|xx)") != OK) return false;
	int i = 0; for (auto b = gg.Begin(), e = gg.End(); b != e; ++b) ++i; return i == 4 && gg.CountChildren() == 4; })
TEST(AltGroup5,  { AltGroup gg; if (gg.ReadToken(S"(ii|dd|cc|xx|dd|dd|dd)") != OK) return false;
	int i = 0; for (auto b = gg.Begin(), e = gg.End(); b != e; ++b) { ++i; if ((*b)->CountChildren() != 2) return false; } return i == 7 && gg.CountChildren() == 7; })
TEST(AltGroup6,  { AltGroup gg; if (gg.ReadToken(S"(ii)") != OK) return false;
	int i = 0; for (auto b = gg.Begin(), e = gg.End(); b != e; ++b) ++i; return i == 1 && gg.CountChildren() == 1; })

TEST(GlobGroup1,  { AltGroup gg(false); return gg.ReadToken(S"ii|dd") == OK && *CUR == '\0'; })
TEST(GlobGroup2,  { AltGroup gg(false); return gg.ReadToken(S"ii|") == ERROR_NO_CHILDREN; })
TEST(GlobGroup3,  { AltGroup gg(false); return gg.ReadToken(S"|dd") == ERROR_NO_CHILDREN; })
TEST(GlobGroup6,  { AltGroup gg(false); return gg.ReadToken(S"I(5)") == OK; })
TEST(GlobGroup7,  { AltGroup gg(false); return gg.ReadToken(S"I(q)") == ERROR_NAN; })
TEST(GlobGroup4,  { AltGroup gg(false); return gg.ReadToken(S"ii|D(5)|cc|  H(11)n") == OK && *CUR == '\0'; })
TEST(GlobGroup5,  { AltGroup gg(false); gg.ReadToken(S"ii|xh");ss s; return dynamic_cast<ss &>(s << gg).str() == "ii|xh"; })

