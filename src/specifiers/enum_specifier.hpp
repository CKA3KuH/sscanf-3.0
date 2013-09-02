#pragma once

class EnumSpecifier : public SpecifierGroup
{
	error_t
		Run(char const * & input, Memory * memory, Delimiters * delimiters)
	{
		EnumMemory
			mem(memory->GetNextPointer());
		for (var i = m_children.begin(), e = m_children.end(); i != e; ++i)
		{
			TRY((*i)->Run(input, mem, delimiters));
		}
		return OK;
	}
};


class EnumSpecifier : public SpecifierGroup
{
public:
	// cons
		EnumSpecifier()
	:
		m_memory(-1),
		SpecifierGroup('e')
	{
	};
	
	// cons
		EnumSpecifier(EnumSpecifier const & that)
	:
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
		if (*input++ != GetSpecifier()) SetOptional();
		NEXT('<', ERROR_NO_TYPE_START);
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
		while (child && child->GetSpecifier() != '>');
		FAIL(child, ERROR_NO_GROUP_END);
		MinusSpecifier::DeleteTrivial(child);
		return OK;
	};
	
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
	
	error_t
		Run(char const * & input, Environment & env)
	{
		// Store a copy of the old memory system.
		Memory *
			om = env.GetMemory();
		// Just takes all writes and null-routes them, but still serves reads.
		EnumMemory
			mem(env.GetMemory());
		env.SetMemory(&mem);
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			TRY((*i)->Run(input, env));
			TRY(env.SkipDelimiters());
		}
		// Restore the old memory system.
		env.SetMemory(om);
		return OK;
	}
	
private:
	bool
		m_storeAlt;
	
	int
		m_memory;
};

