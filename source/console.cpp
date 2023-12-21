#define _CRT_SECURE_NO_WARNINGS
#include "console.h"

namespace Console
{
	void Console::Write(string text)
	{
		freopen("CONOUT$", "w", stdout);
		printf(text.c_str());
	}

	void Console::WriteLine(string text)
	{
		text += "\n";
		freopen("CONOUT$", "w", stdout);
		printf(text.c_str());
	}

	string Console::Input()
	{
		string text;
		freopen("CONIN$", "r", stdin);
		getline(cin, text);
		return text;
	}

	void Console::Clear()
	{
		COORD topLeft = {0, 0};
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO screen;
		DWORD written;

		GetConsoleScreenBufferInfo(console, &screen);
		FillConsoleOutputCharacterA(
			console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
		FillConsoleOutputAttribute(
			console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
			screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
		SetConsoleCursorPosition(console, topLeft);
	}

	void Console::Init()
	{
		AllocConsole();
	}
}