#include "../specifiers.h"

class DelimSpecifier : public Specifier
{
PRIVATE:
	enum E_DELIM_TYPE
	{
		DELIM_TYPE_NONE,
		DELIM_TYPE_RESET, // "p<>", or "P<>"
		DELIM_TYPE_SET_SINGLE, // "p<,>"
		DELIM_TYPE_ADD_SINGLE, // "p+<,>"
		DELIM_TYPE_REM_SINGLE, // "p-<,>"
		DELIM_TYPE_SET_MULTI, // "P<,>"
		DELIM_TYPE_ADD_MULTI, // "P+<,>"
		DELIM_TYPE_REM_MULTI, // "P-<,>"
		DELIM_TYPE_LEGACY_REQ, // Old "p"
		DELIM_TYPE_LEGACY_OPT, // Old "P"
		_DELIM_TYPE_COUNT
	};
	
PUBLIC:
	// cons
		DelimSpecifier()
	:
		Specifier('p'),
		m_type(DELIM_TYPE_NONE),
		m_delim(nullptr)
	{
	};
	
	// cons
		DelimSpecifier(DelimSpecifier const & that)
	:
		Specifier(that),
		m_type(that.m_type),
		m_delim(that.m_delim)
	{
	};
	
	virtual // cons
		~DelimSpecifier()
	{
		delete [] m_delim;
	};
	
	CLONE();
	
	virtual error_t
		ReadToken(char const * & input)
	{
		// Determine what type of delimiter operation this is (set/add/remove).
		// We can't determine if we need legacy operations until later ("env").
		enum E_DELIM_TYPE
			t = (*input++ == GetSpecifier()) ? DELIM_TYPE_SET_SINGLE : DELIM_TYPE_SET_MULTI;
		Utils::SkipWhitespace(input);
		if (*input == '-')
			t = (t == DELIM_TYPE_SET_SINGLE) ? DELIM_TYPE_REM_SINGLE : DELIM_TYPE_REM_MULTI;
		else if (*input == '+')
			t = (t == DELIM_TYPE_SET_SINGLE) ? DELIM_TYPE_ADD_SINGLE : DELIM_TYPE_ADD_MULTI;
		else goto DelimSpecifier_get_params;
		// Get what the actual delimiters are.
		++input;
		Utils::SkipWhitespace(input);
DelimSpecifier_get_params:
		char const *
			opts;
		size_t
			len;
		TRY(Utils::GetParams(input, opts, &len));
		if (len)
		{
			m_delim = new char [len + 1];
			strcpy(m_delim, opts);
		}
		else
		{
			FAIL(t == DELIM_TYPE_SET_SINGLE || t == DELIM_TYPE_SET_MULTI, ERROR_INVALID_DELIMITER_TYPE);
			t = DELIM_TYPE_RESET;
		}
		m_type = t;
		return OK;
	};
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		// So that the environment doesn't try to skip two sets of delimiters.
		env.ZeroRead();
		enum E_DELIM_TYPE
			t = m_type;
		if (env.GetOption(OPTION_OLD_DELIMITERS))
		{
			if (t == DELIM_TYPE_SET_SINGLE) t = DELIM_TYPE_LEGACY_REQ;
			else if (t == DELIM_TYPE_SET_MULTI) t = DELIM_TYPE_LEGACY_OPT;
			else FAIL(false, ERROR_INVALID_DELIMITER_TYPE);
		}
		Delimiters &
			d = env.GetDelimiters();
		switch (t)
		{
			case DELIM_TYPE_RESET:
				d.Reset();
				break;
			case DELIM_TYPE_SET_SINGLE:
				d.Set(m_delim);
				break;
			case DELIM_TYPE_ADD_SINGLE:
				d.Add(m_delim);
				break;
			case DELIM_TYPE_REM_SINGLE:
				d.Remove(m_delim);
				break;
			case DELIM_TYPE_SET_MULTI:
				d.SetOptional(m_delim);
				break;
			case DELIM_TYPE_ADD_MULTI:
				d.AddOptional(m_delim);
				break;
			case DELIM_TYPE_REM_MULTI:
				d.RemoveOptional(m_delim);
				break;
			case DELIM_TYPE_LEGACY_REQ:
				// Can only add one character this way.
				d.Set(*m_delim);
				break;
			case DELIM_TYPE_LEGACY_OPT:
			{
				// Add multiple characters, but each as separate delimiters.
				char *
					delim = m_delim;
				d.Reset();
				while (*delim) d.Add(*delim++);
				break;
			}
		}
		return OK;
	};
	
PRIVATE:
	enum E_DELIM_TYPE
		m_type;
	
	char *
		m_delim;
};

