#pragma once

// This is the class representing some collection of multiple other specifiers.
class SpecifierGroup : public Specifier
{
protected:
	std::list<Specifier *>
		m_children;
};

class QuietGroup : public SpecifierGroup
{
public:
	// This needs to count all READS, but not WRITES.
	int
		GetMemoryUsage() { return 0; };
	
	error_t
		Run(char const * & input, Memory * memory, Delimiters * delimiters)
	{
		// Just takes all writes and null-routes them, but still serves reads.
		QuietMemory
			quiet(memory);
		for (var i = m_children.begin(), e = m_children.end(); i != e; ++i)
		{
			TRY((*i)->Run(input, quiet, delimiters));
		}
		return OK;
	};
};

// This is just a set of specifiers, to be run one after the other - simple as!
class SequentialGroup : public SpecifierGroup
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
		Run(char const * & input, Memory * memory, Delimiters * delimiters)
	{
		for (var i = m_children.begin(), e = m_children.end(); i != e; ++i)
		{
			TRY((*i)->Run(input, memory, delimiters));
		}
		return OK;
	};
}

class AltGroup : public SpecifierGroup
{
public:
	// Local.
	virtual error_t
		ReadToken(char const * & input)
	{
		FAIL(*input == '(', ERROR_NO_GROUP_START);
		Specifier *
			child;
		SequentialGroup *
			alt;
		// Avoids repetition of code.
		goto ReadToken_new_alt;
		do
		{
			if (child->m_specifier == '|')
			{
ReadToken_new_alt:
				alt = new SequentialGroup();
				Add(alt);
			}
			else
			{
				alt->Add(child);
			}
			TRY(gParser->GetNext(input, &child));
		}
		while (child && child->m_specifier != ')');
		return child ? OK : ERROR_NO_GROUP_END;
		// Only enters this when it hits a ')' specifier.  '|' and ')' are
		// (currently) the only two UBER simple specifiers that just consume a
		// single character, do nothing when run, and are discarded instantly.
		// Actually, don't delete them, just use featherweights.  '>' is also
		// one to end sub-specifiers, and '}' to end quiet sections.
	};
	
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
		Run(char const * & input, Memory * memory, Delimiters * delimiters)
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
			// Make a copy of the current state of the delimiters and pass THAT
			// to children so that it resets for every alternate branch.
			// 
			// Example:
			// 
			//  p<,>ii|ff
			// 
			// That denotes either 2 integers separated by commas, or 2 floats.
			// 
			//  p<,>(ii|ff)
			// 
			// That denotes either 2 integers or 2 floats, separated by commas.
			// 
			//  p<,>(ii|p<;>ff)
			// 
			// That denotes either 2 integers separated by commas, or 2 floats
			// separated by either commas or semicolons.
			// 
			//  p<,>(ii|-p<,>ff)
			// 
			// That has the same effect as the first example.
			// 
			// SCRAP THAT.  New "p" syntax:
			// 
			//  p<,> <- Set "," as the ONLY separator.
			//  p<> <- Reset separators (i.e. any whitespace).
			//  p< > <- Set " " as the ONLY separator (no tabs, newlines etc).
			//  +p<,> <- Add "," to the list of possible separators.
			//  -p<,> <- Remove "," from the list of possible separators.
			//  +P<,> <- Add ONE OR MORE commas to the list of separators.
			// 
			// Note that doing "P<>" is like doing "+P< >+P<\t>+P<\r>+P<\n>".
			// This makes "P<>" the default - i.e. any length of any whitespace
			// between two values.
			// 
			Delimiters
				delims(*delimiters);
			last = (*i)->Run(start, memory, &delims);
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

class GlobalGroup : public AltGroup
{
public:
	// Global.
	error_t
		ReadToken(char const * & input)
	{
		Specifier *
			child;
		SequentialGroup *
			alt;
		// Avoids repetition of code.
		goto ReadToken_new_alt;
		do
		{
			if (child->m_specifier == '|')
			{
ReadToken_new_alt:
				alt = new SequentialGroup();
				Add(alt);
			}
			else
			{
				alt->Add(child);
			}
			TRY(gParser->GetNext(input, &child));
		}
		while (child);
		return OK;
	};
};

