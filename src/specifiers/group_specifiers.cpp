#pragma once

#include "group_specifiers.h"
#include "../specifiers/other_specifiers.h"
#include "../parser.h"
#include "../errors.h"

cell
	SequentialGroup::
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
}

error_t
	SequentialGroup::
	Run(char const * & input, Environment & env)
{
	for (auto i = Begin(), e = End(); i != e; ++i)
	{
		TRY((*i)->Run(input, env));
		TRY(env.SkipDelimiters(input));
	}
	return OK;
}

std::ostream &
	SequentialGroup::
	Render(std::ostream & out) const
{
	//out << "(";
	for (auto i = Begin(), e = End(); i != e; ++i)
	{
		(*i)->Render(out);
	}
	return out; // << ")";
}

// Local.
error_t
	AltGroup::
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
			// Takes the value of any "|" in the alt.
			m_sequential = m_sequential && !child->GetSkip();
			delete child;
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
		// Globals can't NOT store the branch taken.
		m_storeAlt = !child->GetSkip();
		delete child;
	}
	else
	{
		// Store the alternate branch taken if there are any alternatives.
		m_storeAlt = CountChildren() > 1;
	}
	return OK;
}

cell
	AltGroup::
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
}

error_t
	AltGroup::
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
		// Also mark our place in memory.
		int
			initial = env.Poll();
		last = (*i)->Run(start, local);
		Specifier *
			prev = *i;
		++i;
		if (last == OK)
		{
			// Found a group that works.
			input = start;
			break;
		}
		else if (i == e) return last;
		else
		{
			env.Skip(initial - env.Poll()); // Rewind.
			if (m_sequential) prev->Skip(env); // Skip everything.
			// If the data is not stored sequentially, then it must be
			// stored in parallel, i.e. all alts go in the same slots.  In
			// that case, after we have rewound to the start of the current
			// memory block, there's no need to skip all the data.
			// 
			// WARNING:
			// 
			// This is probably an undetectable bug:
			//  
			//  sscanf("0x11 0x12", "*i-|xx", 2, dest1, dest2);
			//  
			// In that case, the first alt will fail, and the second alt
			// will be written to the same memory locations.  However, as
			// the first alt used "*" the first parameter is intended to be
			// an input, not an output, but the second branch will still
			// write to it, and write the second hex number to "dest1"
			// (leaving "dest2" untouched - giving a console warning).
		}
		++alt;
	}
	// Find where to store the alternate.
	if (m_sequential)
	{
		while (i != e)
		{
			(*i)->Skip(env);
			++i;
		}
	}
	// Store the value of the alternate used.
	if (m_storeAlt) TRY(env.SetNextValue(alt));
	return OK;
	// Return the last error, whatever it was (may be useful, may not - will
	// be if there's only one alternate (but we can optimise that case)).
	// In fact we will HAVE to optimise that case or the "WriteNextCell"
	// above will fail, because when there is only one version, there is no
	// alternate write target.
}

std::ostream &
	AltGroup::
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
}

