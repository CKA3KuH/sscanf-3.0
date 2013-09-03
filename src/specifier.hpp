#pragma once

template <class T>
inline error_t
	DoClone(Specifier ** dest, T const * that)
{
	*dest = new T(*that);
	return OK;
}

#define CLONE()                         \
	virtual error_t                     \
		Clone(Specifier ** dest)        \
	{                                   \
		return DoClone(dest, this);     \
	}

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
		GetOptional() { return m_optional; } const;
	
	void
		SetSkip() { m_minus = true; };
	
	bool
		GetSkip() { return m_minus; } const;
	
	char
		GetSpecifier() { return m_specifier; } const;
	
	virtual int
		GetMemoryUsage() { return 1; };
	
	virtual // dest
		~Specifier()
	{
	};
	
	virtual std::ostream &
		Render(std::ostream & out) const
	{
		return out << m_specifier;
	};
	
	friend std::ostream &
		operator<<(std::ostream & out, Specifier const & s)
	{
		return s.Render(out);
	};
	
protected:
	// cons
		Specifier(Specifier const & that)
	:
		m_specifier(that.m_specifier),
		m_minus(that.m_minus),
		m_optional(that.m_optional)
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
	
	friend class MinusSpecifier;
};

