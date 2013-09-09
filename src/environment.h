#pragma once

#include "memory.h"
#include "utils.h"
#include <map>

class Environment
{
public:
	// cons
		Environment(Memory * m) //, Delimiter * d, Options * o)
	:
		m_memory(m),
		m_options()
	{
	};
	
	error_t
		SkipDelimiters(char const * & input)
	{
		return Utils::SkipWhitespaceOK(input);
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
		Skip(size_t n) { return m_memory->Skip(n); };
	
	void
		SetOption(char const * name, int const value)
	{
		m_options[std::string(name)] = value;
	};
	
	int
		GetOption(char const * name)
	{
		return m_options[std::string(name)];
	};
	
	// Not used by this environment.  They should throw an error, but we control
	// when and where this one is used, so I can just never use it like that.
	virtual Memory *
		GetMemory()
	{
		return m_memory;
	};
	
	void
		SetMemory(Memory * mem)
	{
		m_memory = mem;
	};
	
private:
	Memory *
		m_memory;
	
	std::map<std::string, int>
		m_options;
};

// The "default" environment is the code used to read in default values from the
// format specifier itself and store the values in the classes themselves.
class DefaultEnvironment : public Environment
{
private:
	class DefaultMemory : public Memory
	{
	public:
		virtual error_t
			GetNextPointer(cell ** const ret)
		{
			FAIL(m_save, ERROR_INVALID_MEMORY);
			*ret = m_save;
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
			Skip(size_t n)
		{
			FAIL(false, ERROR_INVALID_MEMORY);
			return OK;
		};
		
	private:
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
	
public:
	static DefaultEnvironment &
		Get(cell * dest)
	{
		m_defaultEnvironment.m_storage.m_save = dest - 1;
		return m_defaultEnvironment;
	};
	
private:
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
		DefaultEnvironment(DefaultEnvironment const & that)
	:
		Environment(that)
	{
	};
	
	DefaultMemory
		m_storage;
};

