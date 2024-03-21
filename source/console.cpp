#include "console.hpp"

#ifndef DISABLE_CONSOLE
namespace Console {
    bool inited = false;

    void Init() {
        if (inited) {
            return;
        }

        inited = true;

        AllocConsole();
        SetWindowPos(GetConsoleWindow(), 0, 10, 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        FILE* pCout;
        freopen_s(&pCout, "CONOUT$", "w", stdout);
        std::ios::sync_with_stdio();
    }

    void Write(const std::string text) {
        if (inited) // Init();
            std::cout << text.c_str();
    }

    void WriteLn(const std::string text) {
        Write(text + "\n");
    }

    void WriteF(const char* format, ...) {
        if (!inited) return;

        va_list args;
        va_start(args, format);

        int size = _vscprintf(format, args) + 1;
        char* buffer = new char[size];
        vsnprintf_s(buffer, size, _TRUNCATE, format, args);

        std::cout << buffer;

        delete[] buffer;

        va_end(args);
    }

    void Clear() {
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
}
#else
namespace Console {
    void Init() {}
    void Write(const std::string text) {}
    void WriteLn(const std::string text) {}
    void WriteF(const char* format, ...) {}
    void Clear() {}
}
#endif