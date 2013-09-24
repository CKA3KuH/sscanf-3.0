#pragma once

#include "../specifiers.h"
#include "../specifiers/other_specifiers.h"
#include "../parser.h"
#include "../errors.h"

#include <list>

// This is the class representing some collection of multiple other specifiers.
class SpecifierGroup : public Specifier
{
PUBLIC:
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
	
	// dest
		~SpecifierGroup()
	{
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			delete (*i);
		}
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
	
PRIVATE:
	std::list<Specifier *>
		m_children;
	
	friend class Parser;
};

class QuietGroup : public SpecifierGroup
{
PUBLIC:
	// cons
		QuietGroup()
	:
		SpecifierGroup('{'),
		m_unknown(true)
	{
	};
	
	// cons
		QuietGroup(QuietGroup const & that)
	:
		SpecifierGroup(that),
		m_unknown(that.m_unknown)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input);
	
	CLONE();
	
	// This needs to count all READS, but not WRITES.
	virtual cell
		Skip(Environment & env);
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
	virtual std::ostream &
		Render(std::ostream & out) const;
	
PRIVATE:
	// Number of readable children is unknown (or not 0).
	bool
		m_unknown;
};

// This is just a set of specifiers, to be run one after the other - simple as!
class SequentialGroup : public SpecifierGroup
{
PUBLIC:
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
		Skip(Environment & env);
	
	virtual error_t
		Run(char const * & input, Environment & env);
	
	virtual std::ostream &
		Render(std::ostream & out) const;
	
PRIVATE:
	int
		m_memory;
};

class AltGroup : public SpecifierGroup
{
PUBLIC:
	// cons
		AltGroup(bool local = true)
	:
		m_sequential(true),
		m_local(local),
		m_storeAlt(true),
		m_memory(-1),
		SpecifierGroup('(')
	{
	};
	
	// cons
		AltGroup(AltGroup const & that)
	:
		m_sequential(that.m_sequential),
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
		ReadToken(char const * & input);
	
	virtual cell
		Skip(Environment & env);

	virtual error_t
		Run(char const * & input, Environment & env);
	
	virtual std::ostream &
		Render(std::ostream & out) const;
	
PRIVATE:
	bool
		m_sequential,
		m_storeAlt,
		m_local;
	
	int
		m_memory;
};

