#pragma once

#include <functional>

enum E_SSCANF_ERROR
{
	OK,
	ERROR_UNKNOWN_ERROR, // Generic catch-all.
	ERROR_MEMORY_ALLOCATION_FAIL, // std::bad_alloc (untested).
	ERROR_NAN, // Could not evaluate the string as a number.
	ERROR_INVALID_ESCAPE, // Found '\\Q', where 'Q' is not valid.
	ERROR_UNCLOSED_CHARACTER_LITERAL, // Found 'a, with no close '.
	ERROR_NO_DEAFULT_START, // No ( on defaults.
	ERROR_NO_DEAFULT_END, // No ) on defaults.
	ERROR_NO_GROUP_END, // No ) on groupings.
	ERROR_NO_ARRAY_START, // No [ on arrays.
	ERROR_NO_ARRAY_END, // No ] on arrays.
	ERROR_NO_PARAM_START, // No < on parameters.
	ERROR_NO_PARAM_END, // No > on parameters.
	ERROR_NO_STRING_END, // No ' on stringa.
	ERROR_INVALID_ARRAY_SIZE, // [] or [0].
	ERROR_EXPECTED_A_GOT_B, // Expected "A", got "B".
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
};

typedef
	enum E_SSCANF_ERROR
	error_t;

#ifdef SSCANF_DEBUG
	#include <stdio.h>
	#include <sstream>
	
	typedef
		std::basic_stringstream<char>
		ss;
	
	class SscanfTest
	{
	protected:
		// cons
			SscanfTest(char * name);
	};
	
	#define TEST(name, ...)                         \
		class Test##name : public SscanfTest {      \
		public: Test##name() :                      \
			SscanfTest(#name) {                     \
				auto f = [] () -> bool __VA_ARGS__; \
				if (!f()) printf(#name " - FAILED!\n\n"); \
			};                                      \
		}; Test##name gTest##name;
	
	// This declares a test for a class.
	#define CTEST(name, ...)                        \
		public: friend class Test##name;            \
		class Test##name : public SscanfTest {      \
		public: Test##name() :                      \
			SscanfTest(#name) {                     \
				auto f = [] () -> bool __VA_ARGS__; \
				if (!f()) printf(#name " - FAILED!\n\n"); \
			};                                      \
		}; static Test##name mTest##name;
	
	// This defines the memory for a class test.
	#define ITEST(cls, name)                        \
		cls::Test##name cls::mTest##name;
	
	class SscanfConverter
	{
	public:
		// Operator to convert a literal string to a reference to a constant 
		// string pointer.  There is no automatic conversion, and we can't
		// define a cast operator for that, so we use a prefix like "L" ("S").
		char const * &
			operator=(const char * str)
		{
			m_var = str;
			return m_var;
		};
		
		char const *
			GetCurPos() const
		{
			return m_var;
		};
		
	private:
		char const *
			m_var;
	};
	
	extern SscanfConverter TESTER;
	
	#define CUR TESTER.GetCurPos()
	
	#define S TESTER=
#else
	#define TEST(name, ...)
	#define CTEST(name, ...)
	#define ITEST(cls, name)
#endif

// TODO better.
#define FAIL(test, error, ...) do { if (!(test)) return (error); } while (false)
#define TRY(n) do { error_t _error = (n); if (_error != OK) return _error; } while (false)
#define NEXT(i, c, e) do { Utils::SkipWhitespace(i); if (*i++ != (c)) return (e); Utils::SkipWhitespace(i); } while (false)

