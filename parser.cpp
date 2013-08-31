error_t
	Parser::
	Compile(
		char const * & input,
		Specifier ** dest)
{
	*dest = nullptr;
	try
	{
		// We ALWAYS start with an "alt" group.
		GlobalGroup *
			ret = new GlobalGroup(false); // Not ended by ')'.
		// TODO: These checks don't clean up properly.
		TRY(ret->ReadToken(input));
		Utils::SkipWhitespace(input);
		FAIL(*input == '\0', ERROR_EXPECTED_A_GOT_B, "<end of string>", *input);
		*dest = ret;
		// At this point, we could run an optimisation pass on the tree!  The
		// only one I can think of at this point is to replace "Alt" groups with
		// only one child by regular sequence groups.  Or merge hierarchical
		// sequences.
	}
	catch (std::bad_alloc & e)
	{
		// Clean up the allocated memory.
		delete *dest;
		// Catch any memory allocation errors.
		FAIL(false, ERROR_MEMORY_ALLOCATION_FAIL);
	}
	return OK;
}

error_t
	Parser::
	GetNext(
		char const * & input,
		Specifier ** dest)
{
	Utils::SkipWhitespace(input);
	*dest = nullptr;
	char
		c = *input;
	if (c)
	{
		// Get the specifier type from the list of known types, without ever
		// needing to know the class of the object ("Prototype pattern").
		FAIL(m_specifiers[c], ERROR_UNKNOWN_SPECIFIER);
		TRY(m_specifiers[c]->Clone(dest));
		return (*dest)->ReadToken(input);
	}
	return OK;
}














enum E_SSCANF_ERROR
{
	OK,
	ERROR_NO_TOKEN,
	ERROR_CHILD_NOT_SPECIFIER,
};

typedef
	enum E_SSCANF_ERROR
	error_t;

static int
	CUtils::GetUnsigned(char ** c)
{
	int
		n = 0;
	while ('0' <= **c && **c <= '9')
	{
		n = n * 10 + **c - '0';
		*c++;
	}
	return n;
}

static char
	CUtils::GetOneChar(char ** c)
{
	char
		cur = **c;
	*c++;
	if (cur == '\\')
	{
		cur = **c;
		*c++;
		switch (cur)
		{
			case 'a': case 'A':
				cur = '\a';
				break;
			case 'b': case 'B':
				cur = '\b';
				break;
			case 'f': case 'F':
				cur = '\f';
				break;
			case 'n': case 'N':
				cur = '\n';
				break;
			case 'r': case 'R':
				cur = '\r';
				break;
			case 't': case 'T':
				cur = '\t';
				break;
			case 'v': case 'V':
				cur = '\v';
				break;
			case 'u': case 'U':
				cur = '\n';
				break;
			case 'x': case 'X':
				cur = '\n';
				break;
			case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
				cur = cur - '0';
				break;
		}
	}
	return cur;
}

error_t
	ParseSpecifierCount(char ** c, CTokens &tokens)
{
	int
		n = CUtils::GetUnsigned(c);
	CTokens
		child;
	error_t
		e = GetToken(c, child);
	if (e) return e;
	if (child.Size() == 0) return ERROR_NO_CHILD;
	if (child.Size() != 1) return ERROR_NOT_ONE_CHILD;
	CToken
		theOne = child[0];
	if (!theOne.IsSpecifier()) return ERROR_CHILD_NOT_SPECIFIER;
	// Add this token on to the main set of tokens multiple times.
	while (n--) tokens.Add(theOne);
	return OK;
}

error_t
	GetToken(char const * & input, CTokens &tokens)
{
	SkipWhitespace(input);
	unsigned char
		c = (unsigned char)(*input);
	switch (c)
	{
		case '\0': return ERROR_NO_TOKEN;
		case '|': // Very special case.
	}
	Specifier *
		s = GetSpecifier(c);
	if (!s) return UNKNOWN_SPECIFIER
	tokens.Add(s);
	
	
	
	





	if (**c == '\0')
	{
		// End of the line.
		return ERROR_NO_TOKEN;
	}
	else if ('0' <= **c && **c <= '9')
	{
		// A number (specifically an integer).
		return ParseSpecifierCount(c, tokens);
	}
	else if ('a' <= **c && **c <= 'z')
	{
		// A specifier.
		return ParseSpecifier(c, tokens);
	}
	else if ('A' <= **c && **c <= 'Z')
	{
		// An optional specifier.
		return ParseOptionalSpecifier(c, tokens);
	}
	else switch (*c)
	{
		// A special character.
		case '|':
			// Alternate specifier.
			break;
		case '*':
			// Variable input count.
			break;
		case '-':
			// Skipped element.
			break;
		case '(':
			// Group start.
			break;
		case ')':
			// Group end.
			break;
		case '?':
			// Option.
			break;
		case '%':
			// Format type.
			break;
		case '{':
			// Quiet mode start.
			break;
		case '}':
			// Quiet mode end.
			break;
		case '\'':
		case '\"':
			// String literal.
			break;
	}
}

/*

TODO:
- Remove the requirement of quiet strings to contain lengths.
- All the new operators.

*/
