#pragma once

#include "../specifiers.h"
#include "../specifiers/other_specifiers.h"
#include "../parser.h"
#include "../errors.h"

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
	
	std::list<Specifier *>::const_iterator
		Begin() const
	{
		return m_children.begin();
	};
	
	std::list<Specifier *>::const_iterator
		End() const
	{
		return m_children.end();
	};
	
	virtual int
		CountChildren() const { return m_children.size(); };
	
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
			TRY(gParser.GetNext(input, &child));
		}
		while (child && child->GetSpecifier() != '}');
		FAIL(child, ERROR_NO_QUIET_END);
		return OK;
	};
	
	CLONE();
	
	// This needs to count all READS, but not WRITES.
	virtual int
		Skip(Environment & env) { return 0; };
	
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
	
	virtual cell
		Skip(Environment & env)
	{
		// "-1" means "unknown" or "variable".
		if (m_memory == -1)
		{
			cell
				ret = 0,
				cur = 0;
			for (auto i = Begin(), e = End(); i != e; ++i)
			{
				cur = (*i)->Skip(env);
				if (cur == -1) ret = -1;
				else if (ret != -1) ret += cur;
			}
			return (m_memory = ret);
		}
		else
		{
			env.Skip(m_memory);
			return m_memory;
		}
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		error_t
			fail = OK;
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			if (fail)
			{
				// Skip the remaining associated memory.
				(*i)->Skip(env);
			}
			else
			{
				fail = (*i)->Run(input, env);
				if (fail) (*i)->Skip(env);
				else fail = env.SkipDelimiters(input);
			}
		}
		return fail;
	};
	
	virtual std::ostream &
		Render(std::ostream & out) const
	{
		//out << "(";
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			(*i)->Render(out);
		}
		return out; // << ")";
	};
	
private:
	int
		m_memory;
};

class AltGroup : public SpecifierGroup
{
public:
	// cons
		AltGroup(bool local = true)
	:
		m_local(local),
		m_storeAlt(true),
		m_memory(-1),
		SpecifierGroup('(')
	{
	};
	
	// cons
		AltGroup(AltGroup const & that)
	:
		m_local(that.m_local),
		m_storeAlt(that.m_storeAlt),
		m_memory(-1),
		SpecifierGroup(that)
	{
		//Skip(env);
	};
	
	CLONE();
	
	// Local.
	virtual error_t
		ReadToken(char const * & input)
	{
		if (m_local) ++input;
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
				FAIL(alt->Begin() != alt->End(), ERROR_NO_CHILDREN);
ReadToken_new_alt:
				alt = new SequentialGroup();
				Add(alt);
			}
			else
			{
				alt->Add(child);
			}
			TRY(gParser.GetNext(input, &child));
			if (m_local && child->GetSpecifier() == ')') break;
		}
		while (child);
		FAIL(alt->Begin() != alt->End(), ERROR_NO_CHILDREN);
		if (m_local)
		{
			FAIL(child, ERROR_NO_GROUP_END);
		}
		return OK;
	};
		
	virtual cell
		Skip(Environment & env)
	{
		// "-1" means "unknown" or "variable".
		if (m_memory == -1)
		{
			cell
				ret = 0,
				cur = 0;
			for (auto i = Begin(), e = End(); i != e; ++i)
			{
				cur = (*i)->Skip(env);
				if (cur == -1) ret = -1;
				else if (ret != -1) ret += cur;
			}
			if (m_storeAlt)
			{
				if (ret != -1) ++ret;
				env.Skip(1);
			}
			return (m_memory = ret);
		}
		else
		{
			env.Skip(m_memory);
			return m_memory;
		}
	};
	
	// virtual error_t
		// Run(char const * & input, Environment & env)
	// {
		// for (auto i = Begin(), e = End(); i != e; ++i)
		// {
			// TRY((*i)->Run(input, env));
			// TRY(env.SkipDelimiters(input));
		// }
		// return OK;
	// };
	
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
		while (i != e)
		{
			// I'll have to re-write this so that children know their sizes in
			// advance.  Technically "memory usage" is "number of variables".
			(*i)->Skip(env);
			++i;
		}
		// Store the value of the alternate used.
		if (m_storeAlt) TRY(env.SetNextValue(alt));
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
		if (m_local) out << "(";
		for (auto i = Begin(), e = End(); ; )
		{
			(*i)->Render(out);
			++i;
			if (i == e) break;
			out << "|";
		}
		if (m_local) out << ")";
		return out;
	};
	
private:
	bool
		m_storeAlt,
		m_local;
	
	int
		m_memory;
	
	CTEST(AltGroup1,  { AltGroup gg; return gg.ReadToken(S"(ii|dd)") == OK && *CUR == '\0'; })
	CTEST(AltGroup2,  { AltGroup gg; gg.ReadToken(S"(ii|xh)");ss s; return dynamic_cast<ss &>(s << gg).str() == "(ii|xh)"; })
	CTEST(AltGroup4,  { AltGroup gg; if (gg.ReadToken(S"(ii|dd|cc|xx)") != OK) return false;
		int i = 0; for (auto b = gg.Begin(), e = gg.End(); b != e; ++b) ++i; return i == 4 && gg.CountChildren() == 4; })
	CTEST(AltGroup5,  { AltGroup gg; if (gg.ReadToken(S"(ii|dd|cc|xx|dd|dd|dd)") != OK) return false;
		int i = 0; for (auto b = gg.Begin(), e = gg.End(); b != e; ++b) { ++i; if ((*b)->CountChildren() != 2) return false; } return i == 7 && gg.CountChildren() == 7; })
	CTEST(AltGroup6,  { AltGroup gg; if (gg.ReadToken(S"(ii)") != OK) return false;
		int i = 0; for (auto b = gg.Begin(), e = gg.End(); b != e; ++b) ++i; return i == 1 && gg.CountChildren() == 1; })
	
	CTEST(GlobGroup1,  { AltGroup gg(false); return gg.ReadToken(S"ii|dd") == OK && *CUR == '\0'; })
	CTEST(GlobGroup2,  { AltGroup gg(false); return gg.ReadToken(S"ii|") == ERROR_NO_CHILDREN; })
	CTEST(GlobGroup3,  { AltGroup gg(false); return gg.ReadToken(S"|dd") == ERROR_NO_CHILDREN; })
	CTEST(GlobGroup6,  { AltGroup gg(false); return gg.ReadToken(S"I(5)") == OK; })
	CTEST(GlobGroup7,  { AltGroup gg(false); return gg.ReadToken(S"I(q)") == ERROR_NAN; })
	CTEST(GlobGroup4,  { AltGroup gg(false); return gg.ReadToken(S"ii|D(5)|cc|  H(11)n") == OK && *CUR == '\0'; })
	CTEST(GlobGroup5,  { AltGroup gg(false); gg.ReadToken(S"ii|xh");ss s; return dynamic_cast<ss &>(s << gg).str() == "ii|xh"; })
};
