//sscanf.cpp

static cell AMX_NATIVE_CALL
	n_sscanf(AMX * amx, cell * params)
{
	FAIL(g_iTrueMax != 0, ERROR_NOT_INITIALISED);
	FAIL(params[0] >= 8, ERROR_MISSING_PARAMETERS);
	CellMemory
		storage(amx, params);
	// Get the specifier string.
	cell *
		formatAddr;
	amx_GetAddr(amx, params[2], &formatAddr);
	Specifier *
		parent;
	if (*formatAddr == -1)
	{
		// Special case, the passed string is actually a 2 element arrays, the
		// first is -1 as a marker, the second is the address of a pre-compiled
		// specifier.
		parent = (Specifier *)*(formatAddr + 1);
	}
	else
	{
		FAIL(*formatAddr != '\0' && !(*formatAddr == '\1' && *(formatAddr + 1) == '\0'), ERROR_NO_SPECIFIER);
		char *
			format;
		amx_StrParam(amx, params[2], format);
		// Try complie the format line input to a specifier.
		TRY(Parser::Compile(format, &parent));
	}
	FAIL(parent, ERROR_NO_COMPILE);
	// Get the string to split up.
	char *
		input;
	amx_StrParam(amx, params[1], input);
	// Check for CallRemoteFunction style null strings and correct.
	if (!input) input = "";
	else if (input[0] == '\1' && input[1] == '\0') input[0] = '\0';
	// Do the main code with the default delimiters to begin with.  This is the
	// only line in this function not concerned with marshalling data from PAWN
	// and in to C++, i.e. this is the main core of the operation now that we
	// have everything set up.  This is a VASTLY better design than v2.x, where
	// almost everything was controlled by the main "n_sscanf" function (making
	// it HUGE).
	return parent->Run(input, &storage, &gDefaultDelimiters);
}

