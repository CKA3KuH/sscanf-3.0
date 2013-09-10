#include "parser.h"
#include "utils.h"
#include "specifiers.h"
/*#include "specifiers/array_specifier.hpp"
#include "specifiers/delim_specifier.hpp"
#include "specifiers/group_specifiers.hpp"
#include "specifiers/enum_specifier.hpp" // Relies on group_specifiers.
#include "specifiers/player_specifier.hpp"
#include "specifiers/string_specifier.hpp"*/
#include "specifiers/numeric_specifier.h"
#include "specifiers/trivial_specifiers.h"
#include "specifiers/simple_specifiers.h"
#include "specifiers/other_specifiers.h"
#include "specifiers/group_specifiers.h"

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

TEST(PC0, { Specifier * spec; return gParser.Compile(S"", &spec) == ERROR_NO_CHILDREN; })
TEST(PC1, { Specifier * spec; return gParser.Compile(S"ii", &spec) == OK && spec->GetSpecifier() == '\0'; })
TEST(PC5, { Specifier * spec; return gParser.Compile(S"iiq", &spec) == ERROR_UNKNOWN_SPECIFIER; })
TEST(PC2, { Specifier * spec; return gParser.Compile(S"ii|dd", &spec) == OK && spec->GetSpecifier() == '('; })
TEST(PC3, { Specifier * spec; return gParser.Compile(S"i|X(0xFF)", &spec) == OK && spec->GetSpecifier() == '(' && spec->CountChildren() == 2; })

TEST(PC4a, { Specifier * spec; return gParser.Compile(S"iC('q')|(x|x)", &spec) == OK && spec->GetSpecifier() == '('; })
TEST(PC4c, { Specifier * spec; return gParser.Compile(S"iC('q')-|(x|-x-)", &spec) == OK && spec->GetSpecifier() == '('; })
TEST(PC4b, { Specifier * spec; gParser.Compile(S"i C ( 'q')   |(   x|x)", &spec); ss s; return
	dynamic_cast<ss &>(s << *spec).str() == "ic|(x|x)"; })

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

ITEST(Parser, Parser1)

TEST(GN7,  { Specifier * s = nullptr; return gParser.GetNext(S"\xFF;", &s) == ERROR_UNKNOWN_SPECIFIER; })
TEST(GN8,  { Specifier * s = nullptr; return gParser.GetNext(S"		", &s) == OK; })
TEST(GN9,  { Specifier * s = nullptr; return gParser.GetNext(S"~", &s) == ERROR_UNKNOWN_SPECIFIER; })
TEST(GN10, { Specifier * s = nullptr; return gParser.GetNext(S"^", &s) == ERROR_UNKNOWN_SPECIFIER; })

TEST(GN0,  { Specifier * s = nullptr; return gParser.GetNext(S"i", &s) == OK && s && s->GetSpecifier() == 'i'; })
TEST(GN1,  { Specifier * s = nullptr; return gParser.GetNext(S"", &s) == OK && !s; })
TEST(GN2,  { Specifier * s = nullptr; return gParser.GetNext(S"d", &s) == OK && s && s->GetSpecifier() == 'd'; })
TEST(GN3,  { Specifier * s = nullptr; return gParser.GetNext(S"x", &s) == OK && s && s->GetSpecifier() == 'x'; })
TEST(GN4,  { Specifier * s = nullptr; return gParser.GetNext(S"'", &s) == ERROR_NO_STRING_END; })

TEST(GN5,  { Specifier * s = nullptr; char const * p = "oxc"; return
	gParser.GetNext(p, &s) == OK && s && s->GetSpecifier() == 'o' &&
	gParser.GetNext(p, &s) == OK && s && s->GetSpecifier() == 'x' &&
	gParser.GetNext(p, &s) == OK && s && s->GetSpecifier() == 'c' &&
	gParser.GetNext(p, &s) == OK && !s; })

TEST(GN6,  { Specifier * s = nullptr; char const * p = "   o   x   c   "; return
	gParser.GetNext(p, &s) == OK && s && s->GetSpecifier() == 'o' &&
	gParser.GetNext(p, &s) == OK && s && s->GetSpecifier() == 'x' &&
	gParser.GetNext(p, &s) == OK && s && s->GetSpecifier() == 'c' &&
	gParser.GetNext(p, &s) == OK && !s; })

TEST(GN11, { Specifier * s = nullptr; return gParser.GetNext(S"I(45)", &s) == OK && s && s->GetSpecifier() == 'i'; })

TEST(GN12, { Specifier * s = nullptr; char const * p = "   B  (  0b001101  )   d  H(FF) "; return
	gParser.GetNext(p, &s) == OK && s && s->GetSpecifier() == 'b' &&
	gParser.GetNext(p, &s) == OK && s && s->GetSpecifier() == 'd' &&
	gParser.GetNext(p, &s) == OK && s && s->GetSpecifier() == 'h' &&
	gParser.GetNext(p, &s) == OK && !s; })

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
	TRY(Add(new SimpleSpecifier('d', &Utils::ReadDecimal)));
//	TRY(Add(new SimpleSpecifier('f', &Utils::ReadFloat  )));
//	TRY(Add(new SimpleSpecifier('g', &Utils::ReadIEEE754)));
	TRY(Add(new SimpleSpecifier('h', &Utils::ReadHex    )));
	TRY(Add(new SimpleSpecifier('i', &Utils::ReadDecimal)));
//	TRY(Add(new SimpleSpecifier('l', &Utils::ReadLogical)));
	TRY(Add(new SimpleSpecifier('o', &Utils::ReadOctal  )));
	TRY(Add(new SimpleSpecifier('x', &Utils::ReadHex    )));
	TRY(Add(new SimpleSpecifier('c', &Utils::ReadCharEx )));
	TRY(Add(new NumSpecifier())); // 'n'.
	
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
	TRY(Add(new DelimSpecifier()));  // 'p'.
	TRY(Add(new ArraySpecifier()));  // 'a'.
	TRY(Add(new KustomSpecifier())); // 'k'.
	TRY(AddAs(new StringSpecifier(false), 's')); // Unpacked.
	TRY(AddAs(new StringSpecifier(true ), 'z')); // Packed.
	TRY(AddAs(new PlayerSpecifier(false, true ), 'q')); // Bots only.
	TRY(AddAs(new PlayerSpecifier(true,  false), 'r')); // Players only.
	TRY(AddAs(new PlayerSpecifier(true,  true ), 'u')); // Players and Bots.
	
	// Complex groups.
	TRY(Add(new EnumSpecifier())); // 'e'.
	TRY(Add(new QuietGroup()));    // '{'.*/
	TRY(Add(new AltGroup()));      // '('.
	
	// Others.
	TRY(Add(new MinusSpecifier())); // '-'.
	/*
	// Others.
	TRY(Add(new SkipSpecifier()));   // '-'.
	//TRY(Add(new PlusSpecifier()));   // '+'.
	TRY(Add(new OptionSpecifier())); // '?'.*/
	return OK;
}

