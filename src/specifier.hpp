#pragma once

class Specifier
{
public:
	// cons
		Specifier(char s)
	:
		m_specifier(s),
		m_minus(false),
		m_optional(false)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input) = 0;
	
	// "Run" takes a "Environment" parameter.  This includes the current memory
	// system to use, what delimiters and options are active, and which parser
	// is being used to separate up the string.
	virtual error_t
		Run(char const * & input, Environment & env) = 0;
	
	// Can't have a default for this because the class is pure virtual.
	virtual error_t
		Clone(Specifier ** dest) = 0;
	
	void
		SetOptional() { m_optional = true; };
	
	bool
		GetOptional() { return m_optional; };
	
	void
		SetSkip() { m_minus = true; };
	
	bool
		GetSkip() { return m_minus; };
	
	char
		GetSpecifier() { return m_specifier; };
	
protected:
	virtual // dest
		~Specifier()
	{
	};
	
	// cons
		Specifier(Specifier const & s)
	:
		m_specifier(s.m_specifier),
		m_minus(s.m_minus),
		m_optional(s.m_optional)
	{
	};
	
private:
	// cons
		Specifier()
	:
		m_specifier(s),
		m_minus(false),
		m_optional(false)
	{
	};
	
	char
		m_specifier;
	
	bool
		// Only here to skip memory (i.e. "-t").
		m_minus,
		// Doesn't have to exist (i.e. "T()").
		m_optional;
};

