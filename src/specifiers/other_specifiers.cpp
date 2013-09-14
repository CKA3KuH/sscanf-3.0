#include "other_specifiers.h"

error_t
	LiteralSpecifier::
	ReadToken(char const * & input)
{
	char const *
		str;
	// Get the extent of the input string.
	TRY(Utils::GetString(input, str, *input, &m_length));
	// This might throw, but we catch that generically later.
	m_literal = new cell [m_length + 1];
	// Currently has no failure modes when called after "GetStringLength".
	Utils::CopyString(m_literal, str, m_length + 1);
	return OK;
}

bool
	Matches(char const * one, cell const * two, size_t len)
{
	while (len--)
	{
		if (*one++ != *two++) return false;
	}
	return true;
}

error_t
	LiteralSpecifier::
	Run(char const * & input, Environment & env)
{
	if (!m_literal) return OK;
	char const *
		end = input + strlen(input) - m_length;
	while (input <= end)
	{
		if (Matches(input, m_literal, m_length))
		{
			input += m_length;
			return OK;
		}
		++input;
	}
	return ERROR_NO_STRING_MATCH;
}

error_t
	OptionSpecifier::
	ReadToken(char const * & input)
{
	// Skip the "?".
	++input;
	char const *
		opts;
	TRY(Utils::GetParams(input, opts));
	// Off by default, turned on here.
	m_value = 1;
	for (char const * val = opts; *val; ++val)
	{
		if (*val == '=')
		{
			// Has a value.
			*const_cast<char *>(val) = '\0';
			++val;
			Utils::SkipWhitespace(val);
			if (*val == '?') m_value = 0x80000000;
			else TRY(Utils::ReadDecimal(val, m_value));
			break;
		}
		// Strip spaces.
		else if ('\0' < *val && *val <= ' ')
		{
			*const_cast<char *>(val) = '\0';
		}
	}
	m_option = Options::Lookup(opts);
	FAIL(m_option != OPTION_NONE, ERROR_UNKNOWN_OPTION, opts);
	return OK;
}

error_t
	OptionSpecifier::
	Run(char const * & input, Environment & env)
{
	// So that the environment doesn't try to skip two sets of delimiters.
	env.ZeroRead();
	env.SetOption(m_option, m_value);
	return OK;
}

error_t
	MinusSpecifier::
	ReadToken(char const * & input)
{
	++input;
	// Get the child.
	TRY(gParser.GetNext(input, &m_child));
	FAIL(m_child, ERROR_NO_CHILD);
	// Lie about what we are, and pass all other ops through.  This allows
	// other specifiers that only check the specifier type to not need to
	// know anything about possible "minus" operators.
	m_specifier = m_child->GetSpecifier();
	m_child->SetSkip();
	return OK;
}

// This needs to count all READS, but not WRITES.
cell
	MinusSpecifier::
	Skip(Environment & env)
{
	if (m_child) return m_child->Skip(env);
	return 0;
}

error_t
	MinusSpecifier::
	Run(char const * & input, Environment & env)
{
	// So that the environment doesn't try to skip two sets of delimiters.
	//env.ZeroRead();
	// Skips don't do anything but "skip".
	//return env.Skip(Skip(env));
	if (m_child) m_child->Skip(env);
	return OK;
}

