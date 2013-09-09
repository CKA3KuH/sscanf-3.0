#pragma once

class EnumSpecifier : public SpecifierGroup
{
public:
	// cons
		EnumSpecifier()
	:
		m_default(nullptr),
		m_memory(-1),
		SpecifierGroup('e')
	{
	};
	
	// cons
		EnumSpecifier(EnumSpecifier const & that)
	:
		m_default(nullptr),
		m_memory(-1),
		SpecifierGroup(that)
	{
		Skip(env);
		if (that.m_default)
		{
			m_default = new cell [m_memory];
			memcpy(m_default, that.m_default, m_memory * sizeof (cell));
		}
	};
	
	CLONE();
	
	// Local.
	virtual error_t
		ReadToken(char const * & input)
	{
		// Is this a required, or optional, enum?
		if (*input++ != GetSpecifier()) SetOptional();
		NEXT('<', ERROR_NO_TYPE_START);
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
		if (GetOptional())
		{
			NEXT(input, '(', ERROR_NO_DEAFULT_START);
			int
				mem = Skip(Environment & env);
			m_default = new cell[mem];
			TRY(Run(input, gDefaultEnvironment(m_default)));
			// Skip the closing bracket.
			NEXT(input, ')', ERROR_NO_DEAFULT_END);
			// Whatever.
		}
		return OK;
	};
	
	virtual int
		Skip(Environment & env)
	{
		if (m_memory != -1) return m_memory;
		m_memory = 0;
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			m_memory += (*i)->Skip(Environment & env);
		}
		return m_memory;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		// Store a copy of the old memory system.
		Memory *
			om = env.GetMemory();
		// Just takes all writes and null-routes them, but still serves reads.
		EnumMemory
			mem(om);
		env.SetMemory(&mem);
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
		out = out << "e<";
		for (auto i = Begin(), e = End(); i != e; ++i)
		{
			(*i)->Render(out);
		}
		return out << ">";
	};
	
private:
	cell *
		m_default;
	
	bool
		m_storeAlt;
	
	int
		m_memory;
};

