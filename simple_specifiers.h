
class SimpleSpecifier : public Specifier
{
public:
	// cons
		SimpleSpecifier(char c)
	:
		Specifier(c),
		m_default(0)
	{
	};
	
	error_t
		Get(char const * & input, cell & result)
	{
		Utils::SkipWhitespace(input);
		ReadValue(input, result);
		Utils::SkipWhitespace(input);
		if (*input == '\0') return OK;
		while (*delimiters)
		{
			if (Matches(input, *delimiters, input))
			{
				return OK;
			}
			++delimiters;
		}
		return ERROR_NO_DELIMITER;
	};
	
	virtual error_t
		ReadToken(char const * & input)
	{
		// Check this has the correct specifier.
		if (*input | 0x20 == m_specifier)
		{
			// Skip it - simple as!
			if (*input++ != m_specifier)
			{
				SetOptional();
				// Capital letter - read in the deafult.
				// Skip the opening bracket.
				NEXT(input, '(', ERROR_NO_DEAFULT_START);
				// Now read in a value.  This function is ALWAYS pure virtual.
				error_t
					error = ReadValue(input, m_default);
				// Skip the closing bracket.
				NEXT(input, ')', ERROR_NO_DEAFULT_END);
			}
		}
		else FAIL(false, ERROR_EXPECTED_A_GOT_B, m_specifier, *input);
		return OK;
	};
	
	virtual ReadValue()
	{
	}
	
	int
		GetMemoryUsage() { return 1; };
	
private:
	cell
		m_default;
};

class Specifier_b   : public SimpleSpecifier { error_t ReadValue(char const * & input, cell & result) return Utils::ReadBinary (input, result); };
class Specifier_c   : public SimpleSpecifier
{
public:
	// cons
		Specifier_c
	:
		SimpleSpecifier('c', nullptr)
	{
		
	};
	
	error_t ReadValue(char const * & input, cell & result)
	{
		// Detect enclosing quotes.
		bool
			quotes = false;
		if (*input == '\'')
		{
			quotes = true;
			++input;
		}
		error_t
			error = Utils::ReadChar(input, result);
		if (error != OK) return error;
		if (quotes)
		{
			FAIL(*input == '\'', ERROR_UNCLOSED_CHARACTER_LITERAL);
			++input;
		}
		return OK;
	};
};
/*
class Specifier_d_i : public SimpleSpecifier { error_t ReadValue(char const * & input, cell & result) return Utils::ReadDecimal(input, result); };
class Specifier_f   : public SimpleSpecifier { error_t ReadValue(char const * & input, cell & result) return Utils::ReadFloat  (input, result); };
class Specifier_g   : public SimpleSpecifier { error_t ReadValue(char const * & input, cell & result) return Utils::ReadIEEE754(input, result); };
class Specifier_h_x : public SimpleSpecifier { error_t ReadValue(char const * & input, cell & result) return Utils::ReadHex    (input, result); };
class Specifier_l   : public SimpleSpecifier { error_t ReadValue(char const * & input, cell & result) return Utils::ReadLogical(input, result); };
class Specifier_n   : public SimpleSpecifier { error_t ReadValue(char const * & input, cell & result) return Utils::ReadNumber (input, result); };
class Specifier_o   : public SimpleSpecifier { error_t ReadValue(char const * & input, cell & result) return Utils::ReadOctal  (input, result); };
*/
// class Specifier_q   : public SimpleSpecifier { error_t ReadValue(char const * & input, cell & result) return Utils::ReadOctal  (input, result); };
// class Specifier_r   : public SimpleSpecifier { error_t ReadValue(char const * & input, cell & result) return Utils::ReadOctal  (input, result); };
// class Specifier_u   : public SimpleSpecifier { error_t ReadValue(char const * & input, cell & result) return Utils::ReadOctal  (input, result); };

SimpleSpecifier
	gSpecifierD('b', &Utils::ReadBinary ),
	gSpecifierD('d', &Utils::ReadDecimal),
	gSpecifierF('f', &Utils::ReadFloat  ),
	gSpecifierG('g', &Utils::ReadIEEE754),
	gSpecifierH('h', &Utils::ReadHex    ),
	gSpecifierI('i', &Utils::ReadDecimal),
	gSpecifierL('l', &Utils::ReadLogical),
	gSpecifierN('n', &Utils::ReadNumber ),
	gSpecifierO('o', &Utils::ReadOctal  ),
	gSpecifierX('x', &Utils::ReadHex    );



class NumericSpecifier : public Specifier
{
public:
	virtual error_t
		ReadToken(char const * & input)
	{
		char
			c = *input;
		FAIL('0' <= c && c <= '9', ERROR_EXPECTED_A_GOT_B, "digit", c);
		int
			num;
		//error_t
		//	error =
		Utils::ReadDecimal(input, num);
		// These can't happen thanks to the assert above.
		//if (error != OK) return error;
		//if (num < 0) return ERROR_INVALID_COUNT;
		if (num < 2) sWARN(WARNING_UNUSUAL_COUNT);
		m_count = num;
		Utils::SkipWhitespace(input);
		return Parser::GetToken(input, m_children);
	};
	
private:
	int
		m_count;
};

class LiteralSpecifier : public Specifier
{
public:
	// cons
		LiteralSpecifier()
	:
		Specifier(),
		m_literal(0)
	{
	};
	
	error_t
		ReadToken(char const * & input)
	{
		char
			start = *input;
		static char * const
			sDelimList[] = {"?", nullptr};
		FAIL(start == '\'' || start == '"', ERROR_EXPECTED_A_GOT_B, "string literal", *input);
		sDelimList[0][0] = start; // Only end on the correct close.
		++input;
		// Note that BOTH these functions skip whitespace unless it is
		// EXPLICITLY escaped.
		size_t
			len = Utils::GetStringLength(input, sDelimList);
		FAIL(*(input + len) == start, ERROR_UNCLOSED_STRING_LITERAL);
		// This might throw, but we catch that generically later.
		m_literal = new char [len + 1];
		Utils::GetString(input, m_literal, len + 1);
		++m_literal;
		return OK;
	};
	
private:
	char *
		m_literal;
};

// This is the class representing some collection of multiple other specifiers.
class SpecifierGroup : public Specifier
{
protected:
	std::list<Specifier *>
		m_children;
};

class EnumSpecifier : public SpecifierGroup
{
	error_t
		Run(char const * & input, Memory * memory)
	{
		EnumMemory
			mem(memory->GetNextPointer());
		for (var i = m_children.begin(), e = m_children.end(); i != e; ++i)
		{
			TRY((*i)->Run(input, mem));
		}
		return OK;
	}
};

class QuietGroup : public SpecifierGroup
{
public:
	// This needs to count all READS, but not WRITES.
	int
		GetMemoryUsage() { return 0; };
	
	error_t
		Run(char const * & input, Memory * memory)
	{
		// Just takes all writes and null-routes them, but still serves reads.
		QuietMemory
			quiet(memory);
		for (var i = m_children.begin(), e = m_children.end(); i != e; ++i)
		{
			TRY((*i)->Run(input, quiet));
		}
		return OK;
	};
};

// This is just a set of specifiers, to be run one after the other - simple as!
class Sequential : public SpecifierGroup
{
public:
	int
		GetMemoryUsage()
	{
		if (m_memory != -1) return m_memory;
		int
			mem = 0;
		for (var i = m_children.begin(), e = m_children.end(); i != e; ++i)
		{
			mem += (*i)->GetMemoryUsage();
		}
		return mem;
	};
	
	error_t
		Run(char const * & input, Memory * memory)
	{
		for (var i = m_children.begin(), e = m_children.end(); i != e; ++i)
		{
			TRY((*i)->Run(input, memory));
		}
		return OK;
	};
}

class AltGroup : public SpecifierGroup
{
public:
	int
		GetMemoryUsage()
	{
		int
			mem = 1;
		for (var i = m_children.begin(), e = m_children.end(); i != e; ++i)
		{
			mem += (*i)->GetMemoryUsage();
		}
		return mem;
	};
	
	error_t
		Run(char const * & input, Memory * memory)
	{
		error_t
			last = OK;
		cell
			alt = 0;
		std::list<Specifier *>::iterator
			i = m_children.begin(),
			e = m_children.end();
		// Parse this alternate.
		while (i != e)
		{
			char const *
				start = input;
			last = (*i)->Run(start, memory);
			if (last == OK)
			{
				// Found a group that works.
				break;
			}
			++alt;
			++i;
		}
		// Don't bother writing the alternate if there was no solution.
		TRY(last);
		// Find where to store the alternate.
		int
			skip = 0;
		while (i != e)
		{
			// I'll have to re-write this so that children know their sizes in
			// advance.  Technically "memory usage" is "number of variables".
			skip += (*i)->GetMemoryUsage();
			++i;
		}
		// Store the value of the alternate used.
		TRY(memory->Skip(skip));
		TRY(memory->WriteNextCell(alt));
		return OK;
		// Return the last error, whatever it was (may be useful, may not - will
		// be if there's only one alternate (but we can optimise that case)).
		// In fact we will HAVE to optimise that case or the "WriteNextCell"
		// above will fail, because when there is only one version, there is no
		// alternate write target.
	};
};

