#include "Wevt.hpp"
// wevt.cpp : Defines the functions for the static library.
//
#include <AuthZ.h>
#include "wevt.h"
#define SOURCE_NAME L"w32_wevt"
#define REGISTER_SOURCE L"register"
#define REGISTER_SEC L"register-sec"
#define UNREGISTER_SOURCE L"unregister"
#define UNREGISTER_SEC L"unregister-sec"
#define WRITE_EVENT L"write-event"
#define WRITE_SEC L"write-sec"

#define MSG_


void register_source(std::vector<std::wstring>& tokens);
void register_security(std::vector<std::wstring>& tokens);
void unregister_source(std::vector<std::wstring>& tokens);
void unregister_security(std::vector<std::wstring>& tokens);
void write_event(std::vector<std::wstring>& tokens);
void write_security(std::vector<std::wstring>& tokens);
void read_events(std::vector<std::wstring>& tokens);
HANDLE g_log = NULL;


Wevt::Wevt()
{
	m_functions[REGISTER_SOURCE] = register_source;
	m_functions[UNREGISTER_SOURCE] = unregister_source;
	m_functions[WRITE_EVENT] = write_event;
	
	m_name = L"wevt";
}

Wevt::~Wevt()
{
	if (NULL != g_log)
		DeregisterEventSource(g_log);
}

void Wevt::help()
{
	wprintf(L"Wevt:\n==========\n");
	wprintf(REGISTER_SOURCE L"\tregister event source with the application event log\n");
	wprintf(UNREGISTER_SOURCE L"\tunregister event source with application event log\n");
	wprintf(WRITE_EVENT L" <message>\twrite event to registered log\n");
}

void register_source(std::vector<std::wstring>& tokens)
{
	g_log = RegisterEventSourceW(NULL, SOURCE_NAME);
	if (NULL == g_log)
		wprintf(L"RegisterEventSourceW failed with error 0x%x\n", GetLastError());
}

void register_security(std::vector<std::wstring>& tokens)
{
	AUTHZ_SOURCE_SCHEMA_REGISTRATION schema = { 0 };
	//schema.szEventSourceName = SOURCE_NAME;

}

void unregister_source(std::vector<std::wstring>& tokens)
{
	if (NULL != g_log)
	{
		DeregisterEventSource(g_log);
		g_log = NULL;
	}
}

void write_event(std::vector<std::wstring>& tokens)
{
	if (NULL == g_log)
	{
		wprintf(L"Log not registered. Run register to register with the event log\n");
		return;
	}
	const wchar_t* strings[1] = { tokens[0].c_str() };
	if (!ReportEventW(g_log, EVENTLOG_SUCCESS, 3, MSG_W32_EVENT, NULL, 1,
		0, strings, NULL))
		wprintf(L"ReportEventW failed with error 0x%x\n", GetLastError());
	else
		wprintf(L"Wrote %s to application event log with message id %d and provider %s\n", tokens[0].c_str(), 1, SOURCE_NAME);

}

void read_events(std::vector<std::wstring>& tokens)
{
}
