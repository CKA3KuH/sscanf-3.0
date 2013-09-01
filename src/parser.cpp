error_t
	Parser::
	Compile(
		char const * & input,
		Specifier ** dest)
{
	*dest = nullptr;
	try
	{
		// We ALWAYS start with an "alt" group (the special one written just for
		// this purpose and that doesn't end with a close bracket).
		*dest = new GlobalGroup();
		// TODO: These checks don't clean up properly.
		TRY((*dest)->ReadToken(input));
		Utils::SkipWhitespace(input);
		FAIL(*input == '\0', ERROR_EXPECTED_A_GOT_B, "<end of string>", *input);
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

error_t
	Parser::
	Add(Specifier * prototype)
{
	return AddAs(prototype, prototype->GetSpecifier());
}

error_t
	Parser::
	AddAs(Specifier * prototype, char specifier)
{
	FAIL(m_specifiers[specifier] == nullptr, ERROR_DUPLICATE_SPECIFIER);
	m_specifiers[specifier] = prototype;
	if ('a' <= specifier && specifier <= 'z')
	{
		// Add the optional variant.
		return AddAs(prototype, specifier & ~0x20);
	}
	return OK;
}

error_t
	Parser::
	AddAll()
{
	Specifier *
		temp;
	
	// There is no reason in this design why we can't have different parsers
	// with different specifier sets.  That's pretty cool IMHO.
	Add(new SimpleSpecifier('b', &Utils::ReadBinary ));
	Add(new SimpleSpecifier('d', &Utils::ReadDecimal));
	Add(new SimpleSpecifier('f', &Utils::ReadFloat  ));
	Add(new SimpleSpecifier('g', &Utils::ReadIEEE754));
	Add(new SimpleSpecifier('h', &Utils::ReadHex    ));
	Add(new SimpleSpecifier('i', &Utils::ReadDecimal));
	Add(new SimpleSpecifier('l', &Utils::ReadLogical));
	Add(new SimpleSpecifier('n', &Utils::ReadNumber ));
	Add(new SimpleSpecifier('o', &Utils::ReadOctal  ));
	Add(new SimpleSpecifier('x', &Utils::ReadHex    ));
	Add(new SimpleSpecifier('c', &Utils::ReadCharEx ));
	
	// Detect group closes.
	Add(new TrivialSpecifier('|')); // Alternate.
	Add(new TrivialSpecifier(')')); // End group.
	Add(new TrivialSpecifier('}')); // End quiet mode.
	Add(new TrivialSpecifier('>')); // End subtype.
	
	// Add all 10 numeric specifiers.
	temp = new NumericSpecifier();
	AddAs(temp, '0');
	AddAs(temp, '1');
	AddAs(temp, '2');
	AddAs(temp, '3');
	AddAs(temp, '4');
	AddAs(temp, '5');
	AddAs(temp, '6');
	AddAs(temp, '7');
	AddAs(temp, '8');
	AddAs(temp, '9');
	AddAs(temp, '*'); // Special case - unknown count.
	
	// Add string literals.
	temp = new LiteralSpecifier();
	AddAs(temp, '\'');
	AddAs(temp, '\"');
	
	// More complex specifiers.
	Add(new DelimSpecifier());  // 'p'.
	Add(new ArraySpecifier());  // 'a'.
	Add(new KustomSpecifier()); // 'k'.
	AddAs(new StringSpecifier(false), 's'); // Unpacked.
	AddAs(new StringSpecifier(true ), 'z'); // Packed.
	AddAs(new PlayerSpecifier(false, true ), 'q'); // Bots only.
	AddAs(new PlayerSpecifier(true,  false), 'r'); // Players only.
	AddAs(new PlayerSpecifier(true,  true ), 'u'); // Players and Bots.
	
	// Complex groups.
	Add(new EnumSpecifier()); // 'e'.
	Add(new QuietGroup());    // '{'.
	Add(new AltGroup());      // '('.
	
	// Others.
	Add(new SkipSpecifier());   // '-'.
	Add(new PlusSpecifier());   // '+'.
	Add(new OptionSpecifier()); // '?'.
}

