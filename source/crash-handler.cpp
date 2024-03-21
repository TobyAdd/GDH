#include "crash-handler.hpp"
#include "replayEngine.hpp"
#include <MinHook.h>
#pragma comment(lib, "DbgHelp")

bool symbolsInitialized = false;

std::wstring getModuleName(HMODULE module, bool fullPath = true)
{
    wchar_t buffer[MAX_PATH];
    if (!GetModuleFileNameW(module, buffer, MAX_PATH))
        return L"Unknown";
    if (fullPath)
        return buffer;
    return std::filesystem::path(buffer).filename().wstring();
}

std::wstring getExceptionCodeString(DWORD code)
{
    switch (code)
    {
    case EXCEPTION_ACCESS_VIOLATION:
        return L"EXCEPTION_ACCESS_VIOLATION";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        return L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
    case EXCEPTION_STACK_OVERFLOW:
        return L"EXCEPTION_STACK_OVERFLOW";
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        return L"EXCEPTION_ILLEGAL_INSTRUCTION";
    case EXCEPTION_IN_PAGE_ERROR:
        return L"EXCEPTION_IN_PAGE_ERROR";
    case EXCEPTION_BREAKPOINT:
        return L"EXCEPTION_BREAKPOINT";
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        return L"EXCEPTION_DATATYPE_MISALIGNMENT";
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        return L"EXCEPTION_FLT_DENORMAL_OPERAND";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        return L"EXCEPTION_FLT_DIVIDE_BY_ZERO";
    case EXCEPTION_FLT_INEXACT_RESULT:
        return L"EXCEPTION_FLT_INEXACT_RESULT";
    case EXCEPTION_FLT_INVALID_OPERATION:
        return L"EXCEPTION_FLT_INVALID_OPERATION";
    case EXCEPTION_FLT_OVERFLOW:
        return L"EXCEPTION_FLT_OVERFLOW";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        return L"EXCEPTION_INT_DIVIDE_BY_ZERO";
    default:
        return L"Unknown Exception Code";
    }
}

HMODULE handleFromAddress(void *address) {
    HMODULE hModule = nullptr;
    GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCTSTR) address, &hModule);
    return hModule;
}

void printAddr(std::wstring &result, const void *address, bool fullPath = true)
{
    std::wstringstream ss;
    HMODULE hModule = nullptr;

    if (GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCTSTR) address, &hModule)) {
        auto const diff = (uintptr_t) address - (uintptr_t) hModule;
        ss << getModuleName(hModule, fullPath) << L" + 0x" << std::hex << diff << std::dec;

        if (symbolsInitialized) {
            DWORD64 displacement64 = 0;
            wchar_t buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
            auto pSymbol = (PSYMBOL_INFO) buffer;
            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            pSymbol->MaxNameLen = MAX_SYM_NAME;

            auto process = GetCurrentProcess();

            if (SymFromAddr(process, (DWORD64) address, &displacement64, pSymbol)) {
                ss << L" (" << pSymbol->Name << L" + 0x" << std::hex << displacement64 << std::dec;

                IMAGEHLP_LINE64 line;
                line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                DWORD displacement;

                if (SymGetLineFromAddr64(process, (DWORD64) address, &displacement, &line)) {
                    ss << L" in " << line.FileName << L" at line " << line.LineNumber;
                }

                ss << L")";
            }
        }
    } else {
        ss << address;
    }

    result += ss.str();
}

void printRegisterStates(std::wstring &content, PCONTEXT context)
{
    wchar_t buffer[200];

    swprintf_s(
        buffer,
        L"- EAX: %08X\n"
        L"- EBX: %08X\n"
        L"- ECX: %08X\n"
        L"- EDX: %08X\n"
        L"- ESI: %08X\n"
        L"- EDI: %08X\n"
        L"- EBP: %08X\n"
        L"- ESP: %08X\n"
        L"- EIP: %08X\n\n",
        context->Eax, context->Ebx, context->Ecx, context->Edx,
        context->Esi, context->Edi, context->Ebp, context->Esp, context->Eip
    );

    content += buffer;
}

void walkStack(std::wstring &result, PCONTEXT context)
{
    STACKFRAME64 stack;
    memset(&stack, 0, sizeof(STACKFRAME64));

    auto process = GetCurrentProcess();
    auto thread = GetCurrentThread();
    stack.AddrPC.Offset = context->Eip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrStack.Offset = context->Esp;
    stack.AddrStack.Mode = AddrModeFlat;
    stack.AddrFrame.Offset = context->Ebp;
    stack.AddrFrame.Mode = AddrModeFlat;

    while (true)
    {
        if (!StackWalk64(IMAGE_FILE_MACHINE_I386, process, thread, &stack, context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
            break;

        wchar_t buffer[512];
        swprintf_s(buffer, L"- ");
        result += buffer;

        printAddr(result, reinterpret_cast<void *>(stack.AddrPC.Offset));
        result += L"\n";
    }
}

LONG WINAPI handler(EXCEPTION_POINTERS *info)
{
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    symbolsInitialized = SymInitialize(GetCurrentProcess(), nullptr, TRUE);

    std::wstring message;
    message += L"An exception has occurred.\n";
    message += L"Tip: You can use 'CTRL + C' to copy this message.\n\n";

    wchar_t buffer[512];

    message += L"\nException Information:\n";
    
    swprintf_s(buffer, L"- Exception Code: %X (%s)\n", info->ExceptionRecord->ExceptionCode, getExceptionCodeString(info->ExceptionRecord->ExceptionCode).c_str());
    message += buffer;

    swprintf_s(buffer, L"- Exception Flags: %X\n", info->ExceptionRecord->ExceptionFlags);
    message += buffer;

    swprintf_s(buffer, L"- Exception Address: %p (", info->ExceptionRecord->ExceptionAddress);
    message += buffer;
    printAddr(message, info->ExceptionRecord->ExceptionAddress, false);
    message += L")\n";

    swprintf_s(buffer, L"- Number Parameters: %d\n", info->ExceptionRecord->NumberParameters);
    message += buffer;

    message += L"\nStack Trace:\n";
    walkStack(message, info->ContextRecord);

    message += L"\nRegister States:\n";
    printRegisterStates(message, info->ContextRecord);

    MessageBoxW(nullptr, message.c_str(), L"Geometry Dash Crashed (Crash Handler)", MB_OK | MB_ICONERROR);

    if (!engine.replay2.empty()) {
        int result = MessageBoxA(nullptr, "It looks like you have a macro in the buffer and it may not be saved. Save to crash.re?"
        "\n\nNote: It may overwrite a past replay with the name \"crash\"\nRecommend renaming to a different replay name in GDH/macros directory",
        "Replay Engine", MB_YESNO | MB_ICONWARNING);

        if (result == IDYES) {
            MessageBoxA(nullptr, engine.save("crash").c_str(), "Result", MB_OK | MB_ICONINFORMATION);
        }
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

void crashHandler::init() {
    AddVectoredExceptionHandler(0, [](PEXCEPTION_POINTERS ExceptionInfo) -> LONG {
        SetUnhandledExceptionFilter(handler);
        return EXCEPTION_CONTINUE_SEARCH;
    });
    SetUnhandledExceptionFilter(handler);
}