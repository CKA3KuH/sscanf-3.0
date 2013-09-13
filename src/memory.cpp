#include "memory.h"

error_t
	CellMemory::
	GetNextPointer(cell ** const ret)
{
	FAIL(m_cur < m_count, ERROR_OUT_OF_VARIABLES);
	// 1-indexed array.
	amx_GetAddr(m_amx, m_params[++m_cur], ret);
	return OK;
}

error_t
	CellMemory::
	GetNextValue(cell * const ret)
{
	FAIL(m_cur < m_count, ERROR_OUT_OF_VARIABLES);
	cell *
		addr;
	amx_GetAddr(m_amx, m_params[++m_cur], &addr);
	*ret = *addr;
	return OK;
}

error_t
	CellMemory::
	GetNextString(char * ret, size_t len)
{
	FAIL(m_cur < m_count, ERROR_OUT_OF_VARIABLES);
	// 1-indexed array.
	cell *
		addr;
	amx_GetAddr(m_amx, m_params[++m_cur], &addr);
	amx_GetString(ret, addr, false, len);
	return OK;
}

error_t
	CellMemory::
	SetNextValue(cell const val, size_t idx)
{
	// Only increment if we are writing the first value in an array.
	if (!idx) m_cur++;
	// Check that the current variable is in range.
	FAIL(m_cur <= m_count, ERROR_OUT_OF_VARIABLES);
	cell *
		addr;
	amx_GetAddr(m_amx, m_params[m_cur], &addr);
	addr[idx] = val;
	return OK;
}

error_t
	CellMemory::
	SetNextString(char const * val, size_t idx, bool pack)
{
	if (!idx) ++m_cur;
	FAIL(m_cur <= m_count, ERROR_OUT_OF_VARIABLES);
	// 1-indexed array.
	cell *
		addr;
	amx_GetAddr(m_amx, m_params[m_cur], &addr);
	amx_SetString(addr + idx, val, pack, false, strlen(val));
	return OK;
}

error_t
	CellMemory::
	Skip(int n, int part)
{
	m_cur = m_cur + n;
	FAIL(m_cur <= m_count, ERROR_OUT_OF_VARIABLES);
	return OK;
}

int
	CellMemory::
	Poll()
{
	return m_cur + 1;
}

