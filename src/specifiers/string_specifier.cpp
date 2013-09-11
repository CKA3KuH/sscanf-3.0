#include "string_specifier.h"

error_t
	StringSpecifier::
	Run(char const * & input, Environment & env)
{
	// =========================================================================
	//                         Get the destination size.                        
	// =========================================================================
	cell
		dump;
	size_t
		size = 0;
	if (m_ssize < 0)
	{
		// Get it from "env".
		TRY(env.GetNextValue(&dump));
		FAIL(dump > 0, ERROR_INVALID_ARRAY_SIZE);
		size = dump;
	}
	else if (m_ssize > 0) size = m_ssize;
	else FAIL(env.GetMemory()->GetZeroLengthValid(), ERROR_INVALID_ARRAY_SIZE);
	// Include whitespace?  Only the last string ignores implicit whitespace.
	bool
		incws = !m_last;
	// =========================================================================
	//                     Skip strings that aren't needed.                     
	// =========================================================================
	if (!size)
	{
		// Found a quiet optional string with no data to read in.
		if (env.AtDelimiter(input, false)) FAIL(GetOptional(), ERROR_NO_STRING_LITERAL);
		else do
		{
			// Skip the current string, don't save it anywhere.
			TRY(Utils::ReadChar(input, dump));
		}
		while (!env.AtDelimiter(input, incws));
		return OK;
	}
	--size;
	size_t
		mn,
		truLen = m_length;
	// =========================================================================
	//                         Use the optional string.                         
	// =========================================================================
	if (env.AtDelimiter(input, false))
	{
		FAIL(GetOptional(), ERROR_NO_STRING_LITERAL);
		// Much simpler:
		mn = (size < truLen) ? size : truLen;
		// Copy "mn" many cells.
		for (size_t i = 0; i != mn; ++i) TRY(env.SetNextValue(m_default[i], i));
	}
	// =========================================================================
	//                      Read in from the input string.                      
	// =========================================================================
	else
	{
		char const *
			start = input;
		// Count how many characters there are to be copied, trimming trailing
		// spaces.
		size_t
			posLen = 0;
		do
		{
			// This MUST be called first time to initialise "truLen" because
			// leading spaces are always trimmed in advance.
			if (*input > ' ') truLen = ++posLen;
			else ++posLen;
			TRY(Utils::ReadChar(input, dump));
		}
		while (!env.AtDelimiter(input, incws));
		// We can now get some characters from "start" on.
		mn = (size < truLen) ? size : truLen;
		// Copy "mn" many cells.
		for (size_t i = 0; i != mn; ++i)
		{
			Utils::ReadChar(start, dump); // No need to "try" again.
			TRY(env.SetNextValue(dump, i));
		}
	}
	// =========================================================================
	//                                   End.                                   
	// =========================================================================
	TRY(env.SetNextValue('\0', mn));
	TRY(env.Skip(0, size - mn));
	if (truLen > size) logprintf("sscanf warning: String buffer overflow. at %s:%d.", __FILE__, __LINE__);
	return OK;
}
























