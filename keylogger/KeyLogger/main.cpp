#include "mouseHook.h"

#include <ctime>
#include <iostream>
#include <thread>
#include <fstream>
#include <iterator>
#include <ctime>


int main(int argc, char** argv)
{
	Database db;
	std::ifstream config("../../config.txt");
	time_t currentTime = time(nullptr);

	std::string dbName = "sqlite-data/data_" + std::to_string(currentTime) + ".sqlite3";

	db.createDatabase(dbName);

	std::string keyboardCols[4] = { "time", "key", "isPressed", "process"};
	db.createTable("keyboard", 4, keyboardCols);
	db.clear("keyboard");

	std::string mouseCols[3] = { "time", "key", "process"};
	db.createTable("mouse", 3, mouseCols);
	db.clear("mouse");

	MouseHook mouseHook;
	mouseHook.start(db);

	Keylogger keylogger;
	keylogger.start(db);

	return 0;
}