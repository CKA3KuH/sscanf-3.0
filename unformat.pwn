// This is a comment
// uncomment the line below if you want to write a filterscript
//#define FILTERSCRIPT

#include <a_samp>
#include <sscanf3>

#define RUN(%0,%1,%2) if ((error = unformat(%0,%1,%2)) != OK) printf("FAIL: " #%1 " = %s (%d)", UNFORMAT_GetError(error), _:error)
//#define ASSERT(%0) if (!(%0)) printf("FAIL: " #%0)

#define TEST_TRUE(%0)  Testing_Test(!!%0)
#define TEST_FALSE(%0) Testing_Test(!%0)
#define TEST_NULL(%0)  Testing_Test(0 == %0)
#define TEST_NOT_NULL(%0)  Testing_Test(0 != %0)
#define TEST_N(%0,%1)  Testing_Test(%1 == %0)

#define TEST_TRUE_EX(%0,%2)  Testing_Test(bool:(%0), (%2))
#define TEST_FALSE_EX(%0,%2) Testing_Test(!(%0), (%2))
#define TEST_NULL_EX(%0,%2)  Testing_Test((%0) == 0, (%2))
#define TEST_N_EX(%0,%1,%2)  Testing_Test((%0) == (%1), (%2))

#define TEST_MSG "%s"
#define TEST_FAILED "FAIL:"
#define TEST_PASSED "PASS!"
//#define TEST_FAILED "*** Test Failed:"
//#define TEST_PASSED "*** Test Passed:"

#define _Y_TESTEQ(%0) "\"%0\"")
#define _Y_TESTDQ:_Y_TESTEQ(%0"%1"%2) _Y_TESTDQ:_Y_TESTEQ(%0\x22;%1\x22;%2)
//#define _Y_TESTCB:_Y_TESTDQ:_Y_TESTEQ(%0)%1) _Y_TESTCB:_Y_TESTDQ:_Y_TESTEQ(%0\x29;%1)
#define _Y_TESTCB:_Y_TESTDQ:_Y_TESTEQ(%0)%1) _Y_TESTCB:_Y_TESTDQ:_Y_TESTEQ(%0\x29;%1)
#define _Y_TESTOB:_Y_TESTCB:_Y_TESTDQ:_Y_TESTEQ(%0(%1) _Y_TESTOB:_Y_TESTCB:_Y_TESTDQ:_Y_TESTEQ(%0\x28;%1)

#define ASSERT(%0) printf(TEST_MSG" %s", (%0)?(TEST_PASSED):(TEST_FAILED), _Y_TESTOB:_Y_TESTCB:_Y_TESTDQ:_Y_TESTEQ(%0)
#define ASSERT_TRUE(%0) ASSERT(!!%0) //Testing_Test(!!%0,__name)
#define ASSERT_FALSE(%0) ASSERT(!%0) //Testing_Test(!%0,__name)
#define ASSERT_NULL(%0) ASSERT(0 == %0) //Testing_Test(0==%0,__name)
#define ASSERT_N:%1(%0) ASSERT(%1 == %0) //Testing_Test(%1==%0,__name)

main()
{
	new
		E_SSCANF_ERROR:error,
		var0, var1, var2, var3, str0[32], str1[32], str2[32], str3[32];
	#pragma unused var0, var1, var2, var3, str0, str1, str2, str3
	print("|-----------------------------------|");
	print("|                                   |");
	print("| \"unformat\" Unit Tests by \"Y_Less\" |");
	print("|                                   |");
	print("|-----------------------------------|");
	print(" ");
	print(" - Group 1:");
	print(" ");
	RUN("42 43 44", "iii", var0, var1, var2);
	ASSERT(var0 == 42 && var1 == 43 && var2 == 44);
	RUN("42 43 44", "dox", var0, var1, var2);
	ASSERT(var0 == 42 && var1 == 35 && var2 == 0x44);
	RUN("5 @ q", "ccc", var0, var1, var2);
	ASSERT(var0 == '5' && var1 == '@' && var2 == 'q');
	ASSERT(unformat("44", "I\x28;5", var0) == ERROR_NO_DEFAULT_END);
	unformat("FF FF", "hn");
	
	ASSERT(unformat("99", "i", var0) == OK && var0 == 99);
	ASSERT(unformat("88", "h ", var0) == OK && var0 == 0x88);
	ASSERT(unformat("077", " d", var0) == OK && var0 == 77);
	ASSERT(unformat("11", " o ", var0) == OK && var0 == 9);
	ASSERT(unformat("0x22", " x   ", var0) == OK && var0 == 0x22);
	ASSERT(unformat("66", "		b", var0) == ERROR_NAN);
	ASSERT(unformat("0101", " b   ", var0) == OK && var0 == 5);
	ASSERT(unformat("0b11", " b   ", var0) == OK && var0 == 3);
	print(" ");
	print(" - Group 2:");
	print(" ");
	ASSERT(unformat("22", "b|x", var0, var1, var2) == OK && var1 == 0x22 && var2 == 1);
	ASSERT(unformat("0b111", "o|d|b", var0, var1, var2, var3) == OK && var2 == 0b111 && var3 == 2);
	ASSERT(unformat("0b111", "x|d|b", var0, var1, var2, var3) == OK && var0 == 0x0b111 && var3 == 0);
	ASSERT(unformat("0b111", "b|d|x", var0, var1, var2, var3) == OK && var0 == 0b111 && var3 == 0);
	ASSERT(unformat("0b111", "(b|d|x)", var0, var1, var2, var3) == OK && var0 == 0b111 && var3 == 0);
	var1 = 99;
	ASSERT(unformat("0b111", "b|d-|x", var0, var1) == OK && var0 == 0b111 && var1 == 0);
	var3 = 42;
	ASSERT(unformat("0b111", "(b|d|x-)", var0, var1, var2, var3) == OK && var0 == 0b111 && var3 == 42);
	ASSERT(unformat("0b111", "x", var0) == OK && var0 == 0x0b111);
	ASSERT(unformat("0b111", "b", var0) == OK && var0 == 0b111);
	ASSERT(unformat("0b111", "n", var0) == OK && var0 == 0b111);

	ASSERT(unformat("0b111 -1234 0XA", "bih", var0, var1, var2, var2) == OK && var0 == 0b111 && var1 == -1234 && var2 == 0xA);
	ASSERT(unformat("0b111 -1234 0X", "bih", var0, var1, var2, var2) == ERROR_NAN);
	print(" ");
	print(" - Group 2:");
	print(" ");
	ASSERT(unformat("999 88", "b-|dd|x", var0, var1, var2) == OK && var0 == 999 && var1 == 88 && var2 == 1);
	
	print(" ");
	print(" - Group 3:");
	print(" ");
	
	ASSERT(unformat("5", "i<0-9>", var0) == OK && var0 == 5);
	ASSERT(unformat("50", "i<0-9>", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat(" ", "I<0-9>(5)", var0) == OK && var0 == 5);
	// This DID crash, but then I found out why.  The odd error is because the
	// "(" after the "i<0-9>" starts a group, and the first item in that group
	// is "5)", i.e. 5 close group specifiers.  This, of course, will never run,
	// but it will also not compile because that means that the "5" consumes the
	// trailing ")", so there isn't one to end the group.
	ASSERT(unformat("", "i<0-9>(5)", var0) == ERROR_NO_GROUP_END);
	// This line gives an error quite late on.
	//RUN("2", "i<0-9>(5))", var0); // == ERROR_RAN_TRIVIAL.
	ASSERT(unformat("", "I<0-9>(5)", var0) == OK && var0 == 5);
	ASSERT(unformat(" ", "I< 0 - 9 >(50)", var0) == OK && var0 == 50);
	ASSERT(unformat(" ", "H<0 - 9>(50)", var0) == OK && var0 == 0x50);
	ASSERT(unformat(" ", "  X  <  0  -  9  >  (  FF  )  ", var0) == OK && var0 == 0xFF);
	ASSERT(unformat(" ", "D<-4--1>(50)", var0) == OK && var0 == 50);
	ASSERT(unformat(" ", "O<56-66>(43)", var0) == OK && var0 == 35);
	
	ASSERT(unformat(" ", "B<0110-1111>(1000)", var0) == OK && var0 == 0b1000);
	ASSERT(unformat("20", "I<0-9>(50)", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("10", "H<0-F>(50)", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("F", "X<0-9>(FF)", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("0", "D<-4--1>(50)", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("2", "O<56-66>(43)", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("10", "B<0110-1111>(1000)", var0) == ERROR_OUT_OF_RANGE);
	
	ASSERT(unformat("87", "I<-100 - 100>(50)", var0) == OK && var0 == 87);
	ASSERT(unformat("0x400", "H<FF - FFF>(50)", var0) == OK && var0 == 0x400);
	ASSERT(unformat("0", "X<0 - 0>(FF)", var0) == OK && var0 == 0);
	ASSERT(unformat("-2", "D<-4--1>(50)", var0) == OK && var0 == -2);
	ASSERT(unformat("56", "O<56-66> (43)", var0) == OK && var0 == 46);
	ASSERT(unformat("66", "O<56-66>( 43)", var0) == OK && var0 == 54);
	ASSERT(unformat("55", "O<56-66>(43 )", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("67", "O<56-66>(43) ", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("0111", "B<0110-1111>(1000)", var0) == OK && var0 == 0b0111);
	
	ASSERT(unformat("20", "i<0 -  9>", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("10", "h<  0-F>", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("F", "x<0-9>   ", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("0", "  d<-4  -  -1>", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("2", "  o   <56-66>", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("10", "b < 0110 - 1111 > ", var0) == ERROR_OUT_OF_RANGE);
	
	ASSERT(unformat("87", "i<-100 - 100>", var0) == OK && var0 == 87);
	ASSERT(unformat("0x400", "h<FF - FFF>", var0) == OK && var0 == 0x400);
	ASSERT(unformat("0", "x<0 - 0>", var0) == OK && var0 == 0);
	ASSERT(unformat("-2", "d <-4-  -1>", var0) == OK && var0 == -2);
	ASSERT(unformat("56", "o< 56-66>", var0) == OK && var0 == 46);
	ASSERT(unformat("66", "o<56 -66>", var0) == OK && var0 == 54);
	ASSERT(unformat("55", "o<56- 66>", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("67", "o<56-66 >", var0) == ERROR_OUT_OF_RANGE);
	ASSERT(unformat("0111", "b<0110-1111> ", var0) == OK && var0 == 0b0111);

	print(" ");
	print(" - Group 3:");
	print(" ");

	ASSERT(unformat("10 20", "o<0-20>i", var0, var1) == OK && var0 == 8 && var1 == 20);
	ASSERT(unformat("10 20", "o<0-20>i<0-20>", var0, var1) == OK && var0 == 8 && var1 == 20);
	// "n" DOES NOT have <> ranges apparently... (well, obviously, it's a
	// different class).  NOW IT DOES!!!
	ASSERT(unformat("0b111", "n<0b0110-0b1111> ", var0) == OK && var0 == 0b0111);
	//RUN("0b111", "n<0b0110-0b1111> ", var0);

	print(" ");
	print(" - Group 4:");
	print(" ");
	
	ASSERT(unformat("hello", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "hello"));
	ASSERT(unformat("there", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "there"));
	ASSERT(unformat("world", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "world"));
	ASSERT(unformat("hello there world", "s[10]s[10]s[10]", str0, str1, str2) == OK && str0[0] && str1[0] && str2[0]);
	ASSERT(!strcmp(str0, "hello") && !strcmp(str1, "there") && !strcmp(str2, "world"));
	ASSERT(unformat("hello there world", "s[10]s[10]", str0, str1) == OK && str0[0] && str1[0]);
	ASSERT(!strcmp(str0, "hello") && !strcmp(str1, "there wor"));
	ASSERT(unformat("hello there world", "s[10]s[16]", str0, str1) == OK && str0[0] && str1[0]);
	ASSERT(!strcmp(str0, "hello") && !strcmp(str1, "there world"));
	ASSERT(unformat("", "S(woop d)[10]", str0) == OK && str0[0] && !strcmp(str0, "woop d"));
	ASSERT(unformat("", "S(woop d)[4]", str0) == OK && str0[0] && !strcmp(str0, "woo"));
	ASSERT(unformat("long", "s[4]", str0) == OK && str0[0] && !strcmp(str0, "lon"));
	ASSERT(unformat("short one", "s[20]", str0) == OK && str0[0] && !strcmp(str0, "short one"));
	ASSERT(unformat("short one", "s[20] ", str0) == OK && str0[0] && !strcmp(str0, "short"));
	var0 = 2;
	ASSERT(unformat("short one", "s[*]", var0, str0) == OK && var0 == 2 && str0[0] && !strcmp(str0, "s"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 2;
	ASSERT(unformat("short one", "s[*] ", var0, str0) == OK && var0 == 2 && str0[0] && !strcmp(str0, "s"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 4;
	ASSERT(unformat("s o ", "s[*] ", var0, str0) == OK && var0 == 4 && str0[0] && !strcmp(str0, "s"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 4;
	ASSERT(unformat("s o ", "s[*]", var0, str0) == OK && var0 == 4 && str0[0] && !strcmp(str0, "s o"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 5;
	ASSERT(unformat("s o ", "s[*]", var0, str0) == OK && var0 == 5 && str0[0] && !strcmp(str0, "s o"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 5;
	ASSERT(unformat("s o\\ ", "s[*]", var0, str0) == OK && var0 == 5 && str0[0] && !strcmp(str0, "s o "));
	//printf("%d \"%s\"", var0, str0);
	var0 = 5;
	ASSERT(unformat("s o\\ ", "s[*] ", var0, str0) == OK && var0 == 5 && str0[0] && !strcmp(str0, "s"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 5;
	ASSERT(unformat("s\\ o\\ ", "s[*] ", var0, str0) == OK && var0 == 5 && str0[0] && !strcmp(str0, "s o "));
	//printf("%d \"%s\"", var0, str0);

	print(" ");
	print(" - Group 5:");
	print(" ");

	var0 = 2;
	ASSERT(unformat("short one", "S(xxy yzz)[*]", var0, str0) == OK && var0 == 2 && str0[0] && !strcmp(str0, "s"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 2;
	ASSERT(unformat("short one", "S(xxy yzz)[*] ", var0, str0) == OK && var0 == 2 && str0[0] && !strcmp(str0, "s"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 4;
	ASSERT(unformat("s o ", "S(xx yy zz )[*] ", var0, str0) == OK && var0 == 4 && str0[0] && !strcmp(str0, "s"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 4;
	ASSERT(unformat("s o ", "S(xx yy zz )[*]", var0, str0) == OK && var0 == 4 && str0[0] && !strcmp(str0, "s o"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 5;
	ASSERT(unformat("s o ", "S(xx yy zz )[*]", var0, str0) == OK && var0 == 5 && str0[0] && !strcmp(str0, "s o"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 5;
	ASSERT(unformat("s o\\ ", "S(xxy yzz\\ )[*]", var0, str0) == OK && var0 == 5 && str0[0] && !strcmp(str0, "s o "));
	//printf("%d \"%s\"", var0, str0);
	var0 = 5;
	ASSERT(unformat("s o\\ ", "S(xxy yzz\\ )[*] ", var0, str0) == OK && var0 == 5 && str0[0] && !strcmp(str0, "s"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 5;
	ASSERT(unformat("s\\ o\\ ", "S(xxy\\ yzz\\ )[*] ", var0, str0) == OK && var0 == 5 && str0[0] && !strcmp(str0, "s o "));
	//printf("%d \"%s\"", var0, str0);

	print(" ");
	print(" - Group 6:");
	print(" ");


	var0 = 2;
	ASSERT(unformat("", "S(xxy yzz)[*]", var0, str0) == OK && var0 == 2 && str0[0] && !strcmp(str0, "x"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 2;
	ASSERT(unformat("", "S(xxy yzz)[*] ", var0, str0) == OK && var0 == 2 && str0[0] && !strcmp(str0, "x"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 4;
	ASSERT(unformat("", "S(xx yy zz )[*] ", var0, str0) == OK && var0 == 4 && str0[0] && !strcmp(str0, "xx "));
	//printf("%d \"%s\"", var0, str0);
	var0 = 4;
	ASSERT(unformat("", "S(xx yy zz )[*]", var0, str0) == OK && var0 == 4 && str0[0] && !strcmp(str0, "xx "));
	//printf("%d \"%s\"", var0, str0);
	var0 = 10;
	ASSERT(unformat("", "S(xx yy zz )[*]", var0, str0) == OK && var0 == 10 && str0[0] && !strcmp(str0, "xx yy zz"));
	//printf("%d \"%s\"", var0, str0);
	var0 = 10;
	ASSERT(unformat("", "S(xxy yzz\\ )[*]", var0, str0) == OK && var0 == 10 && str0[0] && !strcmp(str0, "xxy yzz "));
	//printf("%d \"%s\"", var0, str0);
	var0 = 10;
	ASSERT(unformat("", "S(xxy yzz\\ )[*] ", var0, str0) == OK && var0 == 10 && str0[0] && !strcmp(str0, "xxy yzz "));
	//printf("%d \"%s\"", var0, str0);
	var0 = 10;
	ASSERT(unformat("", "S(xxy\\ yzz\\ )[*] ", var0, str0) == OK && var0 == 10 && str0[0] && !strcmp(str0, "xxy yzz "));
	//printf("%d \"%s\"", var0, str0);


	print(" ");
	print(" - Group 7:");
	print(" ");

	str0[0] = 888;
	ASSERT(unformat("", "S()[10]", str0) == OK && !str0[0]);
	str0[0] = 999;
	ASSERT(unformat("", "S(  )[10]", str0) == OK && !str0[0]);
	str0[0] = 777;
	ASSERT(unformat("", "S(\\ \\ )[10]", str0) == OK && str0[0] && !strcmp(str0, "  "));
	ASSERT(unformat("", "s[10]", str0) == ERROR_NO_STRING_LITERAL);
	ASSERT(unformat("\\q", "s[10]", str0) == ERROR_INVALID_ESCAPE);
	ASSERT(unformat("\\x", "s[10]", str0) == ERROR_NAN);
	ASSERT(unformat("\\x;", "s[10]", str0) == ERROR_NAN);
	ASSERT(unformat("\\x", "s[10]", str0) == ERROR_NAN);
	ASSERT(unformat("\\0;", "s[10]", str0) == OK && !str0[0]);
	str0[0] = -1, str0[1] = -1, str0[2] = -1, str0[3] = -1;
	// This test passes, but the "str0[0] == '\0'" means it doesn't print right.
	ASSERT(unformat("\\09;", "s[10]", str0) == OK && str0[0] == '\0' && str0[1] == '9' && str0[2] == ';' && str0[3] == '\0');
	ASSERT(unformat("\\-", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "-"));
	// The "ASSERT" macro string expansion doesn't like the excessive "\"s.
	printf("%s %s", (unformat("\\\\", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\\")) ? ("PASS!") : ("FAIL"),
		"\"unformat(\"\\\\\", \"s[10]\", str0) == OK && str0[0] && !strcmp(str0, \"\\\")\"");
	printf("%s %s", (unformat("\\\"", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\"")) ? ("PASS!") : ("FAIL"),
		"\"unformat(\"\\\"\", \"s[10]\", str0) == OK && str0[0] && !strcmp(str0, \"\"\")\"");
	printf("%s %s", (unformat("\\\'", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "'")) ? ("PASS!") : ("FAIL"),
		"\"unformat(\"\\\'\", \"s[10]\", str0) == OK && str0[0] && !strcmp(str0, \"\'\")\"");
	//ASSERT(unformat("\\\\", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\\"));
	//ASSERT(unformat("\\\"", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\""));
	//ASSERT(unformat("\\\'", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "'"));
	ASSERT(unformat("\\x42;", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\x42;"));
	ASSERT(unformat("\\u42;", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\x42;"));
	ASSERT(unformat("\\42;", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\42;"));
	ASSERT(unformat("\\053;", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\43;"));
	// String options: No escapes.
	

	print(" ");
	print(" - Group 8:");
	print(" ");

	ASSERT(unformat("", "?<BLA>") == ERROR_UNKNOWN_OPTION);
	ASSERT(unformat("", "?<NONE>") == ERROR_UNKNOWN_OPTION);
	ASSERT(unformat("", "?<_OPTIONS_COUNT>") == ERROR_UNKNOWN_OPTION);
	ASSERT(unformat("", "?<READ_SIMPLE_>") == ERROR_UNKNOWN_OPTION);
	ASSERT(unformat("", "?<READ_SIMPL>") == ERROR_UNKNOWN_OPTION);
	ASSERT(unformat("", "?<READ_SIMPLE>") == OK);
	ASSERT(unformat("", "?<READ_EXTENDED>") == OK);
	ASSERT(unformat("", "?<READ_NL>") == OK);
	ASSERT(unformat("", "?<NORMALISE_NL>") == OK);
	ASSERT(unformat("7813", "?<NORMALISE_NL>i", var0) == OK && var0 == 7813);
	ASSERT(unformat("34 6456", "i?<READ_SIMPLE>i", var0, var1) == OK && var0 == 34 && var1 == 6456);
	ASSERT(unformat("735 537", "ii?<READ_EXTENDED>", var0, var1) == OK && var0 == 735 && var1 == 537);
	ASSERT(unformat("69 465", "?<READ_NL>ii", var0, var1) == OK && var0 == 69 && var1 == 465);

	
	print(" ");
	print(" - Group 9:");
	print(" ");

	ASSERT(unformat("\\ ", "s[10]", str0) == OK && str0[0] && !strcmp(str0, " "));
	ASSERT(unformat("\\ ", "?<READ_SIMPLE>s[10]", str0) == OK && str0[0] == '\\' && str0[1] == '\0');
	ASSERT(unformat("\\ ", "?<READ_EXTENDED>s[10]", str0) == OK && str0[0] && !strcmp(str0, " "));
	ASSERT(unformat("\\ ", "?<READ_NL>s[10]", str0) == OK && str0[0] && !strcmp(str0, " "));
	ASSERT(unformat("\\r", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\r"));
	ASSERT(unformat("\\r", "?<READ_SIMPLE>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\\r"));
	ASSERT(unformat("\\r", "?<READ_EXTENDED>s[10]", str0) == ERROR_INVALID_ESCAPE);
	ASSERT(unformat("\\r", "?<READ_NL>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\r"));
	ASSERT(unformat("\\n", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\n"));
	ASSERT(unformat("\\n", "?<READ_SIMPLE>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\\n"));
	ASSERT(unformat("\\n", "?<READ_EXTENDED>s[10]", str0) == ERROR_INVALID_ESCAPE);
	ASSERT(unformat("\\n", "?<READ_NL>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\n"));
	ASSERT(unformat("\\r\\n", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\r\n"));
	ASSERT(unformat("\\r\\n", "?<READ_SIMPLE>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\\r\\n"));
	ASSERT(unformat("\\r\\n", "?<READ_EXTENDED>s[10]", str0) == ERROR_INVALID_ESCAPE);
	ASSERT(unformat("\\r\\n", "?<READ_NL>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\r\n"));
	ASSERT(unformat("\\x42;", "s[10]", str0) == OK && str0[0] && !strcmp(str0, "\x42;"));
	ASSERT(unformat("\\x42;", "?<READ_SIMPLE>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\\x42;"));
	ASSERT(unformat("\\x42;", "?<READ_EXTENDED>s[10]", str0) == ERROR_INVALID_ESCAPE);
	ASSERT(unformat("\\x42;", "?<READ_NL>s[10]", str0) == ERROR_INVALID_ESCAPE);
	ASSERT(unformat("\\r", "?<NORMALISE_NL>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\n"));
	ASSERT(unformat("\\r", "?<NORMALISE_NL>?<READ_SIMPLE>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\\r"));
	ASSERT(unformat("\\r", "?<NORMALISE_NL>?<READ_EXTENDED>s[10]", str0) == ERROR_INVALID_ESCAPE);
	ASSERT(unformat("\\r", "?<NORMALISE_NL>?<READ_NL>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\n"));
	ASSERT(unformat("\\n", "?<NORMALISE_NL>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\n"));
	ASSERT(unformat("\\n", "?<NORMALISE_NL>?<READ_SIMPLE>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\\n"));
	ASSERT(unformat("\\n", "?<NORMALISE_NL>?<READ_EXTENDED>s[10]", str0) == ERROR_INVALID_ESCAPE);
	ASSERT(unformat("\\n", "?<NORMALISE_NL>?<READ_NL>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\n"));
	ASSERT(unformat("\\r\\n", "?<NORMALISE_NL>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\n"));
	ASSERT(unformat("\\r\\n", "?<NORMALISE_NL>?<READ_SIMPLE>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\\r\\n"));
	ASSERT(unformat("\\r\\n", "?<NORMALISE_NL>?<READ_EXTENDED>s[10]", str0) == ERROR_INVALID_ESCAPE);
	ASSERT(unformat("\\r\\n", "?<NORMALISE_NL>?<READ_NL>s[10]", str0) == OK && str0[0] && !strcmp(str0, "\n"));


	print(" ");
	print(" - Group 10:");
	print(" ");
	
	ASSERT(unformat("HELLO,WORLD", "s[32]", str0) == OK && str0[0] && !strcmp(str0, "HELLO,WORLD"));
	ASSERT(unformat("HELLO, WORLD", "s[32] ", str0) == OK && str0[0] && !strcmp(str0, "HELLO,"));
	ASSERT(unformat("HELLO, WORLD ", "s[32]", str0) == OK && str0[0] && !strcmp(str0, "HELLO, WORLD"));
	ASSERT(unformat("HELLO,WORLD", "p<,>s[32]", str0) == OK && str0[0] && !strcmp(str0, "HELLO"));
	ASSERT(unformat("HELLO ,WORLD", "p+<,>s[32]s[32]", str0, str1) == OK && str0[0] && !strcmp(str0, "HELLO"));
	ASSERT(unformat("HELLO, WORLD", "p+<,>s[32]s[32]", str0, str1) == OK && str0[0] && !strcmp(str0, "HELLO"));
	RUN("HELLO, WORLD", "p+<,>s[32]s[32]", str0, str1);
	printf("%s", str0);
	ASSERT(unformat("HELLO,, WORLD", "P+<,>s[32]s[32]", str0, str1) == OK && str0[0] && !strcmp(str0, "HELLO") && str1[0] && !strcmp(str1, "WORLD"));
	RUN("HELLO,, WORLD", "P+<,>s[32]s[32]", str0, str1);
	printf("%s", str0);
	printf("%s", str1);
	ASSERT(unformat("HELLO,, WORLD,", "P+<,>s[32]s[32]", str0, str1) == OK && str0[0] && !strcmp(str0, "HELLO") && str1[0] && !strcmp(str1, "WORLD"));
	ASSERT(unformat("HELLO,, WORLD,", "P+<,>s[32]P-<,>s[32]", str0, str1) == OK && str0[0] && !strcmp(str0, "HELLO") && str1[0] && !strcmp(str1, "WORLD,"));
	ASSERT(unformat("HELLO,, WORLD,", "P+<,>s[32]P<>s[32]", str0, str1) == OK && str0[0] && !strcmp(str0, "HELLO") && str1[0] && !strcmp(str1, "WORLD,"));
	ASSERT(unformat("HELLO,, WORLD,", "P+<,>s[32]p<>s[32]", str0, str1) == OK && str0[0] && !strcmp(str0, "HELLO") && str1[0] && !strcmp(str1, "WORLD,"));
	ASSERT(unformat("HELLO,, WORLD,", "P<,>s[32]P<>s[32]", str0, str1) == OK && str0[0] && !strcmp(str0, "HELLO") && str1[0] && !strcmp(str1, "WORLD,"));
	ASSERT(unformat("HELLO,, WORLD,", "p<,>s[32]s[32]", str0, str1) == ERROR_NO_STRING_LITERAL && !strcmp(str0, "HELLO"));
	ASSERT(unformat("HELLO,, WORLD,", "p<,>s[32]S(HA)[32]s[32]", str0, str1, str2) == OK && !strcmp(str0, "HELLO") && !strcmp(str1, "HA") && !strcmp(str2, "WORLD"));
	RUN("HELLO,, WORLD,", "p<,>s[32]S(HA)[32]s[32]", str0, str1, str2);

	

	print(" ");
	print(" - Group 11:");
	print(" ");

	ASSERT(unformat("HELLO,; WORLD,", "P<,;>s[32]S(HA)[32]s[32]", str0, str1, str2) == ERROR_NO_STRING_LITERAL);
	ASSERT(unformat("HELLO,; WORLD,", "P<,;>s[32]s[32]S(HA)[32]", str0, str1, str2) == OK && !strcmp(str0, "HELLO") && !strcmp(str1, "WORLD,") && !strcmp(str2, "HA"));
	ASSERT(unformat("HELLO,; WORLD,", "?<OLD_DELIMITERS>P<,;>s[32]S(HA)[32]s[32]", str0, str1, str2) == OK && !strcmp(str0, "HELLO") && !strcmp(str1, "HA") && !strcmp(str2, "WORLD"));
	ASSERT(unformat("HELLO,; WORLD,", "?<OLD_DELIMITERS>P+<,;>s[32]S(HA)[32]s[32]", str0, str1, str2) == ERROR_INVALID_DELIMITER_TYPE);
	ASSERT(unformat("HELLO,; WORLD,", "?<OLD_DELIMITERS>p<>s[32]S(HA)[32]s[32]", str0, str1, str2) == ERROR_INVALID_DELIMITER_TYPE);


	print(" ");
	print(" - Group 12:");
	print(" ");
	
	
	ASSERT(unformat("1 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("111 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("0101 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("1111 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("0 42", "ld", var0, var1) == OK && var0 == 0 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat(" 42", "L(1)d", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat(" 42", "L(0)d", var0, var1) == OK && var0 == 0 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("T 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("y 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("t 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("Y 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("TRUE 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("yEs 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("tRUe 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("YEs 42", "ld", var0, var1) == OK && var0 == 1 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("F 42", "ld", var0, var1) == OK && var0 == 0 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("f 42", "ld", var0, var1) == OK && var0 == 0 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("N 42", "ld", var0, var1) == OK && var0 == 0 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("n 42", "ld", var0, var1) == OK && var0 == 0 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("FalSE 42", "ld", var0, var1) == OK && var0 == 0 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("false 42", "ld", var0, var1) == OK && var0 == 0 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("nIL 42", "ld", var0, var1) == OK && var0 == 0 && var1 == 42); var1 = 8, var0 = 4;
	ASSERT(unformat("NUlL 42", "ld", var0, var1) == OK && var0 == 0 && var1 == 42); var1 = 8, var0 = 4;

}





