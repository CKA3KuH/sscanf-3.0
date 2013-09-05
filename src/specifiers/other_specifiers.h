#pragma once

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
			m_literal = new char [m_length + 1];
			strcpy(m_literal, that.m_literal);
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
		// Only end on the correct close.
		static char * const
			sDelimList[] = {"?", nullptr};
		sDelimList[0][0] = start;
		// This MUST be done because the reader ALWAYS skips leading spaces, so
		// there's no way to detect them because the have already been passed
		// over by the time this class' "Run" method is called.  I could back-
		// track, but that could in theory go in to memory located before the
		// start of the string and give a buffer underflow (or worse, crash).
		// Shame C strings aren't doubly NULL terminated (maybe mine will be)...
		// Whitespace should be EXPLICITLY escaped.
		m_length = Utils::GetStringLength(input, sDelimList);
		FAIL(*(input + m_length) == start, ERROR_UNCLOSED_STRING_LITERAL);
		// This might throw, but we catch that generically later.
		m_literal = new char [m_length + 1];
		Utils::GetString(input, m_literal, m_length + 1);
		// Skip the end character.
		++input;
		return OK;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		size_t
			len = strlen(input);
		if (len < m_length) return ERROR_NO_STRING_MATCH;
		if (strncmp(input, m_literal, m_length)) return ERROR_NO_STRING_MATCH;
		input += m_length;
		return OK;
	};
	
	virtual int
		GetMemoryUsage() { return 0; };
	
private:
	size_t
		m_length;
	
	char *
		m_literal;
};

class OptionSpecifier : public Specifier
{
public:
	// cons
		OptionSpecifier()
	:
		Specifier('?'),
		m_option(nullptr),
		m_value(nullptr)
	{
	};
	
	// cons
		OptionSpecifier(OptionSpecifier const & that)
	:
		Specifier(that),
		m_option(nullptr),
		m_value(nullptr)
	{
		if (that.m_option)
		{
			m_option = new char [strlen(that.m_option) + 1];
			strcpy(m_option, that.m_option);
		}
		if (that.m_value)
		{
			m_value = new char [strlen(that.m_value) + 1];
			strcpy(m_value, that.m_value);
		}
	};
	
	virtual // cons
		~OptionSpecifier()
	{
		delete m_option;
		delete m_value;
	};
	
	CLONE();
	
	virtual error_t
		ReadToken(char const * & input)
	{
		// Skip the "?".
		++input;
		static char const * const
			sDelimList[] = {">", "=", nullptr};
		// Skip the "<".
		NEXT(input, '<', ERROR_NO_PARAM_START);
		size_t
			lenName = Utils::GetStringLength(input, sDelimList);
		char *
			ptr = input + lenName;
		// Check if there is a value for the option.
		Utils::SkipWhitespace(ptr);
		if (*ptr == '=')
		{
			// Get the length of the value.
			++ptr;
			size_t
				lenValue = Utils::GetStringLength(ptr, sDelimList);
			// Check that the option is ended correctly.
			char *
				ptr2 = ptr + lenValue;
			NEXT(ptr2, '>', ERROR_NO_PARAM_END);
			// Allocate a single block of memory.
			m_option = new char [lenName + lenValue + 2];
			m_value = m_option + lenName + 1;
			// Copy the data.
			Utils::GetString(input, m_option, lenName + 1);
			Utils::GetString(ptr, m_value, lenValue + 1);
			// Finish.
			input = ptr2;
		}
		else if (*ptr == '>')
		{
			// Does not have a value.
			m_option = new char [lenName + 1];
			// Copy the data.
			Utils::GetString(input, m_option, lenName + 1);
			// Finish (skip the ">").
			input = ptr + 1;
		}
		else
		{
			FAIL(false, ERROR_NO_PARAM_END);
		}
		// Option neatly stored for later!
		return OK;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		// So that the environment doesn't try to skip two sets of delimiters.
		env.ZeroRead();
		return env.SetOption(m_option, m_value);
	};
	
	virtual int
		GetMemoryUsage() { return 0; };
	
private:
	size_t
		m_length;
	
	char *
		m_option;
	
	char *
		m_value;
};

class MinusSpecifier : public Specifier
{
public:
	// cons
		MinusSpecifier()
	:
		Specifier('-'), // Actually has 10 valid specifiers.
		m_child(nullptr),
		m_count(-1)
	{
		// Always "skip".
		SetSkip();
	};
	
	// cons
		MinusSpecifier(MinusSpecifier const & that)
	:
		Specifier(that),
		m_child(that.m_child),
		m_count(that.m_count)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input)
	{
		++input;
		// Get the child.
		TRY(gParser->GetNext(input, &m_child));
		FAIL(m_child, ERROR_NO_CHILD);
		// Lie about what we are, and pass all other ops through.  This allows
		// other specifiers that only check the specifier type to not need to
		// know anything about possible "minus" operators.
		m_specifier = m_child->GetSpecifier();
		m_child->SetSkip();
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
		env.ZeroRead();
		// Skips don't do anything but "skip".
		return env.GetMemory()->Skip(GetMemoryUsage());
	};
	
	virtual // dest
		~MinusSpecifier()
	{
		delete m_child;
	};
	
	static void
		DeleteTrivial(Specifier * that)
	{
		if (that->GetSkip())
		{
			dynamic_cast<MinusSpecifier *>(that)->m_child = nullptr;
			delete that;
		}
	};
	
private:
	Specifier *
		m_child;
	
	int
		m_count;
};

