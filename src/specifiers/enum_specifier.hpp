#pragma once

class EnumSpecifier : public SpecifierGroup
{
	error_t
		Run(char const * & input, Memory * memory, Delimiters * delimiters)
	{
		EnumMemory
			mem(memory->GetNextPointer());
		for (var i = m_children.begin(), e = m_children.end(); i != e; ++i)
		{
			TRY((*i)->Run(input, mem, delimiters));
		}
		return OK;
	}
};

