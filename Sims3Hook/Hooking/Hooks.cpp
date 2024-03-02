#include <Windows.h>
#include "Hooks.h"

// based on https://github.com/itsmattkc/LEGOIslandRebuilder/blob/master/lib/util.cpp
bool WriteMemory(LPVOID dst, LPVOID src, size_t len, LPVOID oldData)
{
    DWORD oldProtec;

    if (VirtualProtect(dst, len, PAGE_EXECUTE_READWRITE, &oldProtec)) {
        // Read data out if requested
        if (oldData) {
            memcpy(oldData, dst, len);
        }

        // Write data if provided
        if (src) {
            memcpy(dst, src, len);
        }

        // Restore original protection
        VirtualProtect(dst, len, oldProtec, &oldProtec);

        return TRUE;
    }
    else {
        return FALSE;
    }
}
