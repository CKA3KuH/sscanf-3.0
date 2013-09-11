#include "sdk/plugin.h"

#ifdef SSCANF_DEBUG
	#include <stdio.h>
	#include <sstream>
	
	typedef
		std::basic_stringstream<char>
		ss;
	
	class SscanfTest
	{
	PROTECTED:
		// cons
			SscanfTest(char * name);
	};
	
	class SscanfConverter
	{
	PUBLIC:
		// cons
			// SscanfConverter()
		// :
			// m_var(nullptr)
		// {
		// };
		
		// // dest
			// ~SscanfConverter()
		// {
			// //delete [] m_var;
			// //m_var = nullptr;
		// };
		
		// Operator to convert a literal string to a reference to a constant 
		// string pointer.  There is no automatic conversion, and we can't
		// define a cast operator for that, so we use a prefix like "L" ("S").
		char const * &
			operator=(char const * str)
		{
			size_t
				len = strlen(str);
			//delete [] m_var;
			//m_var = nullptr;
			char *
				st2 = new char [len + 1];
			strcpy(st2, str);
			st2[len] = '\0';
			m_var = st2;
			return m_var;
		};
		
		char const *
			GetCurPos() const
		{
			return m_var;
		};
		
	PRIVATE:
		char const *
			m_var;
	};
	
	extern SscanfConverter TESTER;
	
	#define CUR TESTER.GetCurPos()
	
	#define S TESTER=
	
	bool TestCompare(char const * one, cell const * two);
#endif

