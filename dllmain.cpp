// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <iostream> //cout


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//DisableThreadLibraryCalls(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		//std::cout<<  "Do thread-specific detach..." <<std::endl;
		break;
	case DLL_PROCESS_DETACH:
		//TheServiceParticipant->shutdown();
		std::cout << "Do process-specific detach..." << std::endl;
		break;
	}
	return TRUE;
}

