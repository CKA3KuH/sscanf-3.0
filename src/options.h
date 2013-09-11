#pragma once

enum E_SSCANF_OPTIONS
{
	OPTION_NONE,
	_OPTIONS_COUNT,
};

typedef
	enum E_SSCANF_OPTIONS
	option_t;

option_t
	LookupOption(char const * name);

