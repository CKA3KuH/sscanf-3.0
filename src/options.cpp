// cons
	Option::
	Option(char const * name, T const value)
:
	m_name(nullptr),
	m_value(value)
{
	size_t
		len = strlen(name);
	if (len)
	{
		m_name = new char [len];
		strcpy(m_name, name);
	}
};

