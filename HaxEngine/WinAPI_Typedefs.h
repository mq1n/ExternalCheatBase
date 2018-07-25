#pragma once
#include "Windows_Structs.h"

typedef BOOL(WINAPI* lpIsDebuggerPresent)(void);
typedef VOID(WINAPI* lpSleep)(_In_ DWORD dwMilliseconds);
typedef NTSTATUS(NTAPI* lpNtGetNextThread)(HANDLE ProcessHandle, HANDLE ThreadHandle, ACCESS_MASK DesiredAccess, ULONG HandleAttributes, ULONG Flags, PHANDLE NewThreadHandle);
typedef NTSTATUS(NTAPI* lpNtGetNextProcess)(HANDLE ProcessHandle, ACCESS_MASK DesiredAccess, ULONG HandleAttributes, ULONG Flags, PHANDLE NewProcessHandle);
typedef NTSTATUS(NTAPI* lpNtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);
typedef NTSTATUS(NTAPI* lpNtDuplicateObject)(HANDLE SourceProcessHandle, HANDLE SourceHandle, HANDLE TargetProcessHandle, PHANDLE TargetHandle, ACCESS_MASK DesiredAccess, ULONG Attributes, ULONG Options);
typedef NTSTATUS(NTAPI* lpNtClose)(HANDLE);
typedef NTSTATUS(NTAPI* lpNtGetContextThread)(IN HANDLE ThreadHandle, OUT PCONTEXT pContext);
typedef NTSTATUS(NTAPI* lpNtSetContextThread)(IN HANDLE ThreadHandle, IN PCONTEXT ThreadContext);
typedef NTSTATUS(NTAPI* lpNtReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesReaded);
typedef NTSTATUS(NTAPI* lpNtWaitForSingleObject)(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout);
typedef LONG(WINAPI* lpNtUnmapViewOfSection)(HANDLE ProcessHandle, PVOID BaseAddress);
typedef NTSTATUS(NTAPI* lpNtWriteVirtualMemory)(HANDLE, PVOID, CONST VOID *, SIZE_T, PSIZE_T);
typedef NTSTATUS(NTAPI* lpNtResumeThread)(IN HANDLE ThreadHandle, OUT PULONG SuspendCount OPTIONAL);
typedef NTSTATUS(NTAPI* lpNtTerminateProcess)(HANDLE ProcessHandle, NTSTATUS ExitStatus);
typedef NTSTATUS(NTAPI* lpRtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
typedef NTSTATUS(WINAPI* lpNtQueryInformationProcess)(HANDLE a, PROCESSINFOCLASS b, PVOID c, ULONG d, PULONG e);
typedef LONG(WINAPI* lpZwSetInformationThread)(HANDLE, DWORD, PVOID, ULONG);
typedef NTSTATUS(NTAPI* lpNtQueryVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID Buffer, ULONG Length, PULONG ResultLength);
typedef NTSTATUS(NTAPI* lpNtAdjustPrivilegesToken)(HANDLE TokenHandle, BOOLEAN DisableAllPrivileges, PTOKEN_PRIVILEGES NewState, ULONG BufferLength, PTOKEN_PRIVILEGES PreviousState, PULONG ReturnLength);
typedef NTSTATUS(NTAPI* lpNtOpenProcessToken)(HANDLE ProcessHandle, ACCESS_MASK DesiredAccess, PHANDLE TokenHandle);
typedef NTSTATUS(NTAPI* lpRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
typedef BOOL(WINAPI* lpSetProcessMitigationPolicy)(MitigationStructs::PROCESS_MITIGATION_POLICY MitigationPolicy, PVOID lpBuffer, SIZE_T dwLength);
typedef DWORD(NTAPI* lpCsrGetProcessId)();
typedef NTSTATUS(NTAPI* lpNtSetInformationDebugObject)(HANDLE DebugHandle, DEBUGOBJECTINFOCLASS DebugObjectInformationClass, PVOID DebugInformation, ULONG DebugInformationLength, PULONG ReturnLength);
typedef NTSTATUS(NTAPI* lpNtRemoveProcessDebug)(HANDLE ProcessHandle, HANDLE DebugHandle);
typedef NTSTATUS(NTAPI* lpNtSuspendProcess)(IN HANDLE ProcessHandle);
typedef NTSTATUS(WINAPI* lpNtResumeProcess)(HANDLE ProcessHandle);
typedef NTSTATUS(NTAPI* lpLdrLoadDll)(PWCHAR PathToFile, ULONG *Flags, UNICODE_STRING *ModuleFileName, HMODULE *ModuleHandle);
typedef VOID(NTAPI* lpRtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);
typedef NTSTATUS(NTAPI* lpNtCreateThreadEx)(PHANDLE hThread, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ProcessHandle, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, ULONG CreateFlags, ULONG_PTR StackZeroBits, SIZE_T SizeOfStackCommit, SIZE_T SizeOfStackReserve, LPVOID AttributeList);

