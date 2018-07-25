#pragma once
#include "SafeHandle.h"
#include "ModuleHelper.h"
#include "HijackProcessHandle.h"
#include "MemoryHelper.h"

enum EOPenTypes
{
	OPEN_STANDART, /* OpenProcess API */
	OPEN_FORCE,    /* Enum all handles via NtGetNextProcess */
	OPEN_DUPLICATE_HANDLE, /* Duplicate a already created handle */
	OPEN_ELEVATED_HANDLE, /* TODO */ /* cpuz, dkom */ /* https://www.unknowncheats.me/forum/anti-cheat-bypass/216970-handlemaster-elevating-handle-access-cpu.html */
													 /* https://www.unknowncheats.me/forum/anti-cheat-bypass/228821-elevatehandle-elevate-handles-bypass-battleye.html */
	OPEN_DUPLICATE_WITH_CREATEPROCESS	 /* TODO */	 /* https://www.unknowncheats.me/forum/anti-cheat-bypass/239429-hbastard-bypass-1-line-user-mode-multi-anti-cheat-bypass-exploiting-inheritance.html */
};

enum EKeyTypes
{
	KEY_NONE,
	KEY_UP,
	KEY_DOWN,
	KEY_BOTH
};

class CProcessHelper : public std::enable_shared_from_this<CProcessHelper>
{
	public:
		CProcessHelper();
		CProcessHelper(DWORD dwProcessId);
		CProcessHelper(const char* c_szProcessName);
		CProcessHelper(const char* c_szClassName, const char* c_szTitle);
		CProcessHelper(const char* c_szPath, bool bSuspended);

		~CProcessHelper();

	public:
		void SetProcessId(DWORD dwProcessId);
		void SetProcessName(const char* c_szProcessName);
		void SetWindowInformations(const char* c_szClassName, const char* c_szTitle);
		void SetProcessPath(const char* c_szProcessPath, bool bSuspended);
		void SetOpenType(EOPenTypes openType);
		bool OpenTarget();

		void CreateModuleList();

		const CModuleHelper GetModule(const char* c_szModuleName) const;
		std::size_t ModuleCount() const;

		bool CheckCSGOModulesIsLoaded() const;
		void WaitLoadCSGOModules(DWORD dwDelay);

		DWORD GetBaseAddress() const;
		DWORD GetBaseSize() const;

		HANDLE GetHandle() const;
		DWORD GetProcessId() const;
		const char * GetName() const;
		std::string GetPath() const;
		std::vector<HWND> GetWindows() const;
		HWND GetWindow() const;
		HWND GetWindow(const std::string & szTargetClassname) const;

		bool IsFocused() const;
		bool IsRunning() const;

		void Close(); // class func
		bool Terminate() const;
		bool ProtectHandle() const;
		void SendKey(HWND hWnd, DWORD dwKey, DWORD dwDelay, EKeyTypes type = KEY_BOTH);

	protected:
		bool OpenWithProcessId();
		bool OpenWithProcessName();
		bool OpenWithWindowInformations();
		bool OpenWithCreateProcess(bool bSuspended);

	private:
		std::unique_ptr <CSafeProcessHandle>	m_cSafeProcessHandleHelper;
		std::unique_ptr <CHijackProcessHandle>	m_cHijackProcessHandleHelper;

		DWORD						m_dwProcessId;
		HANDLE						m_hProcessHandle;

		std::string					m_szProcessName;
		std::string					m_szClassName;
		std::string					m_szTitle;
		std::string					m_szProcessPath;

		EOPenTypes					m_openType;
		bool						m_bSuspended;
		bool						m_bIsSonicHandle;

		std::list <CModuleHelper>	m_listModules;
};

