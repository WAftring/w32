#pragma once

#include "W32Module.hpp"
#include <vector>
class Dpapi : public W32Module
{
	std::vector<wchar_t*> m_buffers;
public:
	Dpapi();
	~Dpapi();
	void help();
};