class ArraySpecifier : public Specifier
{
	virtual error_t
		ReadToken(char const * & input)
	{
		// Check this has the correct specifier.
		if (*input | 0x20 == 'a')
		{
			// Skip it - simple as!
			if (*input++ == 'A') SetOptional();
			// Get the type.
			NEXT(input, '<', ERROR_NO_TYPE);
			TRY(Parser::GetSpecifier(input, m_type));
			NEXT(input, '>', ERROR_UNCLOSED_TYPE);
			FAIL(!m_type->GetOptional(), ERROR_OPTIONAL_ARRAY_TYPE);
			// Skip the defaults for now (come back to them after the length).
			char *
				optional = 0;
			if (GetOptional())
			{
				TRY(Utils::GetDefaults(input, &optional));
			}
			// Now get the size.
			TRY(Utils::GetArraySize(input, &m_size));
			if (optional)
			{
				// Parse the options twice :(.  Once to get the number so we can
				// allocate enough memory, the second time to store the values.
				static char const *
					delimiters[] = {",", nullptr};
				cell
					prev,
					cur = 0;
				char *
					start = optional;
				// Got the number of elements.
				m_defaultCount = 0;
				for ( ; ; )
				{
					prev = cur;
					error_t
						error = m_type->Get(start, &cur, delimiters);
					++m_defaultCount;
					if (error != OK)
					{
						if (error != ERROR_NO_DELIMITER) return error;
						// End of the string.
						break;
					}
				}
				// Get the increment for missing values.
				if (m_defaultCount > 1) m_increment = cur - prev;
				else if (m_defaultCount == 1)
				{
					// Don't allocate memory in this case (just a waste).
					m_defaults = nullptr;
					m_increment = cur;
					return OK;
				}
				else return ERROR_NO_DEFAULTS;
				// Allocate the memory.
				m_defaults = new cell [m_defaultCount];
				// Store all the values.
				m_defaultCount = 0;
				start = optional;
				while (m_type->Get(start, &m_defaults[m_defaultCount++], delimiters) == OK) {}
				// We could add a check here to determine that the number of
				// defaults is not greater than the size, but that's a little
				// tricky in the case of multi-dimensional arrays (which I've
				// not written yet).
			}
		}
		else return ERROR_UNEXPECTED_SPECIFIER;
		return OK;
	};
	
private:
	cell
		m_increment;
	
	cell *
		m_defaults;
	
	size_t
		m_size,
		m_defaultCount;
	
	Specifier *
		m_type;
};

