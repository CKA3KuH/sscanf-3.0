#pragma once

class NumericSpecifier : public Specifier
{
public:
	// cons
		NumericSpecifier()
	:
		Specifier('*'), // Actually has 10 valid specifiers.
		m_child(nullptr),
		m_count(-1)
	{
	};
	
	// cons
		NumericSpecifier(NumericSpecifier const & that)
	:
		Specifier(that),
		m_child(that.m_child),
		m_count(that.m_count)
	{
	};
	
	virtual error_t
		ReadToken(char const * & input)
	{
		if (*input == '*')
		{
			m_count = -1; // Determined later.
			++input;
		}
		else
		{
			// Can't be invalid because of which specifiers were used.
			Utils::ReadDecimal(input, m_count);
		}
		TRY(gParser.GetNext(input, &m_child));
		FAIL(m_child, ERROR_NO_CHILD);
		return OK;
	};
	
	CLONE();
	
	// This needs to count all READS, but not WRITES.
	virtual int
		GetMemoryUsage()
	{
		if (m_child)
		{
			if (m_count == -1) return 0;
			return m_child->GetMemoryUsage() * m_count;
		}
		return 0;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		int
			end = 0;
		if (m_count < 0)
		{
			cell
				read;
			TRY(env.GetNextValue(&read));
			end = read;
		}
		else
		{
			end = m_count;
		}
		// Run the same specifier over and over again.
		for (int i = 0; i != end; ++i)
		{
			TRY(m_child->Run(input, env));
			TRY(env.SkipDelimiters(input));
		}
		return OK;
	};
	
	virtual // dest
		~NumericSpecifier()
	{
		delete m_child;
	};
	
	virtual int
		CountChildren() const { return m_count; };
	
private:
	Specifier *
		m_child;
	
	cell
		m_count;
};

