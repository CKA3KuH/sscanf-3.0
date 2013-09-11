#pragma once

#include "../specifiers.h"
#include "../utils.h"
#include "../parser.h"
#include "../options.h"

class LiteralSpecifier : public Specifier
{
public:
	// cons
		LiteralSpecifier()
	:
		Specifier('"'),
		m_length(0),
		m_literal(nullptr)
	{
	};
	
	// cons
		LiteralSpecifier(LiteralSpecifier const & that)
	:
		Specifier(that),
		m_length(that.m_length),
		m_literal(nullptr)
	{
		if (that.m_literal)
		{
			m_literal = new cell [m_length + 1];
			memcpy(m_literal, that.m_literal, (m_length + 1) * sizeof (cell));
		}
	};
	
	virtual // cons
		~LiteralSpecifier()
	{
		delete m_literal;
	};
	
	CLONE();
	
	virtual error_t
		ReadToken(char const * & input);
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
private:
	size_t
		m_length;
	
	cell *
		m_literal;
	
	CTEST(LitSpec0a, { LiteralSpecifier ls; return ls.ReadToken(S"'hello") == ERROR_NO_STRING_END; })
	CTEST(LitSpec0b, { LiteralSpecifier ls; return ls.ReadToken(S"'   hello   \\'") == ERROR_NO_STRING_END; })
	CTEST(LitSpec1,  { LiteralSpecifier ls; return ls.ReadToken(S"'hello'") == OK && ls.Run(S"hello there", DefaultEnvironment::Get(nullptr)) == OK; })
	CTEST(LitSpec2,  { LiteralSpecifier ls; return ls.ReadToken(S"'hello'") == OK && ls.Run(S"there", DefaultEnvironment::Get(nullptr)) == ERROR_NO_STRING_MATCH; })
	CTEST(LitSpec3,  { LiteralSpecifier ls; return ls.ReadToken(S"'   hello'") == OK && ls.Run(S"hello there", DefaultEnvironment::Get(nullptr)) == OK; })
	CTEST(LitSpec4,  { LiteralSpecifier ls; return ls.ReadToken(S"'  hello  '") == OK && ls.Run(S"hello there", DefaultEnvironment::Get(nullptr)) == OK; })
	CTEST(LitSpec5,  { LiteralSpecifier ls; return ls.ReadToken(S"'  hello  '") == OK && ls.Run(S" therehello", DefaultEnvironment::Get(nullptr)) == OK; })
	CTEST(LitSpec6,  { LiteralSpecifier ls; return ls.ReadToken(S"'  hello  '") == OK && ls.Run(S"woop helloSS", DefaultEnvironment::Get(nullptr)) == OK && *CUR == 'S'; })
	CTEST(LitSpec7,  { LiteralSpecifier ls; return ls.ReadToken(S"'  hello  '") == OK && ls.Run(S"helhello__Q", DefaultEnvironment::Get(nullptr)) == OK && *CUR == '_'; })
	CTEST(LitSpec8,  { LiteralSpecifier ls; return ls.ReadToken(S"'  hello  '") == OK && ls.Run(S"hell", DefaultEnvironment::Get(nullptr)) == ERROR_NO_STRING_MATCH; })
};

class OptionSpecifier : public Specifier
{
public:
	// cons
		OptionSpecifier()
	:
		Specifier('?'),
		m_option(OPTION_NONE),
		m_value(0)
	{
	};
	
	// cons
		OptionSpecifier(OptionSpecifier const & that)
	:
		Specifier(that),
		m_option(that.m_option),
		m_value(that.m_value)
	{
	};
	
	virtual // cons
		~OptionSpecifier()
	{
	};
	
	CLONE();
	
	virtual error_t
		ReadToken(char const * & input);
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
private:
	//char *
	//	m_option;
	option_t
		m_option;
	
	cell
		m_value;
	
	// CTEST(Opt1, { OptionSpecifier opt; return opt.ReadToken(S"?<MOO>") == OK && !strcmp(opt.m_option, "MOO"); })
	// CTEST(Opt2, { OptionSpecifier opt; return opt.ReadToken(S"?<HAHAH=56>") == OK && !strcmp(opt.m_option, "HAHAH") && opt.m_value == 56; })
	// CTEST(Opt3, { OptionSpecifier opt; return opt.ReadToken(S"?") == ERROR_NO_PARAM_START; })
	// CTEST(Opt4, { OptionSpecifier opt; return opt.ReadToken(S"?<") == ERROR_NO_PARAM_END; })
};

class MinusSpecifier : public Specifier
{
public:
	// cons
		MinusSpecifier()
	:
		Specifier('-'), // Actually has 10 valid specifiers.
		m_child(nullptr)
	{
		// Always "skip".
		SetSkip();
	};
	
	// cons
		MinusSpecifier(MinusSpecifier const & that)
	:
		Specifier(that),
		m_child(that.m_child)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input);
	
	CLONE();
	
	// This needs to count all READS, but not WRITES.
	virtual cell
		Skip(Environment & env);
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
	virtual // dest
		~MinusSpecifier()
	{
		delete m_child;
	};
	
	virtual int
		CountChildren() const { return m_child ? 1 : 0; };
	
private:
	Specifier *
		m_child;
	
	CTEST(Minus0, { MinusSpecifier ms; return ms.ReadToken(S"-i") == OK && ms.GetSpecifier() == 'i' && ms.GetSkip() == true; })
	CTEST(Minus1, { MinusSpecifier ms; return ms.ReadToken(S"-H(42)") == OK && ms.GetSpecifier() == 'h' && ms.GetSkip() == true; })
};

class NumSpecifier : public Specifier
{
public:
	// cons
		NumSpecifier()
	:
		Specifier('n'),
		m_default(0)
	{
	};
	
	// cons
		NumSpecifier(char c)
	:
		Specifier(c),
		m_default(0)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input);
	
	CLONE();
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
private:
	cell
		m_default;
	
	CTEST(NumXr, { cell dest; NumSpecifier that; return that.Run(S"1234", DefaultEnvironment::Get(&dest)) == OK && dest == 1234; })
	CTEST(NumXm, { cell dest; NumSpecifier that; return that.Run(S"01234", DefaultEnvironment::Get(&dest)) == OK && dest == 668; })
	CTEST(NumXn, { cell dest; NumSpecifier that; return that.Run(S"0b9876", DefaultEnvironment::Get(&dest)) == OK && dest == 0xB9876; })
	CTEST(NumXo, { cell dest; NumSpecifier that; return that.Run(S"0x01234", DefaultEnvironment::Get(&dest)) == OK && dest == 0x1234; })
	CTEST(NumXp, { cell dest; NumSpecifier that; return that.Run(S"0b1110", DefaultEnvironment::Get(&dest)) == OK && dest == 14; })
	CTEST(NumXq, { cell dest; NumSpecifier that; return that.Run(S"0b0", DefaultEnvironment::Get(&dest)) == OK && dest == 0; })
	CTEST(NumXs, { cell dest; NumSpecifier that; return that.Run(S"-56", DefaultEnvironment::Get(&dest)) == OK && dest == -56; })
	CTEST(NumXt, { cell dest; NumSpecifier that; return that.Run(S"G", DefaultEnvironment::Get(&dest)) == ERROR_NAN; })
	CTEST(NumXu, { cell dest; NumSpecifier that; return that.Run(S"-a", DefaultEnvironment::Get(&dest)) == ERROR_NAN; })
};

