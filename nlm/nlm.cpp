// nlm.cpp : Defines the functions for the static library.
//

#include "Nlm.hpp"
#include <netlistmgr.h>
#include <atlbase.h>
#include <comutil.h>
#include <map>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "comsuppwd.lib")

#define GET_NETWORKS			L"get-networks"
#define GET_NETWORK_DETAILS		L"get-network-details"
#define GET_CONNECTIONS			L"get-connections"
#define GET_CONNECTION_DETAILS	L"get-connection-details"

void get_networks(std::vector<std::wstring>& tokens);
void get_network_details(std::vector<std::wstring>& tokens);
void get_connections(std::vector<std::wstring>& tokens);
void get_connection_details(std::vector<std::wstring>& tokens);

void print_network(INetwork* network, bool verbose);
void print_connection(INetworkConnection* connection, bool verbose);

Nlm::Nlm()
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	m_name = L"nlm";
	m_functions[GET_NETWORKS] = get_networks;
	m_functions[GET_NETWORK_DETAILS] = get_network_details;
	m_functions[GET_CONNECTIONS] = get_connections;
	m_functions[GET_CONNECTION_DETAILS] = get_connection_details;
}

void Nlm::help()
{
	wprintf(L"Nlm:\n==========\n");
	wprintf(GET_NETWORKS L"\tget all networks\n");
	wprintf(GET_NETWORK_DETAILS L" <network id>\tget network details\n");
}

void get_networks(std::vector<std::wstring>& tokens)
{
	bool verbose = false;
	if (tokens.size() != 0)
	{
		if (tokens[0] == L"verbose")
			verbose = true;
	}
	CComPtr<INetworkListManager> nlm = nullptr;
	HRESULT hr = nlm.CoCreateInstance(CLSID_NetworkListManager);
	if (FAILED(hr))
	{
		wprintf(L"Failed to create network list manager with 0x%x\n", hr);
		return;
	}
	
	CComPtr<IEnumNetworks> networks = nullptr;
	hr = nlm->GetNetworks(NLM_ENUM_NETWORK_ALL, &networks);
	if (FAILED(hr))
	{
		wprintf(L"Failed to get networks with 0x%x\n", hr);
		return;
	}
	wprintf(L"Enumerating networks\n");
	while (true)
	{
		ULONG result = 0;
		CComPtr<INetwork> network;
		hr = networks->Next(1, &network, &result);
		if (FAILED(hr) || result <= 0)
			break;
		print_network(network, verbose);
	}
	

}

void get_network_details(std::vector<std::wstring>& tokens)
{
	CLSID guid;
	HRESULT hr = CLSIDFromString(tokens[0].c_str(), &guid);
	if (FAILED(hr))
	{
		wprintf(L"Invalid network id: %s\n", tokens[0].c_str());
		return;
	}
	CComPtr<INetworkListManager> nlm = nullptr;
	hr = nlm.CoCreateInstance(CLSID_NetworkListManager);
	if (FAILED(hr))
	{
		wprintf(L"Failed to create network list manager with error 0x%x\n", hr);
		return;
	}
	CComPtr<INetwork> network;
	hr = nlm->GetNetwork(guid, &network);
	if (FAILED(hr))
	{
		wprintf(L"Failed to find network %s with error 0x%x\n", tokens[0].c_str(), hr);
		return;
	}
	print_network(network, true);
}

void get_connections(std::vector<std::wstring>& tokens)
{
	CComPtr<INetworkListManager> nlm = nullptr;
	HRESULT hr = nlm.CoCreateInstance(CLSID_NetworkListManager);
	if (FAILED(hr))
	{
		wprintf(L"Failed to create network list manager with error 0x%x\n", hr);
		return;
	}
	CComPtr<IEnumNetworkConnections> connections = nullptr;
	hr = nlm->GetNetworkConnections(&connections);
	if (FAILED(hr))
	{
		wprintf(L"Failed to get network connections with error 0x%x\n", hr);
		return;
	}

	wprintf(L"Enumerating connections\n");
	while (true)
	{
		ULONG result = 0;
		CComPtr<INetworkConnection> connection = nullptr;
		hr = connections->Next(1, &connection, &result);
		if (FAILED(hr) || result <= 0)
			break;
		wprintf(L"\t");
		print_connection(connection, false);
	}

}

void get_connection_details(std::vector<std::wstring>& tokens)
{
	CLSID guid;
	HRESULT hr = CLSIDFromString(tokens[0].c_str(), &guid);
	if (FAILED(hr))
	{
		wprintf(L"Invalid connection id %s\n", tokens[0].c_str());
		return;
	}

	CComPtr<INetworkListManager> nlm = nullptr;
	hr = nlm.CoCreateInstance(CLSID_NetworkListManager);
	if (FAILED(hr))
	{
		wprintf(L"Failed to create network list manager with error 0x%x\n", hr);
		return;
	}
	CComPtr<INetworkConnection> connection = nullptr;
	hr = nlm->GetNetworkConnection(guid, &connection);
	if (FAILED(hr))
	{
		wprintf(L"Failed to find connection %s with error 0x%x\n", tokens[0].c_str(), hr);
		return;
	}

	wprintf(L"Connection Details:\n\t");
	print_connection(connection, true);
}

void print_network(INetwork* network, bool verbose)
{
	BSTR name;
	BSTR desc;
	GUID guid;
	OLECHAR* guid_string = nullptr;
	NLM_DOMAIN_TYPE type;
	NLM_CONNECTIVITY connectivity;
	network->GetName(&name);
	network->GetDescription(&desc);
	network->GetNetworkId(&guid);
	network->GetDomainType(&type);
	network->GetConnectivity(&connectivity);
	StringFromCLSID(guid, &guid_string);
	wprintf(L"Network Details:\n");
	wprintf(L"Name: %s %s desc: %s type: %x connectivity: %x\n", name, guid_string, desc, type, connectivity);
	CoTaskMemFree(guid_string);

	if (!verbose)
		return;

	wprintf(L"Network Properties:\n");
	std::wstring bool_properties[] = { 
		L"NA_DomainAuthentication_Failed", 
		L"NA_NameSetByPolicy",
		L"NA_IconSetByPolicy",
		L"NA_DescriptionSetByPolicy",
		L"NA_CategorySetByPolicy",
		L"NA_NameReadOnly",
		L"NA_IconReadOnly",
		L"NA_DescriptionReadOnly",
		L"NA_CategoryReadOnly",
		L"NA_AllowMerge" };

	std::wstring uint_properties[] = {
		L"NA_NetworkClass",
		L"NA_InternetConnectivityV4",
		L"NA_InternetConnectivityV6"
	};

	CComPtr<IPropertyBag> properties;
	if (SUCCEEDED(network->QueryInterface(IID_IPropertyBag, (void**)&properties)))
	{
		VARIANT property;
		VariantInit(&property);

		for (auto prop : bool_properties)
		{
			if (SUCCEEDED(properties->Read(prop.c_str(), &property, 0)))
				wprintf(L"\t- %-30s\t%-10s\n", prop.c_str(), property.boolVal == TRUE ? L"true" : L"false");
			else
				wprintf(L"\t- %-30s\t%-10s\n", prop.c_str(), L"unknown");
			VariantClear(&property);
		}

		for (auto prop : uint_properties)
		{
			if (SUCCEEDED(properties->Read(prop.c_str(), &property, 0)))
				wprintf(L"\t- %-30s\t%-10x\n", prop.c_str(), property.uintVal);
			else
				wprintf(L"\t- %-30s\t%-10s\n", prop.c_str(), L"unknown");
			VariantClear(&property);
		}
	}
}

void print_connection(INetworkConnection* connection, bool verbose)
{
	GUID adapter;
	GUID connection_id;
	OLECHAR* adapter_str = nullptr;
	OLECHAR* connection_str = nullptr;
	NLM_CONNECTIVITY connectivity;
	NLM_DOMAIN_TYPE type;
	connection->GetAdapterId(&adapter);
	connection->GetConnectionId(&connection_id);
	connection->GetConnectivity(&connectivity);
	connection->GetDomainType(&type);

	StringFromCLSID(adapter, &adapter_str);
	StringFromCLSID(connection_id, &connection_str);

	wprintf(L"Connection id %s Adapter Id %s Connectivity %x Domain Type %x\n",
		connection_str,
		adapter_str,
		connectivity,
		type);
}
