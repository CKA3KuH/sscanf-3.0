#pragma once

#include <malloc.h>
#include "../sdk/plugin.h"
#include "errors.h"

// This class is pretty much the main work-horse of the whole plugin!
class Utils
{
PUBLIC:
	static int
		Strincmp(char const * a, char const * b, size_t n);
	
	static error_t
		ReadOctal(char const * & input, cell & n);
	
	static error_t
		ReadDecimal(char const * & input, cell & n);
	
	static error_t
		ReadHex(char const * & input, cell & n);
	
	static error_t
		ReadBinary(char const * & input, cell & n);
	
	static error_t
		ReadNum(char const * & input, cell & n);
	
	static error_t
		ReadIEEE(char const * & input, cell & n);
	
	static error_t
		ReadFloat(char const * & input, cell & n);
	
	static error_t
		ReadChar(char const * & input, cell & n);
	
	static error_t
		ReadCharEx(char const * & input, cell & n);
	
	static error_t
		ReadLogical(char const * & input, cell & n);
	
	static void
		SkipWhitespace(char const * & input);
	
	// This is only used in tests, so we can have it in a "return" statement.
	static error_t
		SkipWhitespaceOK(char const * & input) { SkipWhitespace(input); return OK; };
	
	static error_t
		GetBounded(char const * & input, char const * & output, char start, char end, size_t * len = nullptr);
	
	static error_t
		GetDefaults(char const * & input, char const * & output, size_t * len = nullptr);
	
	static error_t
		GetParams(char const * & input, char const * & output, size_t * len = nullptr);
	
	static error_t
		GetSizes(char const * & input, char const * & output, size_t * len = nullptr);
	
	static error_t
		GetString(char const * & input, char const * & output, char const t, size_t * len = nullptr);
	
	static error_t
		CopyString(cell * dest, char const * src, size_t len, bool pad = false);
	
	static error_t
		GetLength(char const * & input, int * s);
};

