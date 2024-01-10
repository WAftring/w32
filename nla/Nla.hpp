#pragma once
#include "W32Module.hpp"
#include <winsock2.h>
#include <MSWSock.h>

class Nla : public W32Module
{
	WSADATA m_wsd;
public:
	Nla();
	~Nla()
	{
		WSACleanup();
	}
	void help();
};

