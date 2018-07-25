#pragma once

class CAntis
{
	public:
		static void SetFakeImageSize();
		static void CrashDebugger();
		static bool DetachFromDebuggerProcess();
		static bool CheckSeDebugPriv();
		static bool CheckCloseProtectedHandle();

		static void CheckRandomApiExport();
		static bool CheckStdChronoTimeRange();

		static bool InitExceptionHandlers();
		static bool InitializeBlockHandles();

		static void AntiCuckoo();
		static void AntiSandboxie();
		static void AntiVirtualMachine();
		static void AntiSunbeltSandBox();
		static void AntiWine();
		static void Anticuckoomon();
		static void AntiSandbox();
};
