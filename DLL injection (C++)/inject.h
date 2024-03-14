#pragma once
#include <windows.h>
#include <iostream>

constexpr size_t MAX_PATH_LENGTH = 260;
constexpr wchar_t DLL_PATH[] = L"dll\\nigga.dll";

#define LOG_OK(MSG, ...) std::cout << "[+] " << MSG << "\n"
#define LOG_INFO(MSG, ...) std::cout << "[*] " << MSG << "\n"
#define LOG_WARNING(MSG, ...) std::cerr << "[-] " << MSG << "\n"

VOID PrintError(_In_ LPCSTR functionName, _In_ DWORD error);

BOOL InjectDLL(_In_ DWORD processId, _In_ LPCWSTR dllPath, _In_ SIZE_T pathSize);
