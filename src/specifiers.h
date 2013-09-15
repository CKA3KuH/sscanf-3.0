#pragma once

#include <iostream>
#include "errors.h"
#include "environment.h"

typedef
	error_t (* ReadFunction_t)(char const * &, cell &);

#define CLONE()                         \
	virtual error_t                     \
		Clone(Specifier ** dest)        \
	{                                   \
		return DoClone(dest, this);     \
	}

class Specifier
{
PUBLIC:
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
		GetOptional() const { return m_optional; };
	
	void
		SetSkip() { m_minus = true; };
	
	bool
		GetSkip() const { return m_minus; };
	
	char
		GetSpecifier() const { return m_specifier; };
	
	virtual error_t
		GetDefault(char const * & input, cell * target)
	{
		char const *
			start;
		TRY(Utils::GetDefaults(input, start));
		TRY(Run(start, DefaultEnvironment::Get(target)));
		FAIL(*start == '\0', ERROR_INVALID_DEFAULT);
		SetOptional();
		return OK;
	};
	
	virtual cell
		Skip(Environment & env) { return 0; };
	
	virtual int
		CountChildren() const { return 0; };
	
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
	
PROTECTED:
	// cons
		Specifier(Specifier const & that)
	:
		m_specifier(that.m_specifier),
		m_minus(that.m_minus),
		m_optional(that.m_optional)
	{
	};
	
	char
		m_specifier;
	
PRIVATE:
	// cons
		Specifier()
	:
		m_specifier('\0'),
		m_minus(false),
		m_optional(false)
	{
	};
	
	bool
		// Only here to skip memory (i.e. "-t").
		m_minus,
		// Doesn't have to exist (i.e. "T()").
		m_optional;
	
	//friend class MinusSpecifier;
};

template <class T>
inline error_t
	DoClone(Specifier ** dest, T const * that)
{
	*dest = new T(*that);
	return OK;
};

