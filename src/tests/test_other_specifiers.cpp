#include "../specifiers/group_specifiers.h"

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
