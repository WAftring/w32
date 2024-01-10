#include "Nla.hpp"
#include <windows.h>
#include <map>

#pragma comment(lib, "ws2_32.lib")

#define DUMP_CONFIG L"dump-config"
#define DUMP_HOTSPOT L"dump-hotspot"
void dump_config(std::vector<std::wstring>& tokens);
void dump_hotspot(std::vector<std::wstring>& tokens);
typedef DWORD(WINAPI *NGH)(PWSTR**, PULONG);
Nla::Nla()
{
	WSAStartup(MAKEWORD(2, 2), &m_wsd);
    m_name = L"Nla";
	m_functions[DUMP_CONFIG] = dump_config;
}

void Nla::help()
{
    wprintf(L"Nla:\n==========\n");
    wprintf(DUMP_CONFIG L"\tdumps current NLA configuration\n");
    wprintf(DUMP_HOTSPOT, L"\tdump hotspot & info\n");
}

void dump_config(std::vector<std::wstring>& tokens)
{
    constexpr int BUFFER_SIZE = 16384;
    DWORD buffer_len = BUFFER_SIZE;
    int                   Err;
    WSAQUERYSET* qs = reinterpret_cast<WSAQUERYSET*>(new BYTE[BUFFER_SIZE]);
    GUID           NLANameSpaceGUID = NLA_SERVICE_CLASS_GUID;
    HANDLE         hNLA;
    PNLA_BLOB pNLA;
    DWORD     Offset = 0;


    memset(qs, 0, buffer_len);

    // Required values
    qs->dwSize = buffer_len;
    qs->dwNameSpace = NS_NLA;
    qs->lpServiceClassId = &NLANameSpaceGUID;

    // Optional values
    qs->dwNumberOfProtocols = 0;
    qs->lpszServiceInstanceName = NULL;
    qs->lpVersion = NULL;
    qs->lpNSProviderId = NULL;
    qs->lpszContext = NULL;
    qs->lpafpProtocols = NULL;
    qs->lpszQueryString = NULL;
    qs->lpBlob = NULL;

    if (WSALookupServiceBeginW(qs, LUP_RETURN_ALL | LUP_DEEP, &hNLA) == SOCKET_ERROR)
    {
        wprintf(L"WSALookupServiceBegin() failed with error %x\n", WSAGetLastError());
        return;
    }

    while (1)
    {
        printf("Querying for Networks...\n");
        while (1)
        {
            memset(qs, 0, buffer_len);

            if (WSALookupServiceNextW(hNLA, LUP_RETURN_ALL, &buffer_len, qs) == SOCKET_ERROR)
            {
                Err = WSAGetLastError();
                if (Err == WSA_E_NO_MORE)
                {
                    // There is no more data. Stop asking
                    goto Exit;
                }
                wprintf(L"WSALookupServiceNext failed with error %x\n", WSAGetLastError());
                WSALookupServiceEnd(hNLA);
                return;
            }
            wprintf(L"Network Name: %s\n", qs->lpszServiceInstanceName);
            wprintf(L"Comment (if any): %s\n", qs->lpszComment);

            if (qs->lpBlob != NULL)
            {
                // Cycle through BLOB data list
                do
                {
                    pNLA = (PNLA_BLOB) & (qs->lpBlob->pBlobData[Offset]);
                    switch (pNLA->header.type)
                    {
                    case NLA_RAW_DATA:
                        printf("\tNLA Data Type: NLA_RAW_DATA\n");
                        break;

                    case NLA_INTERFACE:
                        printf("\tNLA Data Type: NLA_INTERFACE\n");
                        printf("\t\tType: %d\n", pNLA->data.interfaceData.dwType);
                        printf("\t\tSpeed: %d\n", pNLA->data.interfaceData.dwSpeed);
                        printf("\t\tAdapter Name: %s\n", pNLA->data.interfaceData.adapterName);
                        break;

                    case NLA_802_1X_LOCATION:
                        printf("\tNLA Data Type: NLA_802_1X_LOCATION\n");
                        printf("\t\tInformation: %s\n", pNLA->data.locationData.information);
                        break;

                    case NLA_CONNECTIVITY:
                        printf("\tNLA Data Type: NLA_CONNECTIVITY\n");
                        switch (pNLA->data.connectivity.type)
                        {
                        case NLA_NETWORK_AD_HOC:
                            printf("\t\tNetwork Type: AD HOC\n");
                            break;
                        case NLA_NETWORK_MANAGED:
                            printf("\t\tNetwork Type: Managed\n");
                            break;
                        case NLA_NETWORK_UNMANAGED:
                            printf("\t\tNetwork Type: Unmanaged\n");
                            break;
                        case NLA_NETWORK_UNKNOWN:
                            printf("\t\tNetwork Type: Unknown\n");
                        }

                        switch (pNLA->data.connectivity.internet)
                        {
                        case NLA_INTERNET_NO:
                            printf("\t\tInternet connectivity: No\n");
                            break;
                        case NLA_INTERNET_YES:
                            printf("\t\tInternet connectivity: Yes\n");
                            break;
                        case NLA_INTERNET_UNKNOWN:
                            printf("\t\tInternet connectivity: Unknown\n");
                            break;
                        }
                        break;

                    case NLA_ICS:
                        printf("\tNLA Data Type: NLA_ICS\n");
                        printf("\t\tSpeed: %d\n", pNLA->data.ICS.remote.speed);
                        printf("\t\tType: %d\n", pNLA->data.ICS.remote.type);
                        printf("\t\tState: %d\n", pNLA->data.ICS.remote.state);
                        printf("\t\tMachine Name: %S\n", pNLA->data.ICS.remote.machineName);
                        printf("\t\tShared Adapter Name: %S\n", pNLA->data.ICS.remote.sharedAdapterName);
                        break;
                    default:
                        printf("\tNLA Data Type: Unknown to this program\n");
                        break;
                    }

                    Offset = pNLA->header.nextOffset;

                } while (Offset != 0);
            }
        }
    }
Exit:
    WSALookupServiceEnd(hNLA);
}

void dump_hotspot(std::vector<std::wstring>& tokens)
{
    HMODULE mod = LoadLibraryA("nlaapi.dll");
    NGH NlaGetHotspots = (NGH)GetProcAddress(mod, "NlaGetCaptivePortalHosts");
    PWSTR* hosts = nullptr;
    ULONG host_count = 0;
    DWORD error = NlaGetHotspots(&hosts, &host_count);
    if (ERROR_SUCCESS == error)
    {

    }

}
