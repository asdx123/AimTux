#pragma once
#include <cstring>

struct interface_t
{
	typedef void* (*InstantiateInterfaceFn)();
	InstantiateInterfaceFn m_CreateFn;
	const char *m_pName;
	interface_t *m_pNext;
};

inline void**& getvtable(void* inst, size_t offset = 0)
{
	return *reinterpret_cast<void***>((size_t)inst + offset);
}

inline const void** getvtable(const void* inst, size_t offset = 0)
{
	return *reinterpret_cast<const void***>((size_t)inst + offset);
}

template<typename Fn>
inline Fn getvfunc(const void* inst, size_t index, size_t offset = 0)
{
	return reinterpret_cast<Fn>(getvtable(inst, offset)[index]);
}

template <typename interface>
interface* GetInterface(const char* filename, const char* version, bool exact = false)
{
	void* library = dlopen(filename, RTLD_NOLOAD | RTLD_NOW);

	if (!library)
		return nullptr;

	void* interfaces_sym = dlsym(library, "s_pInterfaceRegs");

	if (!interfaces_sym)
		return nullptr;

	interface_t* interfaces = *reinterpret_cast<interface_t**>(interfaces_sym);

	interface_t* cur_interface;

	for (cur_interface = interfaces; cur_interface; cur_interface = cur_interface->m_pNext)
	{
		if (exact)
		{
			if (strcmp(cur_interface->m_pName, version) == 0)
				return reinterpret_cast<interface*>(cur_interface->m_CreateFn());
		}
		else
		{
			if (strstr(cur_interface->m_pName, version) && strlen(cur_interface->m_pName) - 3 == strlen(version))
				return reinterpret_cast<interface*>(cur_interface->m_CreateFn());
		}
	}

	return nullptr;
}

inline uintptr_t GetAbsoluteAddress(uintptr_t instruction_ptr, int offset, int size)
{
	return instruction_ptr + *reinterpret_cast<uint32_t*>(instruction_ptr + offset) + size;
};