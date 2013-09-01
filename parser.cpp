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
	return AddAs(prototype, prototype->m_specifier);
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
	gParser->Add(new SimpleSpecifier('b', &Utils::ReadBinary ));
	gParser->Add(new SimpleSpecifier('d', &Utils::ReadDecimal));
	gParser->Add(new SimpleSpecifier('f', &Utils::ReadFloat  ));
	gParser->Add(new SimpleSpecifier('g', &Utils::ReadIEEE754));
	gParser->Add(new SimpleSpecifier('h', &Utils::ReadHex    ));
	gParser->Add(new SimpleSpecifier('i', &Utils::ReadDecimal));
	gParser->Add(new SimpleSpecifier('l', &Utils::ReadLogical));
	gParser->Add(new SimpleSpecifier('n', &Utils::ReadNumber ));
	gParser->Add(new SimpleSpecifier('o', &Utils::ReadOctal  ));
	gParser->Add(new SimpleSpecifier('x', &Utils::ReadHex    ));
	
	// Detect group closes.
	gParser->Add(new EndingSpecifier('|')); // Alternate.
	gParser->Add(new EndingSpecifier(')')); // End group.
	gParser->Add(new EndingSpecifier('}')); // End quiet mode.
	gParser->Add(new EndingSpecifier('>')); // End subtype.
	
	// Add all 10 numeric specifiers.
	temp = new NumericSpecifier();
	gParser->AddAs(temp, '0');
	gParser->AddAs(temp, '1');
	gParser->AddAs(temp, '2');
	gParser->AddAs(temp, '3');
	gParser->AddAs(temp, '4');
	gParser->AddAs(temp, '5');
	gParser->AddAs(temp, '6');
	gParser->AddAs(temp, '7');
	gParser->AddAs(temp, '8');
	gParser->AddAs(temp, '9');
	gParser->AddAs(temp, '*'); // Special case - unknown count.
	
	// Add string literals.
	temp = new LiteralSpecifier();
	gParser->AddAs(temp, '\'');
	gParser->AddAs(temp, '\"');
	
	// More complex specifiers.
	gParser->Add(new DelimSpecifier());  // 'p'.
	gParser->Add(new CharSpecifier());   // 'c'.
	gParser->Add(new ArraySpecifier());  // 'a'.
	gParser->Add(new KustomSpecifier()); // 'k'.
	gParser->AddAs(new StringSpecifier(false), 's'); // Unpacked.
	gParser->AddAs(new StringSpecifier(true ), 'z'); // Packed.
	gParser->AddAs(new PlayerSpecifier(false, true ), 'q'); // Bots only.
	gParser->AddAs(new PlayerSpecifier(true,  false), 'r'); // Players only.
	gParser->AddAs(new PlayerSpecifier(true,  true ), 'u'); // Players and Bots.
	
	// Complex groups.
	gParser->Add(new EnumSpecifier()); // 'e'.
	gParser->Add(new QuietGroup());    // '{'.
	gParser->Add(new AltGroup());      // '('.
	
	// Others.
	gParser->Add(new SkipSpecifier());   // '-'.
	gParser->Add(new OptionSpecifier()); // '?'.
}

