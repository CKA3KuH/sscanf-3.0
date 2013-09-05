#pragma once

#include <malloc.h>
#include "..\sdk\plugin.h"
#include "errors.h"

typedef
	error_t (* ReadFunction_t)(char const * &, cell &);

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
		ReadChar(char const * & input, cell & n);
	
	static error_t
		ReadCharEx(char const * & input, cell & n);
	
	//static error_t
	//	GetDefaults(char const * & input, char const ** optional);
	
	static error_t
		GetArraySize(char const * & input, int * size, bool empty = false);
	
	static error_t
		NextChar(char const * & input, char val);
	
	static error_t
		NextChars(char const * & input, char const * const val);
	
	static void
		SkipWhitespace(char const * & input);
	
	static error_t
		SkipWhitespaceOK(char const * & input);
};

