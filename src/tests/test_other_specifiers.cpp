#include "../specifiers/group_specifiers.h"
#include "../specifiers/specnum_specifiers.h"

TEST(LitSpec0a, { LiteralSpecifier ls; return ls.ReadToken(S"'hello") == ERROR_NO_STRING_END; })
TEST(LitSpec0b, { LiteralSpecifier ls; return ls.ReadToken(S"'   hello   \\'") == ERROR_NO_STRING_END; })
TEST(LitSpec1,  { LiteralSpecifier ls; return ls.ReadToken(S"'hello'") == OK && ls.Run(S"hello there", DefaultEnvironment::Get(nullptr)) == OK; })
TEST(LitSpec2,  { LiteralSpecifier ls; return ls.ReadToken(S"'hello'") == OK && ls.Run(S"there", DefaultEnvironment::Get(nullptr)) == ERROR_NO_STRING_MATCH; })
TEST(LitSpec3,  { LiteralSpecifier ls; return ls.ReadToken(S"'   hello'") == OK && ls.Run(S"hello there", DefaultEnvironment::Get(nullptr)) == OK; })
TEST(LitSpec4,  { LiteralSpecifier ls; return ls.ReadToken(S"'  hello  '") == OK && ls.Run(S"hello there", DefaultEnvironment::Get(nullptr)) == OK; })
TEST(LitSpec5,  { LiteralSpecifier ls; return ls.ReadToken(S"'  hello  '") == OK && ls.Run(S" therehello", DefaultEnvironment::Get(nullptr)) == OK; })
TEST(LitSpec6,  { LiteralSpecifier ls; return ls.ReadToken(S"'  hello  '") == OK && ls.Run(S"woop helloSS", DefaultEnvironment::Get(nullptr)) == OK && *CUR == 'S'; })
TEST(LitSpec7,  { LiteralSpecifier ls; return ls.ReadToken(S"'  hello  '") == OK && ls.Run(S"helhello__Q", DefaultEnvironment::Get(nullptr)) == OK && *CUR == '_'; })
TEST(LitSpec8,  { LiteralSpecifier ls; return ls.ReadToken(S"'  hello  '") == OK && ls.Run(S"hell", DefaultEnvironment::Get(nullptr)) == ERROR_NO_STRING_MATCH; })

// TEST(Opt1, { OptionSpecifier opt; return opt.ReadToken(S"?<MOO>") == OK && !strcmp(opt.m_option, "MOO"); })
// TEST(Opt2, { OptionSpecifier opt; return opt.ReadToken(S"?<HAHAH=56>") == OK && !strcmp(opt.m_option, "HAHAH") && opt.m_value == 56; })
// TEST(Opt3, { OptionSpecifier opt; return opt.ReadToken(S"?") == ERROR_NO_PARAM_START; })
// TEST(Opt4, { OptionSpecifier opt; return opt.ReadToken(S"?<") == ERROR_NO_PARAM_END; })

TEST(Minus0, { MinusSpecifier ms; return ms.ReadToken(S"-i") == OK && ms.GetSpecifier() == 'i' && ms.GetSkip() == true; })
TEST(Minus1, { MinusSpecifier ms; return ms.ReadToken(S"-H(42)") == OK && ms.GetSpecifier() == 'h' && ms.GetSkip() == true; })

TEST(Opt0,  { OptionSpecifier opt; return opt.ReadToken(S"?<READ_NL>") == OK; })


TEST(FloatSpec00, { FloatSpecifier f('f', &Utils::ReadFloat); return f.ReadToken(S"f") == OK; })
TEST(FloatSpec01, { FloatSpecifier f('f', &Utils::ReadFloat); return f.ReadToken(S"F(5.5)") == OK; })
TEST(FloatSpec02, { FloatSpecifier f('f', &Utils::ReadFloat); return f.ReadToken(S"f<0.0 - 7.7>") == OK; })
TEST(FloatSpec03, { FloatSpecifier f('f', &Utils::ReadFloat); return f.ReadToken(S"f<-2.0 - 7.7>") == OK; })
TEST(FloatSpec04, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"F(5.5)") == OK &&
	f.Run(S"", DefaultEnvironment::Get(&r)) == OK &&
	amx_ctof(r) == 5.5; })
TEST(FloatSpec05, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"f") == OK &&
	f.Run(S"", DefaultEnvironment::Get(&r)) == ERROR_NAN; })
TEST(FloatSpec06, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"f") == OK &&
	f.Run(S"-2.9e3", DefaultEnvironment::Get(&r)) == OK &&
	amx_ctof(r) == -2.9e3; })
TEST(FloatSpec07, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"f<.6 - 8e2>") == OK &&
	f.Run(S"3", DefaultEnvironment::Get(&r)) == OK &&
	amx_ctof(r) == 3; })
TEST(FloatSpec08, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"f<-100.0 - -3.7>") == OK &&
	f.Run(S"-82.6", DefaultEnvironment::Get(&r)) == OK &&
	amx_ctof(r) > -82.7 && amx_ctof(r) < -82.5; }) // Float comparisons...
TEST(FloatSpec09, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"f<-100.0 - -3.7>") == OK &&
	f.Run(S"82.6", DefaultEnvironment::Get(&r)) == ERROR_OUT_OF_RANGE; })
TEST(FloatSpec10, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"F<2.0 - 7.0>(11.0)") == OK &&
	f.Run(S"", DefaultEnvironment::Get(&r)) == OK &&
	amx_ctof(r) == 11.0; })
TEST(FloatSpec11, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"F<2.0 - 7.0>(11.0)") == OK &&
	f.Run(S"4.0", DefaultEnvironment::Get(&r)) == OK &&
	amx_ctof(r) == 4.0; })
TEST(FloatSpec12, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"F<2.0 - 7.0>(11.0)") == OK &&
	f.Run(S"2.0", DefaultEnvironment::Get(&r)) == OK &&
	amx_ctof(r) == 2.0; })
TEST(FloatSpec13, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"F<2.0 - 7.0>(11.0)") == OK &&
	f.Run(S"1.9", DefaultEnvironment::Get(&r)) == ERROR_OUT_OF_RANGE; })
TEST(FloatSpec14, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"F<2.0 - 7.0>(11.0)") == OK &&
	f.Run(S"7.0", DefaultEnvironment::Get(&r)) == OK &&
	amx_ctof(r) == 7.0; })
TEST(FloatSpec15, { FloatSpecifier f('f', &Utils::ReadFloat); cell r; return
	f.ReadToken(S"F<2.0 - 7.0>(11.0)") == OK &&
	f.Run(S"7.1", DefaultEnvironment::Get(&r)) == ERROR_OUT_OF_RANGE; })



