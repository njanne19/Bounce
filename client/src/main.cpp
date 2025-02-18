#define WIN32_LEAN_AND_MEAN
#include <iostream> 
#include "adapter.hpp"


int main() {
	std::cout << "Hello from Bounce!" << std::endl; 
	
    // Load Wintun library and resolve functions.
    HMODULE hWintun = Adapter::LoadWintunLibrary();
    if (!hWintun) {
        return 1;
    }

    // Create an adapter session.
    WINTUN_SESSION_HANDLE session = nullptr;
    WINTUN_ADAPTER_HANDLE adapter = nullptr; 
    if (!Adapter::InitializeAdapterSession(L"BounceVPNAdapter", adapter, session)) {
        return 1;
    }


    // Run the packet processing loop for 5 seconds.
    Adapter::RunPacketProcessingLoop(session, 5000);

    // Clean up the session.
    Adapter::CleanupAdapterSession(adapter, session);

    return 0;
}