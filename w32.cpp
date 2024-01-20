// w32.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <cstdio>
#include <sstream>
#include <iterator>
#include <algorithm>
#include "W32Module.hpp"
#include "nlm/Nlm.hpp"
#include "nla/Nla.hpp"
#include "winhttp/Winhttp.h"
#include "dpapi/Dpapi.hpp"
#include "wevt/Wevt.hpp"
#define VERSION L"1.0"
#define NLM_MODULE L"nlm"
#define NLA_MODULE L"nla"
#define WINHTTP_MODULE L"winhttp"
#define WEVT_MODULE L"wevt"
W32Module* g_activeModule = nullptr;

W32Module* build_module(const std::wstring input);
void process_input(const std::wstring& input);
void help();
void supported_modules();

int wmain(int argc, wchar_t* argv[])
{
    wprintf(L"w32 v%s\n", VERSION);
    if (argc != 1)
    {
        std::vector<std::wstring> tokens;
        for (int i = 1; i < argc; i++)
            tokens.push_back(argv[i]);
        // Try and find the module
        if (L"help" == tokens[0])
        {
            help();
            supported_modules();
            return 0;
        }

        W32Module* mod = build_module(tokens[0]);
        if (nullptr == mod)
        {
            wprintf(L"Invalid module %s\n", tokens[0].c_str());
            return -1;
        }
        tokens.erase(tokens.begin());
        mod->process_input(tokens);
        delete mod;
        return 0;
    }
    
    bool running = true;
    while (running)
    {
        std::wstring input;
        wprintf(L"> ");
        std::getline(std::wcin, input);
        if (input == L"quit")
            running = false;
        else
            process_input(input);
    }
    return 0;
}

W32Module* build_module(const std::wstring input)
{
    // TODO(will): Replace these with the defines
    if (L"nlm" == input)
        return new Nlm();
    else if (L"nla" == input)
        return new Nla();
    else if (L"winhttp" == input)
        return new Winhttp();
    else if (L"dpapi" == input)
        return new Dpapi();
    else if (L"wevt" == input)
        return new Wevt();
    return nullptr;
}

void process_input(const std::wstring& input)
{
    if (L"reset" == input && nullptr != g_activeModule)
    {
        delete g_activeModule;
        g_activeModule = nullptr;
    }
    else if (L"mod" == input)
        wprintf(L"active module: %ws\n", g_activeModule == nullptr ? L"none" : g_activeModule->get_name().c_str());
    else if (L"help" == input)
    {
        help();
        if (nullptr != g_activeModule)
            g_activeModule->help();
        else
            supported_modules();
    }
    else if (nullptr != g_activeModule)
    {
        std::wistringstream iss(input);
        std::wstring token;
        std::vector<std::wstring> tokens;
        while (std::getline(iss, token, L' '))
            tokens.push_back(token);
        g_activeModule->process_input(tokens);
    }
    else
    {
        W32Module* mod = build_module(input);
        if (nullptr != mod)
            g_activeModule = mod;
        else
            wprintf(L"Invalid module: %ws\n", input.c_str());
    }
}

void help()
{
    wprintf(L"General:\n==========\n");
    wprintf(L"quit\tquit the program\n");
    wprintf(L"mod\tlist active module\n");
    wprintf(L"reset\tclear active module\n");
    wprintf(L"\n");
}

void supported_modules()
{
    wprintf(L"Supported Modules:\n==========\n");
    wprintf(NLM_MODULE L"\tnetwork list manager\n");
    wprintf(NLA_MODULE L"\tnetwork location awareness\n");
    wprintf(WINHTTP_MODULE L"\twinhttp\n");
    wprintf(WEVT_MODULE L"\twindows event log\n");
}
