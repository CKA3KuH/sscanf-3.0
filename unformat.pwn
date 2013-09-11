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


	
	
	
	
	
	
	
	
}


