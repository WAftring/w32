// dpapi.cpp : Defines the functions for the static library.
//

#include "Dpapi.hpp"
#include <dpapi.h>
#include <cstdlib>
#define LIST_BUFFER L"list-buffer"
#define ADD_BUFFER L"add-buffer"
#define ENCRYPT_BUFFER L"encrypt-buffer"
#define DECRYPT_BUFFER L"decrypt-buffer"

#pragma comment(lib, "crypt32.lib")

class Buffer
{
	BYTE* m_data;
	size_t m_size;
public:
	BYTE* get_data()
	{
		return m_data;
	}
	size_t get_size() { return m_size; }
	Buffer(size_t size) : m_size(size)
	{
		m_data = new BYTE[m_size];
		memset(m_data, 0, m_size);
	}
	~Buffer()
	{
		delete[]m_data;
	}
};

void list_buffer(std::vector<std::wstring>& tokens);
void add_buffer(std::vector<std::wstring>& tokens);
void encrypt_buffer(std::vector<std::wstring>& tokens);
void decrypt_buffer(std::vector<std::wstring>& tokens);

std::vector<Buffer*> g_buffers;

Dpapi::Dpapi()
{
	m_functions[LIST_BUFFER] = list_buffer;
	m_functions[ADD_BUFFER] = add_buffer;
	m_functions[ENCRYPT_BUFFER] = encrypt_buffer;
	m_functions[DECRYPT_BUFFER] = decrypt_buffer;
	m_name = L"dpapi";
}

void Dpapi::help()
{
	wprintf(LIST_BUFFER L"\tlist buffers\n");
	wprintf(ADD_BUFFER L"\tadd buffer\n");
	wprintf(ENCRYPT_BUFFER L"\t encrypt an input buffer\n");
	wprintf(DECRYPT_BUFFER L"\tdecrypt an input buffer\n");
}

void list_buffer(std::vector<std::wstring>& tokens)
{
	int i = 0;
	if (g_buffers.size() != 0)
	{
		for (auto it : g_buffers)
			wprintf(L"[%d] 0x%p %s\n", i++, reinterpret_cast<void*>(it->get_data()), reinterpret_cast<wchar_t*>(it->get_data()));
	}
	else
		wprintf(L"No buffers\n");
}

void add_buffer(std::vector<std::wstring>& tokens)
{
	DWORD diff = (CRYPTPROTECTMEMORY_BLOCK_SIZE - (tokens[0].size() * 2) % CRYPTPROTECTMEMORY_BLOCK_SIZE);
	DWORD buffer_size = (tokens[0].size() * 2) + diff;
	Buffer* plaintext = new Buffer(buffer_size);
	memcpy(plaintext->get_data(), tokens[0].c_str(), (tokens[0].size() * 2));
	
		g_buffers.push_back(plaintext);
}

void encrypt_buffer(std::vector<std::wstring>& tokens)
{
	int index = std::stoi(tokens[0]);
	if (index > (g_buffers.size() - 1) || index < 0)
		wprintf(L"Index %d out of range\n", index);
	else
	{
		Buffer* buffer = g_buffers[index];
		if (!CryptProtectMemory((LPVOID)buffer->get_data(), buffer->get_size(), CRYPTPROTECTMEMORY_SAME_PROCESS))
			wprintf(L"CryptProtectMemory failed with error 0x%x\n", GetLastError());
		else
			wprintf(L"[%d] 0x%p %s\n", index, reinterpret_cast<void*>(buffer->get_data()), reinterpret_cast<wchar_t*>(buffer->get_data()));
	}
}

void decrypt_buffer(std::vector<std::wstring>& tokens)
{
	int index = std::stoi(tokens[0]);
	if(index > (g_buffers.size() -1) || index < 0)
		wprintf(L"Index %d out of range\n", index);
	else
	{
		Buffer* buffer = g_buffers[index];
		if(!CryptUnprotectMemory((LPVOID)buffer->get_data(), buffer->get_size(), CRYPTPROTECTMEMORY_SAME_PROCESS))
			wprintf(L"CryptUnprotectMemory failed with error 0x%x\n", GetLastError());
		else
			wprintf(L"[%d] 0x%p %s\n", index, reinterpret_cast<void*>(buffer->get_data()), reinterpret_cast<wchar_t*>(buffer->get_data()));
	}
}
