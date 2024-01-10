#pragma once

#include <string>
#include <vector>
#include <cstdio>
#include <map>
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
typedef void(*InputFunc)(std::vector<std::wstring>& tokens);
class W32Module
{
protected:
	std::wstring m_name;
	std::map<const std::wstring, InputFunc> m_functions;
public:
	const std::wstring get_name() { return m_name; }
	void process_input(std::vector<std::wstring>& tokens)
	{
		std::wstring func = tokens[0];
		if (L"help" == func)
			help();
		else if (m_functions.find(func) != m_functions.end())
		{
			if (tokens.size() == 1)
				tokens.clear();
			else
				tokens.erase(tokens.begin());
			m_functions[func](tokens);
		}
	}
	virtual void help() = 0;
};