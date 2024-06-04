#include <iostream>
#include <thread>
#include <chrono>
#include <locale>
#include <codecvt>

#include "mouseHook.h"

Database g_db;

uint64_t timeSinceEpochMillisec2()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void getActiveWindowTitle(std::string& windowTitle) {
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

LRESULT CALLBACK  MouseProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
    if (nCode >= 0) 
    {
        if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN) 
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            std::string process;
            getActiveWindowTitle(process);

            std::cout << "Mouse Clicked at: X = " << x << ", Y = " << y << std::endl;
            const uint64_t time = timeSinceEpochMillisec2();
            g_db.insertMouseKey("mouse", time, wParam, process);
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void MouseHook::mouseHandler()
{
    m_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), 0);

    if (m_mouseHook == NULL)
    {
        std::cerr << "Failed to install mouse hook!" << std::endl;
        return;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void MouseHook::start(Database db)
{
    g_db = db;
    std::thread mouseHandlerThread(&MouseHook::mouseHandler, this);
    m_activeThreads.push_back(std::move(mouseHandlerThread));
}

void MouseHook::stop()
{
    UnhookWindowsHookEx(m_mouseHook);

    for (auto& thread : m_activeThreads)
    {
        thread.join();
    }
}