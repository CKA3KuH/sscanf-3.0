#include "parser.h"
#include "utils.h"
#include "specifiers.h"
/*#include "specifiers/array_specifier.hpp"
#include "specifiers/delim_specifier.hpp"
#include "specifiers/group_specifiers.hpp"
#include "specifiers/enum_specifier.hpp" // Relies on group_specifiers.
#include "specifiers/player_specifier.hpp"*/
#include "specifiers/string_specifier.h"
#include "specifiers/numeric_specifier.h"
#include "specifiers/trivial_specifiers.h"
#include "specifiers/simple_specifiers.h"
#include "specifiers/other_specifiers.h"
#include "specifiers/group_specifiers.h"
#include "specifiers/delim_specifier.h"
#include "specifiers/specnum_specifiers.h"

Parser
	gParser;

#define DEL(dest) do { delete dest; dest = nullptr; } while (false);

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
		AltGroup *
			ret = new AltGroup(false);
		error_t
			e;
		// TODO: These checks don't clean up properly.
		e = ret->ReadToken(input);
		if (e != OK)
		{
			DEL(ret);
			return e;
		}
		Utils::SkipWhitespace(input);
		if (*input != '\0')
		{
			DEL(ret);
			FAIL(false, ERROR_EXPECTED_A_GOT_B_1, "<end of string>", *input);
		}
		// At this point, we could run an optimisation pass on the tree!  The
		// only one I can think of at this point is to replace "Alt" groups with
		// only one child by regular sequence groups.  Or merge hierarchical
		// sequences.
		if (ret->CountChildren() == 1)
		{
			// Return just a sequence.
			*dest = *ret->Begin();
			ret->m_children.clear();
			delete ret;
		}
		else *dest = ret;
	}
	catch (std::bad_alloc const &)
	{
		// Clean up the allocated memory.
		DEL(dest);
		// Catch any memory allocation errors.
		FAIL(false, ERROR_MEMORY_ALLOCATION_FAIL);
	}
	catch (...)
	{
		DEL(dest);
		FAIL(false, ERROR_UNKNOWN_ERROR);
	}
	return OK;
}

#undef DEL

error_t
	Parser::
	GetNext(
		char const * & input,
		Specifier ** dest)
{
	Utils::SkipWhitespace(input);
	*dest = nullptr;
	if (*input)
	{
		signed char
			c = *input - '!';
		FAIL(c >= 0 && c < sizeof (m_specifiers) / sizeof (Specifier *), ERROR_UNKNOWN_SPECIFIER, c + '!');
		// Get the specifier type from the list of known types, without ever
		// needing to know the class of the object ("Prototype pattern").
		FAIL(m_specifiers[c], ERROR_UNKNOWN_SPECIFIER, c + '!');
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
	FAIL(specifier >= '!' && specifier <= '~', ERROR_INVALID_SPECIFIER);
	FAIL(m_specifiers[specifier - '!'] == nullptr, ERROR_DUPLICATE_SPECIFIER);
	m_specifiers[specifier - '!'] = prototype;
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
	TRY(Add(new SimpleSpecifier('b', &Utils::ReadBinary )));
	TRY(Add(new SimpleSpecifier('c', &Utils::ReadCharEx )));
	TRY(Add(new SimpleSpecifier('d', &Utils::ReadDecimal)));
	TRY(Add(new FloatSpecifier ('f', &Utils::ReadFloat  )));
	TRY(Add(new FloatSpecifier ('g', &Utils::ReadIEEE   )));
	TRY(Add(new SimpleSpecifier('h', &Utils::ReadHex    )));
	TRY(Add(new SimpleSpecifier('i', &Utils::ReadDecimal)));
	TRY(Add(new LogicalSpecifier())); // 'l'.
	TRY(Add(new SimpleSpecifier('o', &Utils::ReadOctal  )));
	TRY(Add(new SimpleSpecifier('x', &Utils::ReadHex    )));
	// I don't know why I EVER made this a different class...
	TRY(Add(new SimpleSpecifier('n', &Utils::ReadNum    )));
	
	// Detect group closes.
	TRY(Add(new TrivialSpecifier('|'))); // Alternate.
	TRY(Add(new TrivialSpecifier(')'))); // End group.
	TRY(Add(new TrivialSpecifier('}'))); // End quiet mode.
	TRY(Add(new TrivialSpecifier('>'))); // End subtype.
	
	// Add all 10 numeric specifiers.
	temp = new NumericSpecifier();
	TRY(AddAs(temp, '0'));
	TRY(AddAs(temp, '1'));
	TRY(AddAs(temp, '2'));
	TRY(AddAs(temp, '3'));
	TRY(AddAs(temp, '4'));
	TRY(AddAs(temp, '5'));
	TRY(AddAs(temp, '6'));
	TRY(AddAs(temp, '7'));
	TRY(AddAs(temp, '8'));
	TRY(AddAs(temp, '9'));
	TRY(AddAs(temp, '*')); // Special case - unknown count.
	
	// Add string literals.
	temp = new LiteralSpecifier();
	TRY(AddAs(temp, '\''));
	TRY(AddAs(temp, '\"'));
	/*
	// More complex specifiers.
	TRY(Add(new ArraySpecifier()));  // 'a'.
	TRY(Add(new KustomSpecifier())); // 'k'.
	TRY(AddAs(new StringSpecifier(false), 's')); // Unpacked.*/
	TRY(Add(new StringSpecifier())); // Unpacked.
	//TRY(Add(new PackedSpecifier())); // Packed.
	// if (isbot ? allowbot : allowplayer)
	/*TRY(AddAs(new StringSpecifier(true ), 'z')); // Packed.
	TRY(AddAs(new PlayerSpecifier(false, true ), 'q')); // Bots only.
	TRY(AddAs(new PlayerSpecifier(true,  false), 'r')); // Players only.
	TRY(AddAs(new PlayerSpecifier(true,  true ), 'u')); // Players and Bots.
	
	// Complex groups.
	TRY(Add(new EnumSpecifier())); // 'e'.*/
	TRY(Add(new QuietGroup()));    // '{'.
	TRY(Add(new AltGroup()));      // '('.
	
	// Others.
	TRY(Add(new MinusSpecifier())); // '-'.
	TRY(Add(new DelimSpecifier())); // 'p'.
	TRY(Add(new OptionSpecifier())); // '?'
	return OK;
}

