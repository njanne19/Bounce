// adapter.hpp
#pragma once 
#include <Windows.h> 
#include "wintun.h" 

namespace Adapter {


	// Functions to load the Wintun library and resolve functions. 
	// Returns the HMODULE of the loaded DLL, or NULL on failure. 
	HMODULE LoadWintunLibrary(); 

	// Function to create and start a Wintgun adapter session. 
	// Returns true if successful
	bool InitializeAdapterSession(const WCHAR* adapterName, WINTUN_ADAPTER_HANDLE& adapter, WINTUN_SESSION_HANDLE& session); 

	// Function to clean up and close the adapter session. 
	void CleanupAdapterSession(WINTUN_ADAPTER_HANDLE adapter, WINTUN_SESSION_HANDLE session);

	// Function to run a simple packet processing loop for demonstration 
	void RunPacketProcessingLoop(WINTUN_SESSION_HANDLE session, DWORD durationMilliseconds); 
}