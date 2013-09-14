#include "string_specifier.h"
#include "../errors.h"

Environment *
	gEnv;

// Option: READ_SIMPLE
error_t
	ReadSimpleChar(char const * & input, cell & dest)
{
	// Don't allow ANY escapes.
	dest = *input++;
	return OK;
}

// Option: READ_EXTENDED
error_t
	ReadExtendedChar(char const * & input, cell & dest)
{
	// Allow SOME escapes.
	dest = *input++;
	if (dest == '\\')
	{
		if (*input == '\\') ++input;
		else if (gEnv && gEnv->AtDelimiter(input, true)) dest = *input++;
		else return ERROR_INVALID_ESCAPE;
	}
	return OK;
}

// Option: READ_NL
error_t
	ReadNLChar(char const * & input, cell & dest)
{
	// Allow MORE escapes.
	dest = *input++;
	if (dest == '\\')
	{
		switch (*input)
		{
			case '\\': ++input; break;
			case 'r': dest = '\r'; ++input; break;
			case 'n': dest = '\n'; ++input; break;
			default:
				if (gEnv && gEnv->AtDelimiter(input, true)) dest = *input++;
				else return ERROR_INVALID_ESCAPE;
				break;
		}
	}
	return OK;
}

// Option: NORMALISE_NL
void
	NormaliseNL(char const * & input, cell & cur, ReadFunction_t reader)
{
	// Convert "\n", "\r", and "\r\n" to just "\n".  "\n\r" is malformed, assume
	// it to mean "\n\n" (or look for "\n\r\n", i.e. mixed style).
	char const *
		tmp = input;
	if (cur == '\r')
	{
		if ((*reader)(tmp, cur) == OK && cur == '\n') input = tmp;
		else cur = '\n';
	}
}

error_t
	StringSpecifier::
	Run(char const * & input, Environment & env)
{
	// =========================================================================
	//                         Get the reader configuration.                        
	// =========================================================================
	ReadFunction_t
		reader =
			env.GetOption(OPTION_READ_SIMPLE) ? &ReadSimpleChar :
			env.GetOption(OPTION_READ_EXTENDED) ? &ReadExtendedChar :
			env.GetOption(OPTION_READ_NL) ? &ReadNLChar :
			&Utils::ReadChar;
	gEnv = &env;
	int
		normalise = env.GetOption(OPTION_NORMALISE_NL);
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
			TRY((*reader)(input, dump));
			// If we aren't saving the data, it doesn't matter if we don't
			// normalise it first.
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
			TRY((*reader)(input, dump));
			if (normalise) NormaliseNL(input, dump, reader);
		}
		while (!env.AtDelimiter(input, incws));
		// We can now get some characters from "start" on.
		mn = (size < truLen) ? size : truLen;
		// Copy "mn" many cells.
		for (size_t i = 0; i != mn; ++i)
		{
			(*reader)(start, dump); // No need to "try" again.
			if (normalise) NormaliseNL(input, dump, reader);
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
























