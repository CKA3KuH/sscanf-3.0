#pragma once

class NumericSpecifier : public Specifier
{
PUBLIC:
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
	virtual cell
		Skip(Environment & env)
	{
		if (m_child)
		{
			if (m_count == -1)
			{
				cell
					c,
					s;
				// Get the count.
				env.GetNextValue(&c);
				FAIL(c >= 0, ERROR_COUNT_IS_NEGATIVE);
				// Get the skip value for one.
				s = m_child->Skip(env);
				// Skip the remainder.
				env.Skip(s * (c - 1));
				return -1;
			}
			else
			{
				env.Skip(m_count);
				return m_count;
			}
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
			FAIL(read >= 0, ERROR_COUNT_IS_NEGATIVE);
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
		env.ZeroRead();
		return OK;
	};
	
	virtual // dest
		~NumericSpecifier()
	{
		delete m_child;
	};
	
	virtual int
		CountChildren() const { return m_count; };
	
PRIVATE:
	Specifier *
		m_child;
	
	cell
		m_count;
};

