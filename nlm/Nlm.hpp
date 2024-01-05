#pragma once

#include "W32Module.hpp"
#include <objbase.h>
class Nlm : public W32Module
{
public:
	Nlm() 
	{
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		m_name = L"nlm";
	}
	~Nlm()
	{
		CoUninitialize();
	}
	void process_input(std::vector<std::wstring>& tokens);
	void help();
};