#pragma once

#include <functional>

enum E_SSCANF_ERROR
{
	OK,
	ERROR_NO_TOKEN,
	ERROR_CHILD_NOT_SPECIFIER,
	ERROR_NAN, // Could not evaluate the string as a number.
	ERROR_INVALID_ESCAPE, // Found '\\Q', where 'Q' is not valid.
	ERROR_UNCLOSED_CHARACTER_LITERAL, // Found 'a, with no close '.
	ERROR_NO_DEAFULT_START, // No ( on defaults.
	ERROR_NO_DEAFULT_END, // No ) on defaults.
	ERROR_NO_ARRAY_START, // No [ on arrays.
	ERROR_NO_ARRAY_END, // No ] on arrays.
	ERROR_INVALID_ARRAY_SIZE, // [] or [0].
	ERROR_EXPECTED_A_GOT_B, // Expected "A", got "B".
};

typedef
	enum E_SSCANF_ERROR
	error_t;

#ifdef SSCANF_DEBUG
	#include <stdio.h>
	
	class SscanfTest
	{
	protected:
		// cons
			SscanfTest(char * name); //, std::function<bool()> func);
	};
	
	// The constructor runs the test, so just declaring this runs it.
/*	#define TEST(name, code)                         \
		class Test##name : public SscanfTest {       \
		public: Test##name() : SscanfTest(#name) {}; \
		protected: virtual bool Run() code;          \
		}; Test##name gTest##name;*/
	
	// The constructor runs the test, so just declaring this runs it.
	/*#define TEST(name, ...)                         \
		class Test##name : public SscanfTest {       \
		public: Test##name() :                       \
			SscanfTest(#name, [] () -> bool __VA_ARGS__) {};        \
		}; Test##name gTest##name;*/
	
	#define TEST(name, ...)                         \
		class Test##name : public SscanfTest {      \
		public: Test##name() :                      \
			SscanfTest(#name) {                     \
				auto f = [] () -> bool __VA_ARGS__; \
				if (!f()) printf(" - FAILED!\n\n"); \
			};                                      \
		}; Test##name gTest##name;
	
	class SscanfConverter
	{
	public:
		char const * &
			operator=(const char * str)
		{
			m_var = str;
			return m_var;
		};
		
	private:
		char const *
			m_var;
	};
	
	extern SscanfConverter TESTER;
	
	#define S TESTER=
#else
	#define TEST(name, code)
#endif

// TODO better.
#define FAIL(test, error, ...) do { if (!(test)) return (error); } while (false)
#define TRY(n) do { error_t _error = (n); if (_error != OK) return _error; } while (false)
//#define NEXT(i,c,e) do { if (NextChar(i, c) != OK) return e; } while (false)
#define NEXT(i, c, e) do { Utils::SkipWhitespace(i); if (*i++ != c) return (e); Utils::SkipWhitespace(i); } while (false)

