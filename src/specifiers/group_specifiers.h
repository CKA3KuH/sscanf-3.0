#pragma once

#include "../specifiers.h"
#include <list>

// This is the class representing some collection of multiple other specifiers.
class SpecifierGroup : public Specifier
{
public:
	// cons
		SpecifierGroup(char c)
	:
		Specifier(c),
		m_children()
	{
	};
	
	// cons
		SpecifierGroup(SpecifierGroup const & that)
	:
		Specifier(that),
		m_children()
	{
	};
	
	void
		Add(Specifier * child)
	{
		m_children.push_back(child);
	};
	
	std::list<Specifier *>::iterator
		Begin()
	{
		return m_children.begin();
	};
	
	std::list<Specifier *>::iterator
		End()
	{
		return m_children.end();
	};
	
private:
	std::list<Specifier *>
		m_children;
};

/*
class QuietGroup : public SpecifierGroup
{
public:
	// cons
		QuietGroup()
	:
		SpecifierGroup('{')
	{
	};
	
	// cons
		QuietGroup(QuietGroup const & that)
	:
		SpecifierGroup(that)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input)
	{
		++input;
		Specifier *
			child;
		// Avoids repetition of code.
		goto ReadToken_new_quiet;
		do
		{
			Add(child);
ReadToken_new_quiet:
			TRY(gParser->GetNext(input, &child));
		}
		while (child && child->GetSpecifier() != '}');
		FAIL(child, ERROR_NO_QUIET_END);
		MinusSpecifier::DeleteTrivial(child);
		return OK;
	};
	
	CLONE();
	
	// This needs to count all READS, but not WRITES.
	virtual int
		GetMemoryUsage() { return 0; };
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		// Store a copy of the old memory system.
		Memory *
			om = env.GetMemory();
		// Just takes all writes and null-routes them, but still serves reads.
		QuietMemory
			quiet(env.GetMemory());
		env.SetMemory(&quiet);
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			TRY((*i)->Run(input, env));
			TRY(env.SkipDelimiters());
		}
		// Restore the old memory system.
		env.SetMemory(om);
		return OK;
	};
	
	virtual std::ostream &
		Render(std::ostream & out) const
	{
		out = out << "{";
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			(*i)->Render(out);
		}
		return out << "}";
	};
};
*/

// This is just a set of specifiers, to be run one after the other - simple as!
class SequentialGroup : public SpecifierGroup
{
public:
	// cons
		SequentialGroup()
	:
		m_memory(-1),
		SpecifierGroup('\0') // Invalid specifier (can't be reached).
	{
	};
	
	// cons
		SequentialGroup(SequentialGroup const & that)
	:
		m_memory(-1),
		SpecifierGroup(that)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input)
	{
		FAIL(false, ERROR_PARSE_SEQUENTIAL_GROUP);
		return OK;
	};
	
	CLONE();
	
	virtual int
		GetMemoryUsage()
	{
		if (m_memory != -1) return m_memory;
		m_memory = 0;
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			m_memory += (*i)->GetMemoryUsage();
		}
		return m_memory;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			TRY((*i)->Run(input, env));
			TRY(env.SkipDelimiters());
		}
		return OK;
	};
	
	virtual std::ostream &
		Render(std::ostream & out) const
	{
		out = out << "(";
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			(*i)->Render(out);
		}
		return out << ")";
	};
	
private:
	int
		m_memory;
}

class AltGroup : public SpecifierGroup
{
public:
	// cons
		AltGroup()
	:
		m_storeAlt(true),
		m_memory(-1),
		SpecifierGroup('(')
	{
	};
	
	// cons
		AltGroup(AltGroup const & that)
	:
		m_storeAlt(that.m_storeAlt),
		m_memory(-1),
		SpecifierGroup(that)
	{
		GetMemoryUsage();
	};
	
	CLONE();
	
	// Local.
	virtual error_t
		ReadToken(char const * & input)
	{
		++input;
		Specifier *
			child;
		SequentialGroup *
			alt;
		// Avoids repetition of code.
		goto ReadToken_new_alt;
		do
		{
			if (child->GetSpecifier() == '|')
			{
				MinusSpecifier::DeleteTrivial(child);
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
		while (child && child->GetSpecifier() != ')');
		FAIL(child, ERROR_NO_GROUP_END);
		MinusSpecifier::DeleteTrivial(child);
		return OK;
	};
	
	virtual int
		GetMemoryUsage()
	{
		if (m_memory != -1) return m_memory;
		m_memory = m_storeAlt ? 1 : 0;
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			m_memory += (*i)->GetMemoryUsage();
		}
		return m_memory;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		error_t
			last = OK;
		cell
			alt = 0;
		auto
			i = Begin(),
			e = End();
		// Parse this alternate.
		while (i != e)
		{
			char const *
				start = input;
			// Make a copy of the current state of the delimiters and pass THAT
			// to children so that it resets for every alternate branch.
			Environment
				local(env);
			last = (*i)->Run(start, local);
			++i;
			if (last == OK)
			{
				// Found a group that works.
				input = start;
				break;
			}
			++alt;
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
		TRY(env.GetMemory()->Skip(skip));
		if (m_storeAlt) TRY(env.GetMemory()->WriteNextCell(alt));
		return OK;
		// Return the last error, whatever it was (may be useful, may not - will
		// be if there's only one alternate (but we can optimise that case)).
		// In fact we will HAVE to optimise that case or the "WriteNextCell"
		// above will fail, because when there is only one version, there is no
		// alternate write target.
	};
	
	virtual std::ostream &
		Render(std::ostream & out) const
	{
		out = out << "(";
		for (auto i = Begin(), e = End(); ; )
		{
			(*i)->Render(out);
			++i;
			if (i == e) break;
			out = out << "|";
		}
		return out << ")";
	};
	
private:
	bool
		m_storeAlt;
	
	int
		m_memory;
};

class GlobalGroup : public AltGroup
{
public:
	// cons
		GlobalGroup()
	:
		AltGroup() // Derived from that, but not in the valid parser list.
	{
	};
	
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
			if (child->GetSpecifier() == '|')
			{
				MinusSpecifier::DeleteTrivial(child);
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

