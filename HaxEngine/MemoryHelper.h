#pragma once

class CMemoryHelper
{
	public:
		CMemoryHelper(HANDLE hProcess);
		~CMemoryHelper(void) = default;

		LPVOID	Alloc(std::size_t size, DWORD dwProtection = PAGE_EXECUTE_READWRITE, LPVOID pPreferredLoc = nullptr);
		LPVOID	Commit(LPVOID data, std::size_t size);
		bool	Protect(LPVOID address, std::size_t size, DWORD dwProtectFlag);
		bool	Free(LPVOID address, std::size_t size);

		bool	ReadAddress(LPVOID address, LPVOID buffer, std::size_t size);
		bool	ReadAddressSafe(LPVOID address, LPVOID buffer, std::size_t size);
		bool	WriteAddress(LPVOID address, LPVOID buffer, std::size_t size);
		bool	WriteAddressSafe(LPVOID address, LPVOID buffer, std::size_t size);

		bool	Patch(LPVOID address, LPVOID buffer, std::size_t size);

		bool	IsSafePage(LPVOID address);

		HMODULE GetModule(const char* c_szModule);

		HANDLE	CreateThread(LPTHREAD_START_ROUTINE lpThreadRoutine, LPVOID lpParam, DWORD dwFlag = 0);
		bool	LoadShellCode(LPVOID lpShellCode, std::size_t uiSize, LPVOID lpParam, std::size_t uiParamSize, DWORD dwTimeoutInterval = INFINITE);
		bool	LoadDLL(const std::string & szPath);

	public:
		template <typename T>
		T Read(const DWORD_PTR Address, std::vector<DWORD_PTR> && offsetList)
		{
			T ret;
			ReadProcessMemory(m_hProcess, (LPCVOID)Address, &ret, sizeof(T), nullptr);

			for (const auto & i : offsetList)
				ReadProcessMemory(m_hProcess, LPCVOID((DWORD)ret + i), &ret, sizeof(T), nullptr);

			return ret;
		}
		template <typename T>
		T Read(const DWORD_PTR Address)
		{
			T ret;
			ReadProcessMemory(m_hProcess, (LPCVOID)Address, &ret, sizeof(T), nullptr);
			return ret;
		}
		template <typename T>
		T* ReadStruct(const DWORD_PTR Address)
		{
			T ret;
			if (!ReadProcessMemory(m_hProcess, (LPCVOID)Address, &ret, sizeof(T), nullptr))
				return nullptr;
			return &ret;
		}
		template <typename T>
		bool ReadStruct(const DWORD_PTR Address, T& data)
		{
			return !!ReadProcessMemory(m_hProcess, (LPCVOID)Address, &data, sizeof(T), nullptr);
		}
		template <typename T>
		auto ReadArray(const DWORD_PTR Address, size_t Length)
		{
			std::shared_ptr<T[]> buffer(new T[Length]);
			ReadProcessMemory(m_hProcess, (LPCVOID)Address, buffer.get(), Length, nullptr);
			return buffer;
		}

		template <class cData>
		bool Write(const DWORD_PTR dwAddress, cData Value)
		{
			SIZE_T dwWrittenByteCount;
			auto bWriteRet = WriteProcessMemory(m_hProcess, (LPVOID)dwAddress, &Value, sizeof(cData), &dwWrittenByteCount);
			return bWriteRet;
		}

	private:
		HANDLE m_hProcess;
};

