// dllmain.cpp : Defines the entry point for the DLL application.
#define _CRT_SECURE_NO_WARNINGS
#include "GameFuncs.h"
#include <fstream>
#include <filesystem>
#include <sstream>
namespace fs = std::filesystem;

bool isSteam = false;


std::map<std::string, GameExeFunc> gameExeFuncs = {
    {"mono_add_internal_call", GameExeFunc{0x00E82550,0}}
};


/// MY OWN INTERNAL CALLS
bool FileReadAllText(const unsigned char* path, unsigned char* outText, size_t maxSize) {
    // HACK: the string pointers seem to be off by 16 bytes??? Thus, we offset the pointers accordingly. Could be dangerous.
    path += 16;
    outText += 16;
    if (!fs::exists((const char*)path)) {
        return false;
    }
    std::ifstream ifs((const char*)path);
    std::string out;
    ifs >> out;
    strncpy((char*)outText, out.c_str(), maxSize);
    return true;
}
/// MY OWN INTERNAL CALLS END

void AddBaseInternalCalls() {
    MonoAddInternalCall("IO.File::CReadAllText", FileReadAllText);
}

void HandleMonoSetup() {
    AddBaseInternalCalls();

    // Additionally, search for libraries and call their provided entry point 
    // that should add their own internal calls

    std::string path("./Sims3Hook/plugins/");
    if (!fs::exists(path)) {
        return;
    }
    std::string ext(".dll");
    for (auto& p : fs::recursive_directory_iterator(path)) {
        HMODULE lib = LoadLibraryA(p.path().string().c_str());
        if (!lib) continue;
        FARPROC libEntry = GetProcAddress(lib, "HookLibSetup");
        if (!libEntry) continue;
        typedef void (*HookLibSetupPtr)();
        HookLibSetupPtr HookLibSetup = (HookLibSetupPtr)libEntry;
        HookLibSetup();
    }
}

uintptr_t kMonoSetup_PatchAddr;
uintptr_t kMonoSetup_RetAddr;
uintptr_t kMonoSetup_UnknownInnerCallAddr;
__declspec(naked) void MonoSetupHook() {
    __asm {
        call[kMonoSetup_UnknownInnerCallAddr];
        call HandleMonoSetup
        jmp [kMonoSetup_RetAddr];
    }
}

void SetupHooks() {
    //MessageBox(NULL, "Attach debugger now", "DEBUG", MB_OK);

    char exePath[MAX_PATH];
    GetModuleFileNameA(GetModuleHandle(NULL), exePath, MAX_PATH);

    std::string gamePath = exePath;
    if (gamePath.find("steamapps") != std::string::npos) {
        isSteam = true;
        kMonoSetup_PatchAddr = 0x00D7E6EF;
        kMonoSetup_RetAddr = 0x00D7E6F4;
        kMonoSetup_UnknownInnerCallAddr = 0xD7DD10;
    }
    else {
        MessageBox(NULL, L"Hooking to origin version not yet implemented...", L"ERROR", MB_ICONERROR | MB_OK);
        throw std::runtime_error("Hooking to origin version not yet implemented...");
    }

    WriteJump((LPVOID)kMonoSetup_PatchAddr, &MonoSetupHook);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        SetupHooks();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

