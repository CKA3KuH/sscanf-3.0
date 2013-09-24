#pragma once

#include <malloc.h>
#include "../sdk/plugin.h"
#include "errors.h"

class Memory
{
PUBLIC:
	// cons
		Memory(AMX * amx) : m_amx(amx) {};
	
	virtual error_t
		GetNextPointer(cell ** const ret) = 0;
	
	virtual error_t
		GetNextValue(cell * const ret) = 0;
	
	virtual error_t
		GetNextString(char * ret, size_t len) = 0;
	
	virtual error_t
		SetNextValue(cell const val, size_t idx = 0) = 0;
	
	virtual error_t
		SetNextString(char const * val, size_t idx = 0, bool pack = false) = 0;
	
	virtual error_t
		Skip(int n, int part = 0) = 0;
	
	virtual int
		Poll() = 0;
	
	virtual bool
		GetZeroLengthValid() const
	{
		// Only true in quiet memory.
		return false;
	};
	
PROTECTED:
	AMX *
		m_amx;
};

class CellMemory : public Memory
{
PUBLIC:
	// cons
		CellMemory(AMX * amx, cell * params)
	:
		Memory(amx),
		m_cur(2), // Skip "input", and "specifier".
		m_count(params[0] / 4),
		m_params(params)
	{
	};
	
	virtual error_t
		GetNextPointer(cell ** const ret);
	
	virtual error_t
		GetNextValue(cell * const ret);
	
	virtual error_t
		GetNextString(char * ret, size_t len);
	
	virtual error_t
		SetNextValue(cell const val, size_t idx = 0);
	
	virtual error_t
		SetNextString(char const * val, size_t idx = 0, bool pack = false);
	
	virtual error_t
		Skip(int n, int part = 0);
	
	virtual int
		Poll();
	
PRIVATE:
	size_t
		m_cur,
		m_count;
	
	cell *
		m_params;
};

/*
class EnumMemory : public Memory
{
PUBLIC:
	// cons
		EnumMemory(Memory * parent)
	:
		m_offset(0),
		m_parent(parent),
		m_amx(parent->m_amx)
	{
		// Signal the error later.
		if (m_parent->GetNextPointer(&m_param) != OK) m_cur = 0x7FFFFFFF;
	};
	
	// Make sure parents propogate ALL reads before writes.  Or rather some.
	error_t
		GetNextPointer(cell ** const ret)
	{
		return m_parent->GetNextPointer(ret);
	};
	
	error_t
		GetNextValue(cell * const ret)
	{
		return m_parent->GetNextValue(ret);
	};
	
	error_t
		GetNextString(char * const ret, size_t len)
	{
		return m_parent->GetNextString(ret, len);
	};
	
	error_t
		SetNextValue(cell const val, size_t idx = 0)
	{
		if (m_cur > m_count) return ERROR_NO_STORAGE;
		// Only increment if we are writing the first value in an array.
		if (!idx)
		{
			if (m_cur++ == m_count) return ERROR_NO_STORAGE;
		}
		cell *
			addr;
		amx_GetAddr(m_amx, m_params[m_cur], &addr);
		addr[idx] = val;
		return OK;
	};
	
	error_t
		SetNextString(char * const val, size_t idx = 0, bool pack = false)
	{
		if (m_cur > m_count) return ERROR_NO_STORAGE;
		if (!idx)
		{
			if (m_cur++ == m_count) return ERROR_NO_STORAGE;
		}
		// 1-indexed array.
		cell *
			addr;
		amx_GetAddr(m_amx, m_params[m_cur], &addr);
		amx_SetString(addr + idx, val, pack, false, strlen(val));
		return OK;
	};
	
PRIVATE:
	size_t
		offset;
	
	Memory *
		m_parent;
	
	cell *
		m_param;
};
*/


#ifdef SSCANF_DEBUG
	// This version doesn't store data, it just compares the writes to expected
	// values.
	class TestMemory : public Memory
	{
	PUBLIC:
		// cons
			TestMemory(cell * data, size_t len)
		:
			Memory(nullptr),
			m_len(len),
			m_pos(0),
			m_data(nullptr)
		{
			m_data = (cell *)malloc(len * sizeof (cell));
			memcpy(m_data, data, len * sizeof (cell));
		};
		
		// dest
			~TestMemory()
		{
			delete m_data;
		};
		
		virtual error_t
			GetNextPointer(cell ** const ret) { return OK; }
		
		virtual error_t
			GetNextValue(cell * const ret) { return OK; }
		
		virtual error_t
			GetNextString(char * ret, size_t len) { return OK; }
		
		virtual error_t
			SetNextValue(cell const val, size_t idx = 0)
		{
			if (m_pos >= m_len) return ERROR_UNKNOWN_ERROR;
			if (val != m_data[m_pos++]) return ERROR_UNKNOWN_ERROR;
			return OK;
		}
		
		virtual error_t
			SetNextString(char const * val, size_t idx = 0, bool pack = false) { return OK; }
		
		virtual error_t
			Skip(int n, int part = 0) { m_pos += n + part; return OK; }
		
		virtual int
			Poll() { return m_pos; };
		
		int
			m_len,
			m_pos;
		
		cell *
			m_data;
	};
	
	// This version does store data in a pre-provided memory location.
	class TempMemory : public Memory
	{
	PUBLIC:
		// cons
			TempMemory(cell * data, size_t len)
		:
			Memory(nullptr),
			m_len(len),
			m_pos(-1),
			m_data(data)
		{
		};
		
		// dest
			~TempMemory()
		{
		};
		
		virtual error_t
			GetNextPointer(cell ** const ret) { return OK; }
		
		virtual error_t
			GetNextValue(cell * const ret) { *ret = m_data[++m_pos]; return OK; }
		
		virtual error_t
			GetNextString(char * ret, size_t len) { return OK; }
		
		virtual error_t
			SetNextValue(cell const val, size_t idx = 0)
		{
			if (!idx) ++m_pos;
			if (m_pos + idx >= (size_t)m_len) return ERROR_UNKNOWN_ERROR;
			m_data[m_pos + idx] = val;
			return OK;
		}
		
		virtual error_t
			SetNextString(char const * val, size_t idx = 0, bool pack = false) { return OK; }
		
		virtual error_t
			Skip(int n, int part = 0) { m_pos += n + part; return OK; }
		
		virtual int
			Poll() { return m_pos; };
		
		int
			m_len,
			m_pos;
		
		cell *
			m_data;
	};
#endif

