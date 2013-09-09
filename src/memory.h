#pragma once

#include <malloc.h>
#include "../sdk/plugin.h"
#include "errors.h"

class Memory
{
public:
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
		Skip(size_t n) = 0;
	
protected:
	AMX *
		m_amx;
};

/*
class CellMemory : public Memory
{
public:
	// cons
		CellMemory(AMX * amx, cell * params)
	:
		m_amx(amx);
		m_cur(2), // Skip "input", and "specifier".
		m_count(params[0] / 4),
		m_params(params)
	{
	};
	
	error_t
		GetNextPointer(cell ** const ret)
	{
		if (m_cur >= m_count) return ERROR_NO_STORAGE;
		// 1-indexed array.
		amx_GetAddr(m_amx, m_params[++m_cur], ret);
		return OK;
	};
	
	error_t
		GetNextValue(cell * const ret)
	{
		if (m_cur >= m_count) return ERROR_NO_STORAGE;
		cell *
			addr;
		amx_GetAddr(m_amx, m_params[++m_cur], &addr);
		*ret = *addr;
		return OK;
	};
	
	error_t
		GetNextString(char * const ret, size_t len)
	{
		if (m_cur >= m_count) return ERROR_NO_STORAGE;
		// 1-indexed array.
		cell *
			addr;
		amx_GetAddr(m_amx, m_params[++m_cur], &addr);
		amx_GetString(ret, addr, false, len);
		return OK;
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
	
	error_t
		Skip(size_t n)
	{
		m_cur = m_cur + n;
		if (m_cur >= m_count) return ERROR_NO_STORAGE;
		return OK;
	};
	
private:
	size_t
		m_cur,
		m_count;
	
	cell *
		m_params;
};

class EnumMemory : public Memory
{
public:
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
	
private:
	size_t
		offset;
	
	Memory *
		m_parent;
	
	cell *
		m_param;
};
*/

