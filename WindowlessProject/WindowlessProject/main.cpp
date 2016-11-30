#include <Windows.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
using namespace std;

int WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       cmdShow)
{
	time_t t = time(0);   // get time now

	struct tm now;

	localtime_s(&now, &t);

	string filename = to_string(now.tm_year + 1900);
	filename += '-';
	filename += to_string(now.tm_mon + 1);
	filename += '-';
	filename += to_string(now.tm_mday);
	filename += ' ';
	filename += to_string(now.tm_hour);
	filename += '-';
	filename += to_string(now.tm_min);
	filename += '-';
	filename += to_string(now.tm_mday);
	filename += ".txt";

	//std::getline(std::cin, filename);

	ofstream fout(filename.c_str(), ios::out | ios::trunc);
	fout << "Hello World!\n";
	fout.close();

	return 0;
}