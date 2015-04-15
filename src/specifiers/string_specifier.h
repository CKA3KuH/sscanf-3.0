#pragma once

#include "../specifiers.h"

class StringSpecifier : public Specifier
{
PUBLIC:
	// cons
		StringSpecifier()
	:
		Specifier('s'),
		m_ssize(0),
		m_length(0),
		m_default(nullptr),
		m_last(false)
	{
	};
	
	// cons
		StringSpecifier(StringSpecifier const & that)
	:
		Specifier(that),
		m_ssize(that.m_ssize),
		m_length(that.m_length),
		m_default(that.m_default),
		m_last(that.m_last)
	{
	};
	
	virtual // dest
		~StringSpecifier()
	{
		delete m_default;
	};
	
	virtual error_t
		ReadToken(char const * & input)
	{
		// Check this has the correct specifier.
		FAIL((*input | 0x20) == GetSpecifier(), ERROR_EXPECTED_A_GOT_B_2, GetSpecifier(), *input);
		// Check if this is upper-case (optional).
		if (*input++ != GetSpecifier())
		{
			char const *
				str;
			TRY(Utils::GetDefaults(input, str, &m_length));
			m_default = new cell [m_length + 1];
			Utils::CopyString(m_default, str, m_length + 1);
			SetOptional();
		}
		TRY(Utils::GetLength(input, &m_ssize));
		// If the last specifier is a string, it is handled specially.  Note
		// that the old definitions allowed "S(hi) " to override this behaviour,
		// but the new system is coded to frequently skip spaces.  As a result
		// we must look backwards to preserve the old operation.  Look for the
		// NULL that we replaced the close square bracket with.
		m_last = *input == '\0' && *(input - 1) == '\0';
		return OK;
	};
	
	CLONE();
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
	virtual cell
		Skip(Environment & env)
	{
		env.Skip(1, m_size);
		return 1;
	};
	
PRIVATE:
	bool
		m_last;
	
	int
		m_ssize;
	
	size_t
		m_length;
	
	cell *
		m_default;
};

