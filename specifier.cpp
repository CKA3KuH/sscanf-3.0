// typedef
	// char const * &
	// parse_string

class Specifier
{
public:
	// cons
		Specifier()
	:
		m_parent(nullptr),
		m_optional(false)
	{
	};
	
	virtual error_t
		ReadValue(char const * & input, cell & result) = 0;
	
	virtual error_t
		ReadValueEx(char const * & input, char * result, size_t length) = 0;
	
	virtual error_t
		ReadToken(char const * & input) = 0;
	
	Specifier *
		Clone();
	
	void
		SetOptional() { m_optional = true; };
protected:
private:
	char
		m_specifier;
	
	class Specifier *
		m_parent;
	
	bool
		m_optional;
};

