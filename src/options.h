#pragma once

enum E_SSCANF_OPTIONS
{
	OPTION_NONE,
	OPTION_READ_SIMPLE, // Don't allow any escape characters in strings.
	OPTION_READ_EXTENDED, // Allow a few escape characters only.
	OPTION_READ_NL, // Allow the previous lot, plus "\r" and "\n".
	OPTION_NORMALISE_NL, // Convert "\r" and "\r\n" to just "\n".
	OPTION_OLD_DEFAULT_KUSTOM, // Kustom parse "K"'s default value.
	OPTION_SSCANF_QUIET, // Don't output any messages.
	OPTION_MATCH_NAME_PARTIAL, // Better name matching (find) at any point.
	OPTION_CELLMIN_ON_MATCHES, // Return "cellmin" for multiple name matches.
	OPTION_OLD_DEFAULT_NAME, // Require "U"'s default value to be connected.
	OPTION_OLD_DELIMITERS, // Use the old "p" and "P" behaviours.
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


