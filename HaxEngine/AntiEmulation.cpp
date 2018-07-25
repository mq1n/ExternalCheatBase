#include "HaxEngine.h"
#include "main.h"
#include "Antis.h"
#include "Defines.h"
#include "DynamicWinAPI.h"

inline std::string randomStrGen(int iLength)
{
	CHAR __alphabet[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0x0 }; // abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890
	static std::string charset = __alphabet;
	std::string result;
	result.resize(iLength);

	for (int i = 0; i < iLength; i++)
		result[i] = charset[rand() % charset.length()];

	return result;
}

void CAntis::CheckRandomApiExport()
{
	auto uTime = (unsigned int)time(NULL);
	srand(uTime);
	std::string szFakeFunctionName = randomStrGen(7);

	if (GetProcAddress(WinModuleTable->hKernel32, szFakeFunctionName.c_str())) {
		DEBUG_LOG(LL_CRI, "Analysis tool triggered on CheckRandomApiExport!");
		abort();
	}
}



bool CAntis::CheckStdChronoTimeRange()
{
	auto tStart = std::chrono::high_resolution_clock::now();

	Sleep(1000);

	__try {
		int* p = 0; // access violation
		*p = 0;
	}
	__except (1) { }

	Sleep(1000);

	auto tDiff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - tStart).count();
	if (tDiff > 3000) 
		return true;
	
	return false;
}



void CAntis::AntiCuckoo()
{
	LPDWORD pOld, pFake;

	pFake = (LPDWORD)malloc(4096 * 100);
	memset(pFake, 1, 4096 * 100);
	__asm
	{
		mov eax, fs:[0x44]		// save old value
		mov pOld, eax

		mov eax, pFake			// replace with fake value
		mov fs : [0x44], eax
	}

	// this will not be logged nor executed.
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Sleep, (LPVOID)1000, 0, NULL);

	__asm
	{
		mov eax, pOld		// restore old value, not reached if cuckoo
		mov fs : [0x44], eax
	}

	free(pFake);
}
