#ifndef ENGINE_PROC
#define ENGINE_PROC

#include <Windows.h>
#include <intrin.h>
#include <TlHelp32.h>
#include <vector>
#include <array>

DWORD GetProcId(const wchar_t* proc_name);
uintptr_t SearchInProcessMemory(HANDLE h_process, const uint8_t* pattern, const char* mask);

#endif
