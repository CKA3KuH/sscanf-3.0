#pragma once

class Options
{
public:
	// cons
		Options()
	:
		m_options()
	{
	};
	
	// cons
		Options(Options const & that)
	:
		m_options(that.m_options)
	{
	};
	
	void
		Set(char const * name, int const value)
	{
		m_options[std::string(name)] = value;
	};
	
	int
		Get(char const * name)
	{
		return m_options[std::string(name)];
	};
	
private:
	std::map<std::string, int>
		m_options;
};

