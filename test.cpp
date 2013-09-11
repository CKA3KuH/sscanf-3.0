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

