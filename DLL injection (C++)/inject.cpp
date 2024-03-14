#include <iostream>
#include <Windows.h>
#include <iomanip>

class Injector {
public:
    Injector() = default;

    void SetDLLPath(LPCWSTR path, SIZE_T size) {
        DLLPath = path;
        PathSize = size;
    }

    bool Inject(DWORD pid) {
        if (nullptr == DLLPath || 0 == PathSize) {
            std::cerr << "target DLL has not been set." << std::endl;
            return false;
        }

        std::cout << "attempting to inject " << DLLPath << " to the remote process." << std::endl;

        HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);
        if (nullptr == hProcess) {
            SillyFormat("OpenProcess", GetLastError());
            return false;
        }

        std::cout << "[" << hProcess << "] Obtained a handle on the process !!" << std::endl;

        HMODULE hKernel32 = GetModuleHandleW(L"Kernel32.dll");
        if (nullptr == hKernel32) {
            SillyFormat("GetModuleHandleW", GetLastError());
            CloseHandle(hProcess);
            return false;
        }

        std::cout << "[" << hKernel32 << "] handle to Kernel32 obtained!!" << std::endl;

        PVOID p_LoadLibrary = GetProcAddress(hKernel32, "LoadLibraryW");
        if (nullptr == p_LoadLibrary) {
            SillyFormat("GetProcAddress", GetLastError());
            CloseHandle(hProcess);
            return false;
        }

        std::cout << "[" << p_LoadLibrary << "] Address of LoadLibraryW has been obtained :3" << std::endl;

        PVOID rBuffer = VirtualAllocEx(hProcess, nullptr, PathSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (nullptr == rBuffer) {
            SillyFormat("VirtualAllocEx", GetLastError());
            CloseHandle(hProcess);
            return false;
        }

        std::cout << "[" << rBuffer << "] [RW-] Allocated " << PathSize << " bytes buffer to the target process." << std::endl;

        SIZE_T BytesWritten = 0;
        if (!WriteProcessMemory(hProcess, rBuffer, DLLPath, PathSize, &BytesWritten)) {
            SillyFormat("WriteProcessMemory", GetLastError());
            CloseHandle(hProcess);
            VirtualFreeEx(hProcess, rBuffer, 0, MEM_RELEASE);
            return false;
        }

        std::cout << "[" << rBuffer << "] [RW-] Wrote " << BytesWritten << " bytes to the allocated buffer." << std::endl;

        HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(p_LoadLibrary), rBuffer, 0, nullptr);
        if (nullptr == hThread) {
            SillyFormat("CreateRemoteThread", GetLastError());
            CloseHandle(hProcess);
            VirtualFreeEx(hProcess, rBuffer, 0, MEM_RELEASE);
            return false;
        }

        std::cout << "[" << hThread << "] Thread has been created." << std::endl;

        WaitForSingleObject(hThread, INFINITE);
        std::cout << "[" << hThread << "] Thread has finished execution." << std::endl;

        CloseHandle(hThread);
        CloseHandle(hProcess);
        VirtualFreeEx(hProcess, rBuffer, 0, MEM_RELEASE);

        std::cout << "Cleaned up." << std::endl;

        return true;
    }

private:
    LPCWSTR DLLPath = nullptr;
    SIZE_T PathSize = 0;

    void SillyFormat(const LPCSTR FunctionName, const DWORD Error) const {
        if (nullptr == FunctionName || 0 == Error) {
            std::cerr << "you fucked up" << std::endl;
        }
        std::cerr << "[" << FunctionName << "] failed, error: 0x" << std::hex << std::setw(8) << std::setfill('0') << Error << std::endl;
    }
};

int main() {
    Injector injector;
    DWORD pid;
    LPCWSTR dllPath;
    SIZE_T pathSize;

    injector.SetDLLPath(dllPath, pathSize);

    if (injector.Inject(pid)) {
        std::cout << "DLL Injection successful!" << std::endl;
    } else {
        std::cerr << "DLL Injection failed!" << std::endl;
    }

    return 0;
}
