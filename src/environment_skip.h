#pragma once

#include "memory.h"
#include "utils.h"
#include "options.h"
#include "delimiters.h"
#include <map>
#include <string>

class Environment
{
PUBLIC:
	// cons
		Environment(Memory * m) //, Delimiter * d, Options * o)
	:
		m_skipDelim(true),
		m_options(),
		m_memory(m),
		m_delims(),
		m_writing(true)
	{
	};
	
	// cons
		Environment(Environment & env)
	:
		m_skipDelim(env.m_skipDelim),
		m_options(&env.m_options),
		m_memory(env.m_memory),
		m_delims(env.m_delims),
		m_writing(env.m_writing)
	{
	};
	
	virtual error_t
		SkipDelimiters(char const * & input)
	{
		if (!m_skipDelim)
		{
			m_skipDelim = true;
			return OK;
		}
		FAIL(m_delims.SkipDelimiters(input), ERROR_INVALID_DELIMITER);
		return OK;
	};
	
	virtual bool
		AtDelimiter(char const * & input, bool incWhite) const
	{
		return m_delims.AtDelimiter(input, incWhite);
	};
	
	// Mostly just simple pass-throughs to the memory subsystem.
	error_t
		GetNextPointer(cell ** const ret) { return m_memory->GetNextPointer(ret); };
	
	error_t
		GetNextValue(cell * const ret) { return m_memory->GetNextValue(ret); };
	
	error_t
		GetNextString(char * ret, size_t len) { return m_memory->GetNextString(ret, len); };
	
	error_t
		SetNextValue(cell const val, size_t idx = 0) { return m_memory->SetNextValue(val, idx); };
	
	error_t
		SetNextString(char const * val, size_t idx = 0, bool pack = false) { return m_memory->SetNextString(val, idx, pack); };
	
	error_t
		Skip(int n, int part = 0, bool read = false)
	{
		if (read || m_writing) return m_memory->Skip(n, part);
		return OK;
	};
	
	virtual int
		Poll() { return m_memory->Poll(); };
	
	virtual void
		ZeroRead() { m_skipDelim = false; ;
	
	void
		SetOption(option_t option, int const value) { m_options.Set(option, value); };
	
	int
		GetOption(option_t option) { return m_options.Get(option); };
	
	Delimiters &
		GetDelimiters() { return m_delims; };
	
	// Not used by this environment.  They should throw an error, but we control
	// when and where this one is used, so I can just never use it like that.
	virtual Memory *
		GetMemory() { return m_memory; };
	
	void
		SetMemory(Memory * mem) { m_memory = mem; };
	
	void
		SetQuiet(bool quiet) { m_writing = !quiet; };
	
	bool
		GetQuiet() { return m_writing; };
	
PRIVATE:
	Options
		m_options;
	
	bool
		m_skipDelim,
		m_writing;
	
	Memory *
		m_memory;
	
	Delimiters
		m_delims;
};

// The "default" environment is the code used to read in default values from the
// format specifier itself and store the values in the classes themselves.
class DefaultEnvironment : public Environment
{
PRIVATE:
	class DefaultMemory : public Memory
	{
	PUBLIC:
		virtual error_t
			GetNextPointer(cell ** const ret)
		{
			FAIL(m_save, ERROR_INVALID_MEMORY);
			*ret = ++m_save;
			return OK;
		};
		
		virtual error_t
			GetNextValue(cell * const ret)
		{
			FAIL(false, ERROR_INVALID_MEMORY);
			return OK;
		};
		
		virtual error_t
			GetNextString(char * ret, size_t len)
		{
			FAIL(false, ERROR_INVALID_MEMORY);
			return OK;
		};
		
		virtual error_t
			SetNextValue(cell const val, size_t idx = 0)
		{
			if (!idx) ++m_save;
			//printf("Write to: 0x%08x\n", (int)m_save);
			// No increments in here.
			m_save[idx] = val;
			return OK;
		};
		
		virtual error_t
			SetNextString(char const * val, size_t idx = 0, bool pack = false)
		{
			if (!idx) ++m_save;
			if (pack)
			{
				// Quite unsafe.
				char *
					dest = (char *)(m_save + idx);
				strcpy(dest, val);
			}
			else
			{
				cell *
					dest = m_save + idx;
				while ((*dest = *val))
				{
					++dest;
					++val;
				}
			}
			return OK;
		};
		
		virtual error_t
			Skip(int n, int part = 0)
		{
			//FAIL(false, ERROR_INVALID_MEMORY);
			return OK;
		};
		
		virtual int
			Poll()
		{
			//FAIL(false, ERROR_INVALID_MEMORY);
			return OK;
		};
		
	PRIVATE:
		static inline cell
			FlipCell(cell c)
		{
			c = (c << 16) | (c >> 16);
			c = ((c & 0x00FF00FF) << 8) | ((c & 0xFF00FF00) >> 8);
			return c;
		};
		
		// cons
			DefaultMemory()
		:
			m_save(nullptr),
			Memory(nullptr)
		{
		};
		
		cell *
			m_save;
		
		friend class DefaultEnvironment;
	};
	
PUBLIC:
	static DefaultEnvironment &
		Get(cell * dest)
	{
		m_defaultEnvironment.m_storage.m_save = dest - 1;
		return m_defaultEnvironment;
	};
	
PRIVATE:
	static DefaultEnvironment
		m_defaultEnvironment;
	
	// cons
		DefaultEnvironment()
	:
		m_storage(),
		Environment(&m_storage)
	{
	};
	
	// cons
		DefaultEnvironment(DefaultEnvironment & that)
	:
		Environment(that)
	{
	};
	
	DefaultMemory
		m_storage;
};

