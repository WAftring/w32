#pragma once

#include <string>
#include <vector>
#include <cstdio>
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
typedef void(*InputFunc)(std::vector<std::wstring>& tokens);
class W32Module
{
protected:
	std::wstring m_name;
public:
	const std::wstring get_name() { return m_name; }
	virtual void process_input(std::vector<std::wstring>& tokens) = 0;
	virtual void help() = 0;
};