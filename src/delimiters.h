#pragma once

class Delimiters
{
PUBLIC:
	// cons
		Delimiters()
	:
		m_delimiters(),
		m_optionals()
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
		for (auto i = m_delimiters.begin(), e = m_delimiters.end(); i != e; ++i)
		{
			delete [] *i;
		}
		m_delimiters.clear();
		for (auto i = m_optionals.begin(), e = m_optionals.end(); i != e; ++i)
		{
			delete [] *i;
		}
		m_optionals.clear();
	};
	
	bool
		Delimit(char const * & input)
	{
		Utils::SkipWhitespace(input);
		if (m_delimiters.empty())
		{
		}
		else
		{
		}
		Utils::SkipWhitespace(input);
		return *input != '\0';
	};
	
PRIVATE:
	bool
		Match(char const * one, char const * two)
	{
		while (
	}
	
	
	void
		DoAdd(std::list<char *> & dest, char * delim)
	{
		size_t
			len = strlen(delim);
		char *
			c = new char [len + 1];
		strcpy(c, delim);
		c[len] = '\0';
		dest.push_back(c);
	};
	
	std::list<char *>
		m_delimiters,
		m_optionals;
};

