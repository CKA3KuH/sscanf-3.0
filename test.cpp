#include "src/utils.h"
#include "src/specifiers.h"
#include "src/specifiers/trivial_specifiers.h"
#include "src/specifiers/group_specifiers.h"
#include "src/specifiers/simple_specifiers.h"

#include <stdio.h>
#include <stdarg.h>

void logprintf(char * msg, ...)
{
	// Wrapper to make this function work in the test mode.
	va_list
		args;
	va_start(args, msg);
	vprintf(msg, args);
	printf("\n");
	va_end(args);
}

int
	main()
{
	return 0;
}

bool TestCompare(char const * one, cell const * two)
{
	while (*two)
	{
		if (*one != *two) return false;
		++one, ++two;
	}
	return *one == '\0';
}

// Need to test the test functions!  No point using it for verification if it
// doesn't work!
TEST(TestCompare0,  { cell d[] = {'g', 'D', 'q', '5', '\0'}; return TestCompare("gDq5", d) == true; });
TEST(TestCompare1,  { cell d[] = {'g', 'D', 'q', '5', '\0'}; return TestCompare("gdq5", d) == false; });
TEST(TestCompare2,  { cell d[] = {'g', 'D', 'q', '5', '\0'}; return TestCompare("gDq", d) == false; });
TEST(TestCompare3,  { cell d[] = {'g', 'D', 'q', '5', '\0'}; return TestCompare("gDq57", d) == false; });
TEST(TestCompare4,  { cell d[] = {'g', 'D', 'q', '5', '\0'}; return TestCompare("", d) == false; });
TEST(TestCompare5,  { cell d[] = {'\0'}; return TestCompare("", d) == true; });
TEST(TestCompare6,  { cell d[] = {'\0'}; return TestCompare(" ", d) == false; });

TEST(CopyStr0,  { cell dest[42]; return Utils::CopyString(dest, S"Hello", 5, false) == OK && TestCompare("Hell", dest); })
TEST(CopyStr1,  { cell dest[7]; dest[6] = 42; return Utils::CopyString(dest, S"Hello", 7, false) == OK && TestCompare("Hello", dest) && dest[6] == 42; })
TEST(CopyStr2,  { cell dest[7]; dest[6] = 42; return Utils::CopyString(dest, S"world", 7, true) == OK && TestCompare("world", dest) && dest[6] == 0; })





