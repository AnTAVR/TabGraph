/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/17 17:38:55 by gpinchon          #+#    #+#             */
/*   Updated: 2019/03/30 10:30:12 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once

#include "GLIncludes.hpp"
#include <iostream>

#define consoleLog(message) std::cout << message << std::endl;

#ifdef DEBUG_MOD
#define _debugStream(func, line) std::cerr << __DATE__ << " " << __TIME__ << " | " << func << " at line [" << line << "] : "
#define debugLog(message) _debugStream(__PRETTY_FUNCTION__, __LINE__) << message << std::endl;
#define glCheckError() _glCheckError(__PRETTY_FUNCTION__, __LINE__)
inline auto _glCheckError(const char* func, const int line)
{
    GLenum errorCode;
    GLenum errorRet = GL_NO_ERROR;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        errorRet |= errorCode;
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
            error = "GL_INVALID_ENUM";
            break;
            case GL_INVALID_VALUE:
            error = "GL_INVALID_VALUE";
            break;
            case GL_INVALID_OPERATION:
            error = "GL_INVALID_OPERATION";
            break;
            case GL_STACK_OVERFLOW:
            error = "GL_STACK_OVERFLOW";
            break;
            case GL_STACK_UNDERFLOW:
            error = "GL_STACK_UNDERFLOW";
            break;
            case GL_OUT_OF_MEMORY:
            error = "GL_OUT_OF_MEMORY";
            break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        _debugStream(func, line) << error << std::endl;
    }
    return errorRet;
}
#else
#define debugLog(message)
inline auto glCheckError()
{
    return (GL_NO_ERROR);
}
#endif

#ifdef _WIN32

#undef MAT2
#include <windows.h>
#include <DbgHelp.h>

inline void stack_trace(void)
{

    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    CONTEXT context;
    memset(&context, 0, sizeof(CONTEXT));
    context.ContextFlags = CONTEXT_FULL;
    RtlCaptureContext(&context);

    SymInitialize(process, nullptr, true);
    SymSetOptions(SYMOPT_LOAD_LINES);

    DWORD image;
    STACKFRAME stackframe;
    ZeroMemory(&stackframe, sizeof(STACKFRAME64));

#ifdef _M_IX86
    image = IMAGE_FILE_MACHINE_I386;
    stackframe.AddrPC.Offset = context.Eip;
    stackframe.AddrPC.Mode = AddrModeFlat;
    stackframe.AddrFrame.Offset = context.Ebp;
    stackframe.AddrFrame.Mode = AddrModeFlat;
    stackframe.AddrStack.Offset = context.Esp;
    stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
    image = IMAGE_FILE_MACHINE_AMD64;
    stackframe.AddrPC.Offset = context.Rip;
    stackframe.AddrPC.Mode = AddrModeFlat;
    stackframe.AddrFrame.Offset = context.Rsp;
    stackframe.AddrFrame.Mode = AddrModeFlat;
    stackframe.AddrStack.Offset = context.Rsp;
    stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
    image = IMAGE_FILE_MACHINE_IA64;
    stackframe.AddrPC.Offset = context.StIIP;
    stackframe.AddrPC.Mode = AddrModeFlat;
    stackframe.AddrFrame.Offset = context.IntSp;
    stackframe.AddrFrame.Mode = AddrModeFlat;
    stackframe.AddrBStore.Offset = context.RsBSP;
    stackframe.AddrBStore.Mode = AddrModeFlat;
    stackframe.AddrStack.Offset = context.IntSp;
    stackframe.AddrStack.Mode = AddrModeFlat;
#endif


    while (StackWalk(
          image, process, thread,
          &stackframe, &context, nullptr, 
          SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
    {
        std::string moduleName = "???";
        std::string functionName = "???";
        std::string fileName = "???";
        unsigned int lineNumber = 0;

        HINSTANCE moduleBase = (HINSTANCE)SymGetModuleBase(process, stackframe.AddrPC.Offset);
        char moduleBuff[MAX_PATH];
        if (moduleBase && GetModuleFileNameA(moduleBase, moduleBuff , MAX_PATH))
            moduleName = moduleBuff ;

        char symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + MAX_SYM_NAME];
        PIMAGEHLP_SYMBOL symbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
        symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL) + MAX_SYM_NAME;
        symbol->MaxNameLength = MAX_SYM_NAME;

        if (SymGetSymFromAddr(process, stackframe.AddrPC.Offset, NULL, symbol))
            functionName = symbol->Name;

        DWORD  offset = 0;
        IMAGEHLP_LINE line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

        if (SymGetLineFromAddr(process, stackframe.AddrPC.Offset, &offset, &line))
        {
            fileName = line.FileName;
            lineNumber = line.LineNumber;
        }

        std::cerr << "\t[" << moduleName << "] " << functionName << " in file : " << fileName << " at line " << lineNumber << std::endl;
    }
    SymCleanup(process);
}

#else

#include <execinfo.h>

inline void stack_trace(void)
{
    void *array[1024];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 1024);
    strings = backtrace_symbols(array, size);
    std::cerr << "Obtained " << size << " stack frames.\n";
    for (i = 0; i < size; i++) {
        std::cerr << strings[i] << std::endl;
    }
    free (strings);
}

#endif //_WIN32

inline void sigHandler(int signum) {
    std::cerr << "Received interuption signal(" << signum << ")" << std::endl;
    std::cerr << "Print Stack :" << std::endl;
    stack_trace();
    exit(signum);
}