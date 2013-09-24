#pragma once

#include <functional>

#include "../sscanf.h"

enum E_SSCANF_ERROR
{
	OK,
	ERROR_NOT_INITIALISED, // "UNFORMAT_Init" not yet called.
	ERROR_MISSING_PARAMETERS, // Not enough parameters given.
	ERROR_NO_SPECIFIER, // No specifier passed.
	ERROR_NO_COMPILE, // Unspecified compilation error.
	ERROR_UNKNOWN_ERROR, // Generic catch-all.
	ERROR_MEMORY_ALLOCATION_FAIL, // std::bad_alloc (untested).
	ERROR_NAN, // Could not evaluate the string as a number.
	ERROR_INVALID_ESCAPE, // Found '\\Q', where 'Q' is not valid.
	ERROR_UNCLOSED_CHARACTER_LIT, // Found 'a, with no close '.
	ERROR_NO_DEFAULT_START, // No ( on defaults.
	ERROR_NO_DEFAULT_END, // No ) on defaults.
	ERROR_NO_GROUP_END, // No ) on groupings.
	ERROR_NO_ARRAY_START, // No [ on arrays.
	ERROR_NO_ARRAY_END, // No ] on arrays.
	ERROR_NO_PARAM_START, // No < on parameters.
	ERROR_NO_PARAM_END, // No > on parameters.
	ERROR_NO_STRING_END, // No ' on stringa.
	ERROR_INVALID_ARRAY_SIZE, // [] or [0].
	ERROR_EXPECTED_A_GOT_B_1, // Expected "A", got "B".
	ERROR_EXPECTED_A_GOT_B_2, // Expected "A", got "B".
	ERROR_RAN_TRIVIAL, // Called "Run" on a trivial specifier.
	ERROR_INVALID_MEMORY, // There's no valid memory location available;
	ERROR_NO_STRING_MATCH, // Can't match "'xyz'".
	ERROR_NO_STRING_LITERAL, // Found '' as a string literal.
	ERROR_UNKNOWN_SPECIFIER, // Found an unrecognised letter.
	ERROR_INVALID_SPECIFIER, // Letter out of range.
	ERROR_DUPLICATE_SPECIFIER, // Two things the same.
	ERROR_PARSE_SEQUENTIAL_GROUP, // Tried to call "ReadToken" on "SequentialGroup",
	ERROR_NO_CHILD, // "-" "Run" called with no child set up.
	ERROR_NO_CHILDREN, // Alt branch with no children.
	ERROR_OUT_OF_VARIABLES,
	ERROR_INVALID_DELIMITER,
	ERROR_INVALID_DEFAULT,
	ERROR_INVALID_RANGE,
	ERROR_OUT_OF_RANGE,
	ERROR_UNKNOWN_OPTION,
	ERROR_INVALID_DELIMITER_TYPE,
	ERROR_COUNT_IS_NEGATIVE,
	ERROR_NO_QUIET_END
};

#define SHOW_OK ""
#define SHOW_ERROR_NOT_INITIALISED "System not initialised."
#define SHOW_ERROR_MISSING_PARAMETERS "Insufficient parameters."
#define SHOW_ERROR_NO_SPECIFIER "Missing required specifier."
#define SHOW_ERROR_NO_COMPILE "Unknown compilation error."
#define SHOW_ERROR_UNKNOWN_ERROR "Unknown error."
#define SHOW_ERROR_MEMORY_ALLOCATION_FAIL "std::bad_alloc thrown."
#define SHOW_ERROR_NAN "Input is not a number."
#define SHOW_ERROR_INVALID_ESCAPE "Invalid escape sequence."
#define SHOW_ERROR_UNCLOSED_CHARACTER_LIT "Unclosed character literal."
#define SHOW_ERROR_NO_DEFAULT_START "No default value found."
#define SHOW_ERROR_NO_DEFAULT_END "Unclosed default value."
#define SHOW_ERROR_NO_GROUP_END "Unclosed group."
#define SHOW_ERROR_NO_ARRAY_START "No array size found."
#define SHOW_ERROR_NO_ARRAY_END "Unclosed array size."
#define SHOW_ERROR_NO_PARAM_START "Missing requried specifier parameter."
#define SHOW_ERROR_NO_PARAM_END "Unclosed specifier parameter."
#define SHOW_ERROR_NO_STRING_END "Unclosed string literal."
#define SHOW_ERROR_INVALID_ARRAY_SIZE "Invalid array size."
#define SHOW_ERROR_EXPECTED_A_GOT_B_1 "Expected %s got %c."
#define SHOW_ERROR_EXPECTED_A_GOT_B_2 "Expected %c got %c."
#define SHOW_ERROR_RAN_TRIVIAL "Attempted to \"Run\" a Trivial Specifier."
#define SHOW_ERROR_INVALID_MEMORY "Cannot write to location."
#define SHOW_ERROR_NO_STRING_MATCH "No string literal match found."
#define SHOW_ERROR_NO_STRING_LITERAL "Empty string literal given."
#define SHOW_ERROR_UNKNOWN_SPECIFIER "Unknown format specifier '%c'."
#define SHOW_ERROR_INVALID_SPECIFIER "Invalid specifier."
#define SHOW_ERROR_DUPLICATE_SPECIFIER "Duplicate specifier."
#define SHOW_ERROR_PARSE_SEQUENTIAL_GROUP "Tried to call \"ReadToken\" on a Sequential Group."
#define SHOW_ERROR_NO_CHILD "Attempted to \"Run\" a specifier with no child."
#define SHOW_ERROR_NO_CHILDREN "Empty alt branch."
#define SHOW_ERROR_OUT_OF_VARIABLES "Insufficient destination variables given."
#define SHOW_ERROR_INVALID_DELIMITER "Could not find a valid delimiter."
#define SHOW_ERROR_INVALID_DEFAULT "The default value is not valid."
#define SHOW_ERROR_INVALID_RANGE "The range given is not valid (lower - higher)."
#define SHOW_ERROR_OUT_OF_RANGE "The input is outside the given range."
#define SHOW_ERROR_UNKNOWN_OPTION "Unknown option \"%s\" encountered."
#define SHOW_ERROR_INVALID_DELIMITER_TYPE "Invalid combination of delimiter options."
#define SHOW_ERROR_COUNT_IS_NEGATIVE "Variable numeric count is < 0."
#define SHOW_ERROR_NO_QUIET_END "Unclosed quiet section."

typedef
	enum E_SSCANF_ERROR
	error_t;

#ifdef SSCANF_DEBUG
	#define PRIVATE   public
	#define PUBLIC    public
	#define PROTECTED public
	
	#include "../test.h"
	
	#define TEST(name, ...)                         \
		class Test##name : public SscanfTest {      \
		PUBLIC: Test##name() :                      \
			SscanfTest(#name) {                     \
				auto f = [] () -> bool __VA_ARGS__; \
				if (!f()) printf(#name " - FAILED!\n\n"); \
			};                                      \
		}; Test##name gTest##name;
#else
	#define PRIVATE   private
	#define PUBLIC    public
	#define PROTECTED protected
	
	#define TEST(name, ...)
#endif

#if defined SSCANF_QUIET && !defined SSCANF_DEBUG
	#define FAIL(test, error, ...) \
		do { \
			if (!(test)) { \
				return (error); \
			} \
		} while (false)
	
	extern logprintf_t
		logprintf;
#else
	#ifdef SSCANF_DEBUG
		void logprintf(char * msg, ...);
	#else
		extern logprintf_t
			logprintf;
	#endif

	#define FAIL(test, error, ...) \
		do { \
			if (!(test)) { \
				logprintf("sscanf error: " SHOW_##error " (%d) at %s:%d.", __VA_ARGS__, (int)error, __FILE__, __LINE__); \
				return (error); \
			} \
		} while (false)
#endif

#define TRY(n) do { error_t _error = (n); if (_error != OK) return _error; } while (false)
#define NEXT(i, c, e) do { Utils::SkipWhitespace(i); FAIL(*i++ == (c), e); Utils::SkipWhitespace(i); } while (false)

