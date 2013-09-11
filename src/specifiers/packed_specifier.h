#include "string_specifier.h"
#include "../memory.h"

class PackedSpecifier : StringSpecifier
{
PRIVATE:
	class PackedMemory : public Memory
	{
		PUBLIC:
			// cons
				PackedMemory(Memory * m)
			:
				m_pidx(0),
				m_write(0),
				m_buffer(0),
				m_parent(m)
			{
			};
			
			virtual error_t
				GetNextPointer(cell ** const ret) { return m_parent->GetNextPointer(ret); };
			
			virtual error_t
				GetNextValue(cell * const ret) { return m_parent->GetNextValue(ret); };
			
			virtual error_t
				GetNextString(char * ret, size_t len) { return m_parent->GetNextString(ret, len); };
			
			virtual error_t
				SetNextValue(cell const val, size_t idx = 0)
			{
				if (idx == 0 || idx / 4 != m_pidx / 4)
				{
					// Writing to a different real slot.
					TRY(m_parent->SetNextValue(m_buffer, m_pidx / 4));
					m_buffer = 0;
				}
				if (idx)
				{
					
				}
				else
				{
					// Flush.
				}
				m_pidx = idx;
				return m_parent->SetNextValue(val, idx);
			};
			
			virtual error_t
				SetNextString(char const * val, size_t idx = 0, bool pack = false) { return m_parent->SetNextString(val, idx, pack); };
			
			virtual error_t
				Skip(int n, int part = 0) { return m_parent->Skip(n, part); };
			
			virtual int
				Poll() { return m_parent->Poll(); };
			
			virtual bool
				GetZeroLengthValid() const  { return m_parent->GetZeroLengthValid(); };
			
		PRIVATE:
			size_t
				m_pidx,
				m_write;
			
			cell
				m_buffer;
			
			Memory *
				m_parent;
	};
	
PUBLIC:
	// cons
		PackedSpecifier()
	:
		StringSpecifier(),
		m_ssize(0),
		m_length(0),
		m_default(nullptr),
		m_last(false)
	{
		m_specifier = 'z';
	};
	
	// cons
		PackedSpecifier(StringSpecifier const & that)
	:
		StringSpecifier(that),
		m_ssize(that.m_ssize),
		m_length(that.m_length),
		m_default(that.m_default),
		m_last(that.m_last)
	{
	};
	
	virtual // dest
		~PackedSpecifier()
	{
	};
	
	CLONE();
	
	virtual error_t
		Run(char const * & input, Environment & env)
	{
		// Variation on the memory.
		Memory *
			oldMem = env.GetMemory();
		PackedMemory
			mem(oldMem);
		env.SetMemory(&mem);
		TRY(dynamic_cast<StringSpecifier *>(this)->Run(input, env));
		// Flush pending writes.
		//mem.Flush();
		env.SetMemory(oldMem);
		return OK;
	};
};

