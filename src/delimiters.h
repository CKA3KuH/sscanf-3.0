#pragma once

#include "errors.h"

#include <vector>
#include <list>

class Delimiters
{
PUBLIC:
	// cons
		Delimiters()
	:
		m_delimiters(),
		m_optionals(),
		m_delimStore(16, '\0'),
		m_write(0),
		m_hasAny(false)
	{
	};
	
	// dest
		~Delimiters()
	{
		Reset();
	};
	
	void
		SetOptional(char delim)
	{
		Reset();
		AddOptional(delim);
	};
	
	void
		SetOptional(char * delim)
	{
		Reset();
		AddOptional(delim);
	};
	
	void
		Set(char delim)
	{
		Reset();
		Add(delim);
	};
	
	void
		Set(char * delim)
	{
		Reset();
		Add(delim);
	};
	
	void
		AddOptional(char delim)
	{
		static char
			c[2] = { '\0', '\0' };
		c[0] = delim;
		DoAdd(m_optionals, c);
	};
	
	void
		AddOptional(char * delim)
	{
		DoAdd(m_optionals, delim);
	};
	
	void
		Add(char delim)
	{
		static char
			c[2] = { '\0', '\0' };
		c[0] = delim;
		DoAdd(m_delimiters, c);
	};
	
	void
		Add(char * delim)
	{
		DoAdd(m_delimiters, delim);
	};
	
	void
		Reset()
	{
		m_delimiters.clear();
		m_optionals.clear();
		m_write = 0;
		m_hasAny = false;
	};
	
	bool
		AtDelimiter(char const * input, bool incWhite);
	
	bool
		SkipDelimiters(char const * & input);
	
PRIVATE:
	bool
		Match(char const * & input, char const * delim);
	
	void
		DoAdd(std::list<size_t> & dest, char * delim);
	
	std::list<size_t>
		m_delimiters,
		m_optionals;
	
	// Store the data in a single large block for better memory management.
	std::vector<char>
		m_delimStore;
	
	size_t
		m_write;
	
	bool
		m_hasAny;
};

