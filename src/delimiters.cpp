#pragma once

#include "delimiters.h"

bool
	Delimiters::
	AtDelimiter(char const * input, bool incWhite) const
{
	// The only other valid delimiter (ALWAYS valid - the end of the string
	// by definition ends everything...
	if (*input == '\0') return true;
	else if (incWhite && (unsigned char)*input <= ' ') return true;
	else if (m_hasAny)
	{
		// The hard part - they're at a character that isn't whitespace,
		// determine if it matches any known explicit delimiter.  This
		// doesn't need to skip as many as possible, just identify at least
		// one.
		char const *
			d = m_delimStore.data();
		for (auto i = m_delimiters.begin(), e = m_delimiters.end(); i != e; ++i)
		{
			if (Delimiters::Match(input, d + *i)) return true;
		}
		for (auto i = m_optionals.begin(), e = m_optionals.end(); i != e; ++i)
		{
			if (Delimiters::Match(input, d + *i)) return true;
		}
	}
	return false;
}

bool
	Delimiters::
	SkipDelimiters(char const * & input) const
{
	if ((unsigned char)(*input - 1) < ' ')
	{
		do ++input;
		while ((unsigned char)(*input - 1) < ' ');
		// Reached the end of the string, or skipped at least one space and
		// we don't need an explicit delimiter.
		if (!m_hasAny) return true;
	}
	// Have some explicit specifiers to skip.
	if (m_hasAny)
	{
		char const *
			d = m_delimStore.data();
		for (auto i = m_delimiters.begin(), e = m_delimiters.end(); i != e; ++i)
		{
			if (Delimiters::Match(input, d + *i))
			{
				while ((unsigned char)(*input - 1) < ' ') ++input;
				return true;
			}
		}
		// "Optional" specifiers can appear multiple times as a
		// single delimiter.  But they can't appear in conjunction
		// with "Singular" specifiers.
		for (auto i = m_optionals.begin(), e = m_optionals.end(); i != e; ++i)
		{
			char const *
				d2 = d + *i;
			if (Delimiters::Match(input, d2))
			{
				do { while ((unsigned char)(*input - 1) < ' ') ++input; }
				while (Match(input, d2));
				return true;
			}
		}
	}
	// Didn't find an explicit delimiter, and didn't find any valid spaces.
	return *input == '\0';
}

bool
	Delimiters::
	Match(char const * & input, char const * delim)
{
	char const *
		cur = input;
	while (*cur && *delim) if (*cur++ != *delim++) return false;
	input = cur;
	return *delim == '\0';
}

void
	Delimiters::
	DoAdd(std::list<size_t> & dest, char const * delim)
{
	// Don't add pure whitespace to the list of possibles.  This does:
	//  
	//  if ('\0' < *delim <= ' ')
	//  
	// In a single check.
	//if ((unsigned char)(*delim - 1) < ' ' && *(delim + 1) == '\0') return;
	size_t
		len = strlen(delim) + 1,
		cap = m_delimStore.capacity();
	// Make sure there is enough space left.
	if (cap - m_write < len) m_delimStore.resize(cap + 16 + len);
	// Copy the data over.
	strcpy(m_delimStore.data() + m_write, delim);
	dest.push_back(m_write);
	m_write += len;
	// Now have at least one explicit delimiter.
	m_hasAny = true;
};

void
	Delimiters::
	DoRemove(std::list<size_t> & dest, char const * delim)
{
	char const *
		d = m_delimStore.data();
	for (auto i = dest.begin(), e = dest.end(); i != e; ++i)
	{
		if (Delimiters::Match(delim, d + *i))
		{
			dest.erase(i);
			return;
		}
	}
};

