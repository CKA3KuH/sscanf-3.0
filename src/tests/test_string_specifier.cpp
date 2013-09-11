#include "../specifiers/string_specifier.h"

TEST(StringSpec0,  { StringSpecifier sts; return sts.ReadToken(S"") == ERROR_EXPECTED_A_GOT_B_2; })
TEST(StringSpec1,  { StringSpecifier sts; return sts.ReadToken(S"q") == ERROR_EXPECTED_A_GOT_B_2; })
TEST(StringSpec2,  { StringSpecifier sts; return sts.ReadToken(S"s") == ERROR_NO_ARRAY_START; })
TEST(StringSpec3,  { StringSpecifier sts; return sts.ReadToken(S"s]") == ERROR_NO_ARRAY_START; })
TEST(StringSpec4,  { StringSpecifier sts; return sts.ReadToken(S"s[") == ERROR_NO_ARRAY_END; })
TEST(StringSpec5,  { StringSpecifier sts; return sts.ReadToken(S"s[]") == OK && sts.m_ssize == 0; })
TEST(StringSpec6,  { StringSpecifier sts; return sts.ReadToken(S"s[1]") == OK && sts.m_ssize == 1; })
TEST(StringSpec7,  { StringSpecifier sts; return sts.ReadToken(S"s[10]") == OK && sts.m_ssize == 10; })
TEST(StringSpec8,  { StringSpecifier sts; return sts.ReadToken(S"s[10]") == OK && sts.m_last == true; })
TEST(StringSpec9,  { StringSpecifier sts; return sts.ReadToken(S"s[10] ") == OK && sts.m_last == false; })
TEST(StringSpec10, { StringSpecifier sts; return sts.ReadToken(S"s[-] ") == ERROR_NAN; })
TEST(StringSpec11, { StringSpecifier sts; return sts.ReadToken(S"s[e] ") == ERROR_NAN; })
TEST(StringSpec12, { StringSpecifier sts; return sts.ReadToken(S"s[1k] ") == ERROR_NAN; })
TEST(StringSpec13, { StringSpecifier sts; return sts.ReadToken(S"s[0] ") == ERROR_INVALID_ARRAY_SIZE; })
TEST(StringSpec14, { StringSpecifier sts; return sts.ReadToken(S"s[-84] ") == ERROR_INVALID_ARRAY_SIZE; })
TEST(StringSpec15, { StringSpecifier sts; return sts.ReadToken(S"s[*]") == OK && sts.m_ssize == -1; })
TEST(StringSpec16, { StringSpecifier sts; return sts.ReadToken(S"s [ ] ") == OK; })
TEST(StringSpec17, { StringSpecifier sts; return sts.ReadToken(S"S[1]") == ERROR_NO_DEFAULT_START; })
TEST(StringSpec18, { StringSpecifier sts; return sts.ReadToken(S"S([1]") == ERROR_NO_DEFAULT_END; })
TEST(StringSpec19, { StringSpecifier sts; return sts.ReadToken(S"S([1])") == ERROR_NO_ARRAY_START; })
TEST(StringSpec20, { StringSpecifier sts; return sts.ReadToken(S"S(\\(d)[3]") == OK; })
TEST(StringSpec21, { StringSpecifier sts; return sts.ReadToken(S"S(())[4]") == OK; })


TEST(StringSpec22, { StringSpecifier sts; return sts.ReadToken(S"S(  hello there  )[7]") == OK && sts.m_ssize == 7 && sts.m_length == 11 && TestCompare("hello there", sts.m_default); })
TEST(StringSpec23, { StringSpecifier sts; return sts.ReadToken(S"S(  hello \\)  )[8]") == OK && sts.m_ssize == 8 && sts.m_length == 7 && TestCompare("hello )", sts.m_default); })
TEST(StringSpec24, { StringSpecifier sts; return sts.ReadToken(S"S(  \\\"  )[9]") == OK && sts.m_ssize == 9 && sts.m_length == 1 && TestCompare("\"", sts.m_default); })

TEST(StringSpec25, { StringSpecifier sts; return sts.ReadToken(S"s[4]") == OK && sts.m_default == nullptr && sts.m_length == 0; })


TEST(StringSRun00, { StringSpecifier sts; sts.ReadToken(S"s[20]");
	cell dest[20];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hello", e) == OK;
})
TEST(StringSRun01, { StringSpecifier sts; sts.ReadToken(S"s[20]");
	cell dest[20];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hello", e) == OK && TestCompare("hello", dest);
})
TEST(StringSRun02, { StringSpecifier sts; sts.ReadToken(S"s[20]");
	cell dest[20];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hello there", e) == OK && TestCompare("hello there", dest);
})
TEST(StringSRun03, { StringSpecifier sts; sts.ReadToken(S"s[20] ");
	cell dest[20];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hello there", e) == OK && TestCompare("hello", dest);
})
TEST(StringSRun04, { StringSpecifier sts; sts.ReadToken(S"s[20]");
	cell dest[20];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hel\\(lo there  ", e) == OK && TestCompare("hel(lo there", dest);
})
TEST(StringSRun05, { StringSpecifier sts; sts.ReadToken(S"s[20] ");
	cell dest[20];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hello\\ there", e) == OK && TestCompare("hello there", dest);
})
TEST(StringSRun06, { StringSpecifier sts; sts.ReadToken(S"s[10]");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hello there   ", e) == OK && TestCompare("hello the", dest);
})
TEST(StringSRun07, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
})



TEST(StringSRun08, { StringSpecifier sts; sts.ReadToken(S"s[*] ");
	cell dest[11] = {10};
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest + 1);
})
TEST(StringSRun09, { StringSpecifier sts; sts.ReadToken(S"s[*]");
	cell dest[11] = {10};
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hello there   ", e) == OK && TestCompare("hello the", dest + 1);
})
TEST(StringSRun10, { StringSpecifier sts; sts.ReadToken(S"s[*] ");
	cell dest[11] = {5};
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hell", dest + 1);
})
TEST(StringSRun11, { StringSpecifier sts; sts.ReadToken(S"s[] ");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == ERROR_INVALID_ARRAY_SIZE;
})
TEST(StringSRun12, { StringSpecifier sts; sts.ReadToken(S"s[*] ");
	cell dest[11] = {-5};
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == ERROR_INVALID_ARRAY_SIZE;
})
TEST(StringSRun13, { StringSpecifier sts; sts.ReadToken(S"s[*] ");
	cell dest[11] = {0};
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == ERROR_INVALID_ARRAY_SIZE;
})
TEST(StringSRun14, { StringSpecifier sts; sts.ReadToken(S"s[*] ");
	cell dest[11] = {1, 42};
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && dest[1] == '\0';
})



TEST(StringSRun15, { StringSpecifier sts; sts.ReadToken(S"S(fine)[5] ");
	cell dest[5];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"", e) == OK && TestCompare("fine", dest);
})
TEST(StringSRun16, { StringSpecifier sts; sts.ReadToken(S"S(bad)[] ");
	cell dest[11];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"", e) == ERROR_INVALID_ARRAY_SIZE;
})
TEST(StringSRun17, { StringSpecifier sts; sts.ReadToken(S"S(longer)[5]");
	cell dest[5];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"", e) == OK && TestCompare("long", dest);
})
TEST(StringSRun18, { StringSpecifier sts; sts.ReadToken(S"S(has space)[*]");
	cell dest[11] = {10};
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"", e) == OK && TestCompare("has space", dest + 1);
})
TEST(StringSRun19, { StringSpecifier sts; sts.ReadToken(S"S(has space)[*] ");
	cell dest[11] = {10};
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"", e) == OK && TestCompare("has space", dest + 1);
})




TEST(StringSRun20, { StringSpecifier sts; sts.ReadToken(S"s[20]");
	cell dest[20];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hello there   ", e) == OK && TestCompare("hello there", dest);
})



/*	TEST(StringSRun13, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
})
TEST(StringSRun14, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
})
TEST(StringSRun15, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
})
TEST(StringSRun16, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
})
TEST(StringSRun17, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
})
TEST(StringSRun18, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
})
TEST(StringSRun19, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
})
TEST(StringSRun20, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
})
TEST(StringSRun21, { StringSpecifier sts; sts.ReadToken(S"s[10] ");
	cell dest[10];
	TempMemory mem(dest, sizeof (dest) / sizeof (cell));
	Environment e(&mem);
	return sts.Run(S"hellothere   ", e) == OK && TestCompare("hellother", dest);
})*/
//TEST(StringSpec26, { StringSpecifier sts; return sts.ReadToken(S"S(  \\\"  )[4]") == OK && TestCompare("\"", sts.m_default); })
//TEST(StringSpec27, { StringSpecifier sts; return sts.ReadToken(S"S(  \\\"  )[4]") == OK && TestCompare("\"", sts.m_default); })

