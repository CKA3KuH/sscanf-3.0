#include "options.h"
#include <string.h>

#define OPTION(n) if (!strcmp(name, #n)) return OPTION_##n

option_t
	Options::
	Lookup(char const * name)
{
	OPTION(READ_SIMPLE);
	OPTION(READ_EXTENDED);
	OPTION(READ_NL);
	OPTION(NORMALISE_NL);
	OPTION(OLD_DEFAULT_KUSTOM);
	OPTION(SSCANF_QUIET);
	OPTION(MATCH_NAME_PARTIAL);
	OPTION(CELLMIN_ON_MATCHES);
	OPTION(OLD_DEFAULT_NAME);
	OPTION(OLD_DELIMITERS);
	return OPTION_NONE;
}
	
void
	Options::
	Set(option_t option, int const value)
{
	if (OPTION_NONE < option && option < _OPTIONS_COUNT)
	{
		if (value == 0x80000000)
		{
			if (m_parent) m_options[option] = m_parent->m_options[option];
			else m_options[option] = 0;
		}
		else m_options[option] = value;
	}
}

int
	Options::
	Get(option_t option)
{
	if (OPTION_NONE < option && option < _OPTIONS_COUNT)
	{
		return m_options[option];
	}
	return -1;
}

