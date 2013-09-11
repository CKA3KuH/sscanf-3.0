#pragma once

#include "../specifiers.h"

class StringSpecifier : public Specifier
{
public:
	// cons
		StringSpecifier()
	:
		Specifier('"'),
		m_ssize(0),
		m_length(0),
		m_default(nullptr),
		m_last(false)
	{
	};
	
	// cons
		StringSpecifier(StringSpecifier const & that)
	:
		Specifier('"'),
		m_ssize(that.m_ssize),
		m_length(that.m_length),
		m_default(that.m_default),
		m_last(that.m_last)
	{
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
		}
		TRY(Utils::GetLength(input, &m_ssize));
		// If the last specifier is a string, it is handled specially.  Note
		// that the old definitions allowed "S(hi) " to override this behaviour,
		// but the new system is coded to frequently skip spaces.  As a result
		// we must look backwards to preserve the old operation.
		m_last = *input == '\0' && *(input - 1) > ' ';
		return OK;
	};
	
	CLONE();
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		cell
			size = 0;
		if (m_ssize < 0)
		{
			// Get it from "env".
			TRY(env.GetNextValue(&size));
			FAIL(size > 0, ERROR_INVALID_ARRAY_SIZE);
		}
		else if (m_ssize > 0) size = m_ssize;
		else FAIL(env.GetMemory()->GetZeroLengthValid(), ERROR_INVALID_ARRAY_SIZE);
		char const *
			start = input;
		cell
			dump;
		if (size)
		{
			// Reserve one character for the "NULL" terminator always.
			--size;
			size_t
				idx = 0;
			if (m_last)
			{
				while (idx < size && !env.AtDelimiter(input))
				{
					TRY(Utils::ReadChar(input, dump));
					TRY(env.SetNextValue(dump, idx++));
				}
				// Pad the remainder of any array memory.
				TRY(env.SetNextValue('\0', idx));
				TRY(env.Skip(0, size - idx));
				if (env.AtDelimiter(input)) return OK;
			}
			else
			{
				while (idx < size && *input > ' ')
				{
					TRY(Utils::ReadChar(input, dump));
					TRY(env.SetNextValue(dump, idx++));
				}
				// Both "idx" and "size" are OBO at this point, so cancel out.
				TRY(env.SetNextValue('\0', idx));
				TRY(env.Skip(0, size - idx));
				if (*input <= ' ') return OK;
			}
			// This is not a fatal error - the remainder is being dumped.
			logprintf("sscanf warning: String buffer overflow. at %s:%d.", __FILE__, __LINE__);
		}
		// Loop to an explicit delimiter (NULL counts).  Use "ReadChar" so that
		// we can still do "\ " to include spaces when we shouldn't otherwise.
		if (m_last) while (!env.AtDelimiter(input)) TRY(Utils::ReadChar(input, dump));
		else while (*input > ' ') TRY(Utils::ReadChar(input, dump));
		// Discard the result.
		return OK;
	};
	
	virtual cell
		Skip(Environment & env)
	{
		env.Skip(1);
		return 1;
	};
	
private:
	bool
		m_last;
	
	int
		m_ssize;
	
	size_t
		m_length;
	
	cell *
		m_default;
};

