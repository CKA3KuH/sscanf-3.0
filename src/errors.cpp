#include "errors.h"

#ifdef SSCANF_DEBUG

int
	gTestsRun = 0;

// cons
	SscanfTest::
	SscanfTest(char * name)
{
	++gTestsRun;
	//printf("Running: %s...\n", name);
}

SscanfConverter TESTER;

#endif

