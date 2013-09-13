#pragma once

enum E_SSCANF_OPTIONS
{
	OPTION_NONE,
	OPTION_READ_SIMPLE,
	OPTION_READ_EXTENDED,
	OPTION_READ_NL,
	OPTION_NORMALISE_NL,
	_OPTIONS_COUNT,
};

typedef
	enum E_SSCANF_OPTIONS
	option_t;

option_t
	LookupOption(char const * name);

