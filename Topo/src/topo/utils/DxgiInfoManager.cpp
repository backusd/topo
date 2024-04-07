#include "pch.h"
#include "DxgiInfoManager.h"
#include "topo/TopoException.h"
#include "TranslateErrorCode.h"

namespace topo
{
#ifdef TOPO_PLATFORM_WINDOWS

#define DXGI_EXCEPT(hr) EXCEPTION(std::format("DxgiInfoManager Exception\n[Error Code] {0:#x} ({0})\n[Description] {1}", hr, ::topo::TranslateErrorCode(hr)))
#define THROW_DXGI_LAST_EXCEPT() auto _err = GetLastError(); throw EXCEPTION(std::format("DxgiInfoManager Exception\n[Error Code] {0:#x} ({0})\n[Description] {1}", _err, ::topo::TranslateErrorCode(_err)))


DxgiInfoManager::DxgiInfoManager()
{
	// define function signature of DXGIGetDebugInterface
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// load the dll that contains the function DXGIGetDebugInterface
	const auto hModDxgiDebug = LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		THROW_DXGI_LAST_EXCEPT();
	}

	// get address of DXGIGetDebugInterface in dll
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
		);
	if (DxgiGetDebugInterface == nullptr)
	{
		THROW_DXGI_LAST_EXCEPT();
	}

	DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&pDxgiInfoQueue));
}

DxgiInfoManager::~DxgiInfoManager() noexcept
{
	if (pDxgiInfoQueue != nullptr)
	{
		pDxgiInfoQueue->Release();
	}
}

void DxgiInfoManager::Set() noexcept
{
	// set the index (next) so that the next all to GetMessages()
	// will only get errors generated after this call
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++)
	{
		SIZE_T messageLength{};

		// get the size of message i in bytes

		HRESULT hr = pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);
		if (FAILED(hr))
			throw DXGI_EXCEPT(hr);

		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		// get the message and push its description into the vector
		hr = pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength);
		if (FAILED(hr))
			throw DXGI_EXCEPT(hr);

		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}

std::string DxgiInfoManager::GetConcatenatedMessages() const
{
	std::string msg;
	std::vector<std::string> msgs = GetMessages();

	// join all info messages with newlines into single string
	for (const auto& m : msgs)
	{
		msg += m;
		msg.push_back('\n');
	}
	// remove final newline if exists
	if (!msg.empty())
	{
		msg.pop_back();
	}

	return msg;
}

#endif
}