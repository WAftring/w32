#pragma once

#include "W32Module.hpp"
#include <objbase.h>
class Nlm : public W32Module
{
public:
	Nlm();
	~Nlm()
	{
		CoUninitialize();
	}
	void process_input(std::vector<std::wstring>& tokens);
	void help();
};