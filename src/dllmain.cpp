#include "pch.h"
#include <windows.h>
#include <cstdint>
#include <fstream>
#include <string>

/*
===========================================================       INTRO       ===========================================================

I'm not gonna try to act like I wrote this all myself, I had some help from Copilot with this.

I tried to make up for it by doing research to know how all of this works afterwards, 
adding my own comments trying to make reading this code easier,
and adding my own functions that can help streamline debugging and other stuff I guess? Idk

hope it helps

oh also, while I didn't get this to work myself, if you can find some way to hook the dll, this address stores the difficulty numbers
in mselect:
bm2dx.dll + 0x967676

*/

/*
===========================================================COMPATIBLE VERSIONS===========================================================


------------------------------------------------------
LDJ:2025091710-010

bm2dx.dll / LDJ-68c117b0_aeecec

tested and proven to work with the addresses provided
------------------------------------------------------


*/


/// <summary>
/// I don't personally ever use this since it's kinda intrusive but it's there if you need it for debugging
/// </summary>
/// <param name="title">title of the message box</param>
/// <param name="message">message to contain within the box</param>
void GenerateDebugMessageBox(std::string title, std::string message) {
    //I probably should have made the requirement be LPCSTR but oh well I'm lazy
    MessageBoxA(0, message.c_str(), title.c_str(), 0);
}

/// <summary>
/// I'm lazy so I made an easy debug function to print out a message to the console
/// </summary>
/// <param name="message">message to write to console</param>
void PrintDebugConsoleMessage(std::string message) {
    OutputDebugStringA(message.c_str());
}




/// <summary>
/// Gets the base memory address of the provided DLL. Used oftentimes in this script to find said location of bm2dx.dll
/// </summary>
/// <param name="dllName">filename of dll</param>
/// <returns>memory address of dll if found</returns>
uintptr_t GetDllBaseAddress(const wchar_t* dllName)
{
    //gets the address of the module (duh)
    HMODULE hModule = GetModuleHandleW(dllName);
    //if the module isn't a null pointer, return the hmodule variable as a uintptr_t
    if (hModule != nullptr)
        return reinterpret_cast<uintptr_t>(hModule);
    return 0;
}


/// <summary>
/// primary loop of the DLL, the source version as of now just reads the ticker data in LDJ mode (before the scrolling effect)
/// </summary>
/// <param name=""></param>
/// <returns></returns>
DWORD WINAPI ReaderThread(LPVOID)
{

    // Give the game time to fully load, decrease this value if you need the dll to load faster
    Sleep(20000);

    //get address of bm2dx.dll
    uintptr_t dllBase = GetDllBaseAddress(L"bm2dx.dll");

    //if the dll's address is non-existent, close the loop
    if (dllBase == 0)
    {
        PrintDebugConsoleMessage("DLL Not found.\n");
        return 0;
    }

    //address of the ticker message (for LDJ:2025091710-010, I will add version detection later on)
    uintptr_t tickerAddr = dllBase + 0x0B30143C;


    while (true)
    {

        // 60 bytes + null terminator
        char buffer[61] = { 0 };

        //copy the ticker data from memory
        memcpy(buffer, (void*)tickerAddr, 60);

        //create the variable "ticker" from the buffer of 60 bytes (and one null terminator)
        std::string ticker(buffer);

        //this variable (as of now) just displays the latest ticker data it has obtained
        std::ofstream log("ldj-tickerget.log");

        //print latest ticker data to log        
        log << ticker << std::endl;;


        //wait
        Sleep(30);
    }

    return 0;
}



/// <summary>
/// starting point of the dll
/// [WARNING!!!!!!!] I learned this the hard way, this method needs to stay at the bottom of this script or else any functions below this will not be recognized when compiling.
/// I am a c# person, not a c++ person, so I have no idea why that is.
/// </summary>
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        //do the thing
        CreateThread(nullptr, 0, ReaderThread, nullptr, 0, nullptr);
    }
    return TRUE;
}


