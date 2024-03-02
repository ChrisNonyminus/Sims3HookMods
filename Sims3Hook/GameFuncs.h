#pragma once
#include <Windows.h>

#include "Hooking/Hooks.h"

#include <string>
#include <stdexcept>

#include <map>
extern bool isSteam;
struct GameExeFunc {
    uintptr_t SteamVerAddr;
    uintptr_t OriginVerAddr1_69;

    uintptr_t GetAddr() const {
        if (isSteam) {
            return SteamVerAddr;
        }
        else {
            return OriginVerAddr1_69;
        }
    }

    template <typename TRet, typename...Args>
    TRet StdCall(Args... args) {
        //assert(GetAddr() != 0 && "Address not found in your game's exe yet");
        return Call__StdCall_Function<TRet>(GetAddr(), args...);
    }

    template <typename TRet, typename...Args>
    TRet CdeclCall(Args... args) {
        //assert(GetAddr() != 0 && "Address not found in your game's exe yet");
        return Call_Function<TRet>(GetAddr(), args...);
    }
};

extern std::map<std::string, GameExeFunc> gameExeFuncs;

void MonoAddInternalCall(const char* name, LPVOID funcPtr);
