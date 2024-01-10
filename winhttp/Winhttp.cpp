#include "Winhttp.h"
#include <winhttp.h>
#include "IHandle.h"

#define MAKE_REQ L"make-req"
void make_request(std::vector<std::wstring>& tokens);

#pragma comment(lib, "Winhttp.lib")

Winhttp::Winhttp()
{
	m_name = L"winhttp";
	m_functions[MAKE_REQ] = make_request;
}

void Winhttp::help()
{
	wprintf(MAKE_REQ L" <resource>\tmake a winhttp request to resource\n");
}

void make_request(std::vector<std::wstring>& tokens)
{
	std::wstring resource = tokens[0];
	wprintf(L"Making winhttp request to %s\n", resource.c_str());
	IHandle session = WinHttpOpen(L"W32 / 1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
	if (session.is_null())
	{
		wprintf(L"WinHttpOpen failed with error 0x%x\n", GetLastError());
		return;
	}

	URL_COMPONENTSW components = { 0 };
	components.dwStructSize = sizeof(components);
	components.dwHostNameLength = (DWORD)-1;
	components.dwUrlPathLength = (DWORD)-1;
	
	if (TRUE != WinHttpCrackUrl(resource.c_str(), resource.size(), 0, &components))
	{
		wprintf(L"WinHttpCrack URL for %s failed with error 0x%x\n", resource.c_str(), GetLastError());
		return;
	}

	wchar_t hostname[255] = { 0 };
	memcpy_s(hostname, 255, components.lpszHostName, (components.dwHostNameLength - components.dwUrlPathLength) * 2);

	IHandle connect = WinHttpConnect(session.get_data(), hostname, components.nPort, 0);
	if (connect.is_null())
	{
		wprintf(L"WinHttpConnect to %s:%d failed with error 0x%x\n", hostname, components.nPort, GetLastError());
		return;
	}

	IHandle request = WinHttpOpenRequest(connect.get_data(), L"GET", components.lpszUrlPath, NULL, NULL, NULL, 0);
	if (request.is_null())
	{
		wprintf(L"WinHttpOpenRequest failed with error 0x%x\n", GetLastError());
		return;
	}

	BOOL result = WinHttpSendRequest(request.get_data(), WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, WINHTTP_IGNORE_REQUEST_TOTAL_LENGTH, NULL);
	if (!result)
	{
		wprintf(L"WinHttpSendRequest failed with error 0x%x\n", GetLastError());
		return;
	}

	WinHttpReceiveResponse(request.get_data(), NULL);
	// Print the header
	wchar_t* headers = new wchar_t[4096];
	memset(headers, 0, 4096 * 2);
	DWORD header_size = 4096;
	DWORD num_headers = 0;
	wprintf(L"Headers:\n");
	if (WinHttpQueryHeaders(request.get_data(), WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, (LPVOID)headers, &header_size, &num_headers))
		wprintf(L"%s", headers);
	else
		wprintf(L"WinHttpQueryHeaders failed with error 0x%x\n", GetLastError());
	wprintf(L"Content:\n");
	while (true)
	{
		char buffer[1024] = { 0 };
		DWORD bytes_read = 0;
		BOOL success = WinHttpReadData(request.get_data(), buffer, sizeof(buffer), &bytes_read);
		if (!success || bytes_read == 0)
			break;
		printf("%s", buffer);
	}
	delete []headers;
	wprintf(L"\n");
}
