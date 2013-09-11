#pragma once

#include <malloc.h>
#include "../sdk/plugin.h"
#include "errors.h"

class Utils
{
public:
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
		ReadChar(char const * & input, cell & n);
	
	static error_t
		ReadCharEx(char const * & input, cell & n);
	
	//static error_t
	//	GetDefaults(char const * & input, char const ** optional);
	
	// static error_t
		// GetArraySize(char const * & input, int * size, bool empty = false);
	
	// static error_t
		// NextChar(char const * & input, char val);
	
	// static error_t
		// NextChars(char const * & input, char const * const val);
	
	static void
		SkipWhitespace(char const * & input);
	
	// This is only used in tests, so we can have it in a "return" statement.
	static error_t
		SkipWhitespaceOK(char const * & input) { SkipWhitespace(input); return OK; };
	
	// static error_t
		// GetStringLength(char const * start, char * delims, size_t * len);
	
	// static error_t
		// GetString(cell * dest, char const * & input, size_t len);
	
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
};

