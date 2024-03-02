#pragma once
#include <Windows.h>
#include <stdint.h>

bool WriteMemory(LPVOID dst, LPVOID src, size_t len, LPVOID oldData = NULL);

template<typename T> __forceinline T& Make_Global(const uintptr_t address)
{
    return *reinterpret_cast<T*>(address);
}

template<typename T> __forceinline T* Make_Pointer(const uintptr_t address)
{
    return reinterpret_cast<T*>(address);
}

template<typename T, typename... Types> __forceinline T Call_Function(const uintptr_t address, Types... args)
{
    return reinterpret_cast<T(__cdecl*)(Types...)>(address)(args...);

}


template<typename T, typename... Types> __forceinline T Call__StdCall_Function(const uintptr_t address, Types... args)
{
    return reinterpret_cast<T(__stdcall*)(Types...)>(address)(args...);

}
template<typename T, typename... Types> __forceinline T Call__Fastcall_Function(const uintptr_t address, Types... args)
{
    return reinterpret_cast<T(__fastcall*)(Types...)>(address)(args...);

}

template<typename T, typename X, typename... Types>
__forceinline T Call_Method(const uintptr_t address, X* const self, Types... args)
{
    return reinterpret_cast<T(__thiscall*)(X*, Types...)>(address)(self, args...);

}

#pragma pack(push, 1)
struct x86_jump
{
    uint8_t cmd = 0xE9;
    uintptr_t addr;
};
#pragma pack(pop)

inline bool WriteJump(LPVOID dst, LPVOID target) {
    x86_jump j;
    j.addr = (uint32_t)target - (uint32_t)dst - 1 - 4;
    return WriteMemory(dst, &j, sizeof(x86_jump));
}
