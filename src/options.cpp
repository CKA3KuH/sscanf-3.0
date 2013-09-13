#include "options.h"
#include <string.h>

#define OPTION(n) if (!strcmp(name, #n)) return OPTION_##n

option_t
	LookupOption(char const * name)
{
	OPTION(READ_SIMPLE);
	OPTION(READ_EXTENDED);
	OPTION(READ_NL);
	OPTION(NORMALISE_NL);
	return OPTION_NONE;
}

