class Players
{
PUBLIC:
	error_t
		Inc(int id, bool isbot, char * name)
	{
		// If the player already exists, increment the count.  Else check the
		// name.
	};
	
	error_t
		Dec(int id)
	{
		// Decrement the count.
	};
	
	int
		Find(char * name, bool partial)
	{
		return m_fail;
	};
	
PRIVATE:
	int
		m_fail, // INVALID_PlAYER_ID
		m_max;  // MAX_PLAYERS
	
	bool *
		m_bot;
	
	bool *
		m_connected;
	
	char **
		m_names;
};

