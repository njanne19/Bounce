// adapter.cpp
#define WIN32_LEAN_AND_MEAN
#include "adapter.hpp" 
#include <iostream> 
#include <cstring> 
#include <Windows.h> 

// These types must match the signatures defined in wintun.h 
typedef WINTUN_ADAPTER_HANDLE(WINAPI* PFN_WintunCreateAdapter)(const WCHAR*, const WCHAR*, const GUID*);
typedef WINTUN_SESSION_HANDLE(WINAPI* PFN_WintunStartSession)(WINTUN_ADAPTER_HANDLE, DWORD);
typedef void (WINAPI* PFN_WintunEndSession)(WINTUN_SESSION_HANDLE);
typedef void (WINAPI* PFN_WintunCloseAdapter)(WINTUN_ADAPTER_HANDLE);


// New functions for packet I/O: 
typedef BYTE* (WINAPI* PFN_WintunReceivePacket)(WINTUN_SESSION_HANDLE, DWORD*);
typedef void (WINAPI* PFN_WintunReleaseReceivePacket)(WINTUN_SESSION_HANDLE, const BYTE*);
typedef BYTE* (WINAPI* PFN_WintunAllocateSendPacket)(WINTUN_SESSION_HANDLE, DWORD);
typedef void (WINAPI* PFN_WintunSendPacket)(WINTUN_SESSION_HANDLE, const BYTE*);


// Declare global function pointers
static PFN_WintunCreateAdapter pWintunCreateAdapter = nullptr;
static PFN_WintunStartSession	pWintunStartSession = nullptr;
static PFN_WintunEndSession	pWintunEndSession = nullptr;
static PFN_WintunCloseAdapter  pWintunCloseAdapter = nullptr;

static PFN_WintunReceivePacket        pWintunReceivePacket = nullptr;
static PFN_WintunReleaseReceivePacket pWintunReleaseReceivePacket = nullptr;
static PFN_WintunAllocateSendPacket   pWintunAllocateSendPacket = nullptr;
static PFN_WintunSendPacket           pWintunSendPacket = nullptr;


namespace Adapter {

	// Function to dynamically load the DLL and resolve functions 
	HMODULE LoadWintunLibrary() {
		// Load the DLL into the process. The L prefix indicates a wide-character string 
		HMODULE hWintun = LoadLibraryExW(L"wintun.dll", nullptr, 0);
		if (!hWintun) {
			std::wcerr << L"Failed to load wintun.dll. Error: " << GetLastError() << std::endl;
			return false;
		}

		// Use GetProcAddress to retrieve the address of each function, 
		// and cast the returned FARPROC (generic function pointer) to our specific type. 
		pWintunCreateAdapter = reinterpret_cast<PFN_WintunCreateAdapter>(
			GetProcAddress(hWintun, "WintunCreateAdapter"));
		pWintunStartSession = reinterpret_cast<PFN_WintunStartSession>(
			GetProcAddress(hWintun, "WintunStartSession"));
		pWintunEndSession = reinterpret_cast<PFN_WintunEndSession>(
			GetProcAddress(hWintun, "WintunEndSession"));
		pWintunCloseAdapter = reinterpret_cast<PFN_WintunCloseAdapter>(
			GetProcAddress(hWintun, "WintunCloseAdapter"));

		pWintunReceivePacket = reinterpret_cast<PFN_WintunReceivePacket>(
			GetProcAddress(hWintun, "WintunReceivePacket"));
		pWintunReleaseReceivePacket = reinterpret_cast<PFN_WintunReleaseReceivePacket>(
			GetProcAddress(hWintun, "WintunReleaseReceivePacket"));
		pWintunAllocateSendPacket = reinterpret_cast<PFN_WintunAllocateSendPacket>(
			GetProcAddress(hWintun, "WintunAllocateSendPacket"));
		pWintunSendPacket = reinterpret_cast<PFN_WintunSendPacket>(
			GetProcAddress(hWintun, "WintunSendPacket"));

		// Check that all required functions were successfully loaded. 
		if (!pWintunCreateAdapter || !pWintunStartSession || !pWintunEndSession || !pWintunCloseAdapter ||
			!pWintunReceivePacket || !pWintunReleaseReceivePacket ||
			!pWintunAllocateSendPacket || !pWintunSendPacket)
		{
			DWORD LastError = GetLastError();
			FreeLibrary(hWintun);
			SetLastError(LastError);
			std::wcerr << L"Failed to resolve one or more functions from wintun.dll. Error code: " << LastError << std::endl;
			return NULL;
		}
		return hWintun;
	}


	bool InitializeAdapterSession(const WCHAR* adapterName, WINTUN_ADAPTER_HANDLE& adapter, WINTUN_SESSION_HANDLE& session) {
		// Create a new adapter. here, tunnel type is "Wintun" and no specific GUID is provided
		adapter = pWintunCreateAdapter(adapterName, L"Wintun", nullptr);
		if (!adapter) {
			std::wcerr << L"Failed to create Wintun adapter. Error: " << GetLastError() << std::endl;
			return false;
		}
		std::wcout << L"Wintun adapter created successfully." << std::endl;

		session = pWintunStartSession(adapter, 4 * 1024 * 1024); // TODO: Looking into this value
		if (!session) {
			std::wcerr << L"Failed to start Wintun session. Error: " << GetLastError() << std::endl;
			pWintunCloseAdapter(adapter);
			return false;
		}
		std::wcout << L"Wintun session started succesfully." << std::endl;
		return true;
	}

	void CleanupAdapterSession(WINTUN_ADAPTER_HANDLE adapter, WINTUN_SESSION_HANDLE session) {
		pWintunEndSession(session);
		pWintunCloseAdapter(adapter); // Assuming session uniquely identifies the adapter.
		std::wcout << L"Wintun session ended and adapter closed." << std::endl;
	}

	void RunPacketProcessingLoop(WINTUN_SESSION_HANDLE session, DWORD durationMilliseconds) {
		DWORD startTime = GetTickCount();
		while (GetTickCount() - startTime < durationMilliseconds) {
			DWORD packetSize = 0;
			BYTE* packet = pWintunReceivePacket(session, &packetSize);
			if (packet) {
				// For demonstration, print packet information.
				uint8_t version = packet[0] >> 4;
				if (version == 4)
					std::wcout << L"Received IPv4 Packet of size: " << packetSize << std::endl;
				else if (version == 6)
					std::wcout << L"Received IPv6 Packet of size: " << packetSize << std::endl;
				else
					std::wcout << L"Received unknown packet of size: " << packetSize << std::endl;

				// Optionally, echo the packet or process it further.
				pWintunReleaseReceivePacket(session, packet);
			}
			else {
				Sleep(10);
			}
		}
	}
}
