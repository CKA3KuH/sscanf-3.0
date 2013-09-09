#pragma once

#include "../specifiers.h"
#include "../utils.h"
#include "../parser.h"

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
		ReadToken(char const * & input)
	{
		// Will either be ' or ".  The former is the preferred syntax as it
		// doesn't require escaping in PAWN literal strings, but " is equally as
		// acceptable if you want to put in the extra effort.
		char
			start = *input++;
		// This MUST be done because the reader ALWAYS skips leading spaces, so
		// there's no way to detect them because the have already been passed
		// over by the time this class' "Run" method is called.  I could back-
		// track, but that could in theory go in to memory located before the
		// start of the string and give a buffer underflow (or worse, crash).
		// Shame C strings aren't doubly NULL terminated (maybe mine will be)...
		// Whitespace should be EXPLICITLY escaped.
		char
			delims[2] = { start, '\0' };
		TRY(Utils::GetStringLength(input, delims, &m_length));
		FAIL(m_length, ERROR_NO_STRING_LITERAL);
		// This might throw, but we catch that generically later.
		m_literal = new cell [m_length + 1];
		// Currently has no failure modes when called after "GetStringLength".
		Utils::GetString(m_literal, input, m_length);
		// Skip the end character and whitespace.
		NEXT(input, start, ERROR_NO_STRING_END);
		return OK;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		if (!m_literal) return OK;
		char const *
			end = input + strlen(input) - m_length;
		while (input <= end)
		{
			if (LiteralSpecifier::Matches(input, m_literal, m_length))
			{
				input += m_length;
				return OK;
			}
			++input;
		}
		return ERROR_NO_STRING_MATCH;
	};
	
	static bool
		Matches(char const * one, cell const * two, size_t len)
	{
		while (len--)
		{
			if (*one++ != *two++) return false;
		}
		return true;
	};
	
	virtual int
		GetMemoryUsage() { return 0; };
	
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
		m_option(nullptr),
		m_value(0)
	{
	};
	
	// cons
		OptionSpecifier(OptionSpecifier const & that)
	:
		Specifier(that),
		m_option(nullptr),
		m_value(0)
	{
		if (that.m_option)
		{
			m_option = new char [strlen(that.m_option) + 1];
			strcpy(m_option, that.m_option);
		}
	};
	
	virtual // cons
		~OptionSpecifier()
	{
		delete [] m_option;
	};
	
	CLONE();
	
	virtual error_t
		ReadToken(char const * & input)
	{
		// Skip the "?".
		++input;
		// Skip the "<".
		NEXT(input, '<', ERROR_NO_PARAM_START);
		size_t
			lenName;
		if (Utils::GetStringLength(input, ">=", &lenName) == ERROR_NO_STRING_END) return ERROR_NO_PARAM_END;
		// Check if there is a value for the option.
		m_option = new char [lenName + 1];
		cell *
			dest;
		try
		{
			dest = new cell [lenName + 1];
		}
		catch (...)
		{
			delete [] m_option;
			throw;
		}
		// Copy the data.
		Utils::GetString(dest, input, lenName);
		Utils::SkipWhitespace(input);
		m_option[lenName] = '\0';
		while (lenName--)
		{
			m_option[lenName] = (char)dest[lenName];
		}
		delete [] dest;
		// Finish (skip the ">").
		if (*input == '=')
		{
			// Get the length of the value.
			++input;
			Utils::SkipWhitespace(input);
			Utils::ReadDecimal(input, m_value);
		}
		NEXT(input, '>', ERROR_NO_PARAM_END);
		// Option neatly stored for later!
		return OK;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		// So that the environment doesn't try to skip two sets of delimiters.
		//env.ZeroRead();
		env.SetOption(m_option, m_value);
		return OK;
	};
	
	virtual int
		GetMemoryUsage() { return 0; };
	
private:
	char *
		m_option;
	
	cell
		m_value;
	
	CTEST(Opt1, { OptionSpecifier opt; return opt.ReadToken(S"?<MOO>") == OK && !strcmp(opt.m_option, "MOO"); })
	CTEST(Opt2, { OptionSpecifier opt; return opt.ReadToken(S"?<HAHAH=56>") == OK && !strcmp(opt.m_option, "HAHAH") && opt.m_value == 56; })
	CTEST(Opt3, { OptionSpecifier opt; return opt.ReadToken(S"?") == ERROR_NO_PARAM_START; })
	CTEST(Opt4, { OptionSpecifier opt; return opt.ReadToken(S"?<") == ERROR_NO_PARAM_END; })
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
		ReadToken(char const * & input)
	{
		++input;
		// Get the child.
		TRY(gParser.GetNext(input, &m_child));
		FAIL(m_child, ERROR_NO_CHILD);
		// Lie about what we are, and pass all other ops through.  This allows
		// other specifiers that only check the specifier type to not need to
		// know anything about possible "minus" operators.
		m_specifier = m_child->GetSpecifier();
		m_child->SetSkip();
		return OK;
	};
	
	CLONE();
	
	// This needs to count all READS, but not WRITES.
	virtual int
		GetMemoryUsage()
	{
		if (m_child) return m_child->GetMemoryUsage();
		return 0;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		// So that the environment doesn't try to skip two sets of delimiters.
		//env.ZeroRead();
		// Skips don't do anything but "skip".
		return env.Skip(GetMemoryUsage());
	};
	
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
		ReadToken(char const * & input)
	{
		// Check this has the correct specifier.
		FAIL((*input | 0x20) == GetSpecifier(), ERROR_EXPECTED_A_GOT_B, GetSpecifier(), *input);
		// Check if this is upper-case (optional).
		if (*input++ != GetSpecifier())
		{
			SetOptional();
			// Capital letter - read in the deafult.
			// Skip the opening bracket.
			NEXT(input, '(', ERROR_NO_DEAFULT_START);
			TRY(Run(input, DefaultEnvironment::Get(&m_default)));
			// Skip the closing bracket.
			NEXT(input, ')', ERROR_NO_DEAFULT_END);
		}
		return OK;
	};
	
	CLONE();
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		cell
			dest;
		TRY(ReadNum(input, dest));
		return env.SetNextValue(dest);
		//return env.SkipDelimiters();
	};
	
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
	
#if defined SSCANF_DEBUG
public:
	static error_t
		ReadNum(char const * & input, cell & n);
#else
	static inline error_t
		ReadNum(char const * & input, cell & n);
#endif
};

