//sscanf.cpp

#include "sscanf.h"

#include "src/parser.h"
#include "src/memory.h"
#include "src/specifiers.h"
#include "src/errors.h"

#include <malloc.h>
#include "sdk/plugin.h"

logprintf_t
	logprintf,
	real_logprintf;

extern void *
	pAMXFunctions;

#define IS_NULL(addr) ((addr)[0] == '\0' || ((addr)[0] == '\1' && (addr)[1] == '\0'))

// Based on amx_StrParam but using 0 length strings.  This can't be inline as
// it uses alloca - it could be written to use malloc instead, but that would
// require memory free code all over the place!
#define STR_PARAM(amx,param,result)                                             \
	do {                                                                        \
		cell * amx_cstr_; int amx_length_;                                      \
		amx_GetAddr((amx), (param), &amx_cstr_);                                \
		if (IS_NULL(amx_cstr_)) (result) = "";                                  \
		else {                                                                  \
			amx_StrLen(amx_cstr_, &amx_length_);                                \
			if (((result) = (char *)alloca((amx_length_ + 1) * sizeof (*(result)))))         \
				amx_GetString((result), amx_cstr_, sizeof (*(result)) > 1, amx_length_ + 1); \
			else FAIL(false, ERROR_MEMORY_ALLOCATION_FAIL); } }                 \
	while (false)

static cell AMX_NATIVE_CALL
	n_unformat(AMX * amx, cell * params)
{
//	FAIL(g_iTrueMax != 0, ERROR_NOT_INITIALISED);
	FAIL(params[0] >= 8, ERROR_MISSING_PARAMETERS);
	CellMemory
		storage(amx, params);
	// Get the specifier string.
	cell *
		formatAddr;
	amx_GetAddr(amx, params[2], &formatAddr);
	Specifier *
		parent = nullptr;
	error_t
		error = OK;
	char const *
		cptr = nullptr;
	bool
		del = true;
	if (*formatAddr == -1)
	{
		// Special case, the passed string is actually a 2 element arrays, the
		// first is -1 as a marker, the second is the address of a pre-compiled
		// specifier.
		parent = (Specifier *)*(formatAddr + 1);
		del = false;
	}
	else
	{
		FAIL(*formatAddr != '\0' && !(*formatAddr == '\1' && *(formatAddr + 1) == '\0'), ERROR_NO_SPECIFIER);
		char *
			format;
		amx_StrParam(amx, params[2], format);
		cptr = format;
		// Try complie the format line input to a specifier.
		error = gParser.Compile(cptr, &parent);
		if (error != OK)
		{
			delete parent;
			return (cell)error;
		}
		FAIL(parent, ERROR_NO_COMPILE);
	}
	// Get the string to split up.
	char *
		input;
	//logprintf("GET ");
	STR_PARAM(amx, params[1], input);
	cptr = input;
	//logprintf("input = \"%s\"", input);
	// Do the main code with the default delimiters to begin with.  This is the
	// only line in this function not concerned with marshalling data from PAWN
	// and in to C++, i.e. this is the main core of the operation now that we
	// have everything set up.  This is a VASTLY better design than v2.x, where
	// almost everything was controlled by the main "n_sscanf" function (making
	// it HUGE).
	Environment
		env(&storage);
	Utils::SkipWhitespace(cptr);
	error = parent->Run(cptr, env);
	if (del) delete parent; // Don't delete pre-compiled specifiers.
	return (cell)error;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL
	Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL
	Load(void ** ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	real_logprintf = logprintf;
	//GetServer = (GetServer_t)ppData[0xE1];
	
	//logprintf("0x%08X\n", (int)logprintf);
	logprintf("\n");
	logprintf(" ===============================\n");
	logprintf("     unformat plugin loaded.    \n");
	logprintf("         Version:  3.0.0        \n");
	logprintf("   (c) 2013 Alex \"Y_Less\" Cole  \n");
	logprintf(" ===============================\n");
	return true;
}

//----------------------------------------------------------
// The Unload() function is called when the server shuts down,
// meaning this plugin gets shut down with it.

PLUGIN_EXPORT void PLUGIN_CALL
	Unload()
{
	logprintf("\n");
	logprintf(" ===============================\n");
	logprintf("    unformat plugin unloaded.   \n");
	logprintf(" ===============================\n");
}

AMX_NATIVE_INFO
	sscanfNatives[] =
		{
			{"unformat", n_unformat},
			//{"UNFORMAT_Init",    n_UNFORMAT_Init},
			//{"UNFORMAT_Join",    n_UNFORMAT_Join},
			//{"UNFORMAT_Leave",   n_UNFORMAT_Leave},
			//{"UNFORMAT_Option",  n_UNFORMAT_Option},
			//{"UNFORMAT_Compile", n_UNFORMAT_Compile},
			{0,        0}
		};

// From "amx.c", part of the PAWN language runtime:
// http://code.google.com/p/pawnscript/source/browse/trunk/amx/amx.c

#define USENAMETABLE(hdr) \
	((hdr)->defsize==sizeof(AMX_FUNCSTUBNT))

#define NUMENTRIES(hdr,field,nextfield) \
	(unsigned)(((hdr)->nextfield - (hdr)->field) / (hdr)->defsize)

#define GETENTRY(hdr,table,index) \
	(AMX_FUNCSTUB *)((unsigned char*)(hdr) + (unsigned)(hdr)->table + (unsigned)index*(hdr)->defsize)

#define GETENTRYNAME(hdr,entry) \
	(USENAMETABLE(hdr) ? \
		(char *)((unsigned char*)(hdr) + (unsigned)((AMX_FUNCSTUBNT*)(entry))->nameofs) : \
		((AMX_FUNCSTUB*)(entry))->name)

PLUGIN_EXPORT int PLUGIN_CALL
	AmxLoad(AMX * amx) 
{
	/*int
		num,
		idx;
	// Operate on the raw AMX file, don't use the amx_ functions to avoid issues
	// with the fact that we've not actually finished initialisation yet.  Based
	// VERY heavilly on code from "amx.c" in the PAWN runtime library.
	AMX_HEADER *
		hdr = (AMX_HEADER *)amx->base;
	AMX_FUNCSTUB *
		func;
	num = NUMENTRIES(hdr, natives, libraries);
	for (idx = 0; idx != num; ++idx)
	{
		func = GETENTRY(hdr, natives, idx);
		if (!strcmp("SetPlayerName", GETENTRYNAME(hdr, func)))
		{
			// Intercept the call!
			SetPlayerName = (AMX_NATIVE)func->address;
			func->address = (ucell)n_SSCANF_SetPlayerName;
			break;
		}
	}*/
	return amx_Register(amx, sscanfNatives, -1);
}

//----------------------------------------------------------
// When a gamemode is over or a filterscript gets unloaded, this
// function gets called. No special actions needed in here.

PLUGIN_EXPORT int PLUGIN_CALL
	AmxUnload(AMX * amx) 
{
	return AMX_ERR_NONE;
}

