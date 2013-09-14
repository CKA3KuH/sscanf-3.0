#pragma once

enum E_SSCANF_OPTIONS
{
	OPTION_NONE,
	OPTION_READ_SIMPLE,
	OPTION_READ_EXTENDED,
	OPTION_READ_NL,
	OPTION_NORMALISE_NL,
	OPTION_OLD_DEFAULT_KUSTOM,
	OPTION_SSCANF_QUIET,
	OPTION_MATCH_NAME_PARTIAL,
	OPTION_CELLMIN_ON_MATCHES,
	OPTION_OLD_DEFAULT_NAME,
	_OPTIONS_COUNT,
};

typedef
	enum E_SSCANF_OPTIONS
	option_t;

option_t
	LookupOption(char const * name);

class Options
{
public:
	// cons
		Options()
	:
		m_parent(nullptr)
	{
		for (int i = (int)OPTION_NONE; i != (int)_OPTIONS_COUNT; ++i)
		{
			m_options[(option_t)i] = 0;
		}
	};
	
	// cons
		Options(Options * parent)
	:
		m_parent(parent)
	{
		for (int i = (int)OPTION_NONE; i != (int)_OPTIONS_COUNT; ++i)
		{
			m_options[(option_t)i] = parent->m_options[(option_t)i];
		}
	};
	
	void
		Set(option_t option, int const value);
	
	int
		Get(option_t option);

	static option_t
		Lookup(char const * name);
	
private:
	Options *
		m_parent;
	
	int
		m_options[_OPTIONS_COUNT];
};


