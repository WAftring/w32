#pragma once
#include <Windows.h>
#include <winhttp.h>
class IHandle
{
	HINTERNET m_handle;
public:
	IHandle(HINTERNET handle) : m_handle(handle)
	{

	}
	bool is_null() { return NULL == m_handle; }
	~IHandle()
	{
		if (!is_null())
			WinHttpCloseHandle(m_handle);
	}
	HINTERNET get_data()
	{
		return m_handle;
	}
};

