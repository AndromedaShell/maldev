#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            MessageBoxW(NULL, L"HALO EVERYNYAN", L"sata andagi ^_^", MB_ICONEXCLAMATION);
            break;
    }
    return TRUE;
}
