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

	
	
	
	
	
	
	
	
}


