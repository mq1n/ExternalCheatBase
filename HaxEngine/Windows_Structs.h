#pragma once
#include <Windows.h>

namespace
{
	typedef LONG KPRIORITY;
	typedef LONG NTSTATUS;

	typedef struct _UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWSTR  Buffer;
	} UNICODE_STRING, *PUNICODE_STRING;

	typedef enum _THREADINFOCLASS
	{
		ThreadBasicInformation, // q: THREAD_BASIC_INFORMATION
		ThreadTimes, // q: KERNEL_USER_TIMES
		ThreadPriority, // s: KPRIORITY
		ThreadBasePriority, // s: LONG
		ThreadAffinityMask, // s: KAFFINITY
		ThreadImpersonationToken, // s: HANDLE
		ThreadDescriptorTableEntry,
		ThreadEnableAlignmentFaultFixup, // s: BOOLEAN
		ThreadEventPair,
		ThreadQuerySetWin32StartAddress, // q: PVOID
		ThreadZeroTlsCell, // 10
		ThreadPerformanceCount, // q: LARGE_INTEGER
		ThreadAmILastThread, // q: ULONG
		ThreadIdealProcessor, // s: ULONG
		ThreadPriorityBoost, // qs: ULONG
		ThreadSetTlsArrayAddress,
		ThreadIsIoPending, // q: ULONG
		ThreadHideFromDebugger, // s: void, BOOLEAN
		ThreadBreakOnTermination, // qs: ULONG
		ThreadSwitchLegacyState,
		ThreadIsTerminated, // 20, q: ULONG
		ThreadLastSystemCall, // q: THREAD_LAST_SYSCALL_INFORMATION
		ThreadIoPriority, // qs: ULONG
		ThreadCycleTime, // q: THREAD_CYCLE_TIME_INFORMATION
		ThreadPagePriority, // q: ULONG
		ThreadActualBasePriority,
		ThreadTebInformation, // q: THREAD_TEB_INFORMATION (requires THREAD_GET_CONTEXT + THREAD_SET_CONTEXT)
		ThreadCSwitchMon,
		ThreadCSwitchPmu,
		ThreadWow64Context, // q: WOW64_CONTEXT
		ThreadGroupInformation, // 30, q: GROUP_AFFINITY
		ThreadUmsInformation,
		ThreadCounterProfiling,
		ThreadIdealProcessorEx, // q: PROCESSOR_NUMBER
		ThreadCpuAccountingInformation, // since WIN8
		ThreadSuspendCount, // since WINBLUE
		MaxThreadInfoClass
	} THREADINFOCLASS;

	typedef enum _PROCESSINFOCLASS
	{
		ProcessBasicInformation, // 0, q: PROCESS_BASIC_INFORMATION, PROCESS_EXTENDED_BASIC_INFORMATION
		ProcessQuotaLimits, // qs: QUOTA_LIMITS, QUOTA_LIMITS_EX
		ProcessIoCounters, // q: IO_COUNTERS
		ProcessVmCounters, // q: VM_COUNTERS, VM_COUNTERS_EX
		ProcessTimes, // q: KERNEL_USER_TIMES
		ProcessBasePriority, // s: KPRIORITY
		ProcessRaisePriority, // s: ULONG
		ProcessDebugPort, // q: HANDLE
		ProcessExceptionPort, // s: HANDLE
		ProcessAccessToken, // s: PROCESS_ACCESS_TOKEN
		ProcessLdtInformation, // 10
		ProcessLdtSize,
		ProcessDefaultHardErrorMode, // qs: ULONG
		ProcessIoPortHandlers, // (kernel-mode only)
		ProcessPooledUsageAndLimits, // q: POOLED_USAGE_AND_LIMITS
		ProcessWorkingSetWatch, // q: PROCESS_WS_WATCH_INFORMATION[]; s: void
		ProcessUserModeIOPL,
		ProcessEnableAlignmentFaultFixup, // s: BOOLEAN
		ProcessPriorityClass, // qs: PROCESS_PRIORITY_CLASS
		ProcessWx86Information,
		ProcessHandleCount, // 20, q: ULONG, PROCESS_HANDLE_INFORMATION
		ProcessAffinityMask, // s: KAFFINITY
		ProcessPriorityBoost, // qs: ULONG
		ProcessDeviceMap, // qs: PROCESS_DEVICEMAP_INFORMATION, PROCESS_DEVICEMAP_INFORMATION_EX
		ProcessSessionInformation, // q: PROCESS_SESSION_INFORMATION
		ProcessForegroundInformation, // s: PROCESS_FOREGROUND_BACKGROUND
		ProcessWow64Information, // q: ULONG_PTR
		ProcessImageFileName, // q: UNICODE_STRING
		ProcessLUIDDeviceMapsEnabled, // q: ULONG
		ProcessBreakOnTermination, // qs: ULONG
		ProcessDebugObjectHandle, // 30, q: HANDLE
		ProcessDebugFlags, // qs: ULONG
		ProcessHandleTracing, // q: PROCESS_HANDLE_TRACING_QUERY; s: size 0 disables, otherwise enables
		ProcessIoPriority, // qs: ULONG
		ProcessExecuteFlags, // qs: ULONG
		ProcessResourceManagement,
		ProcessCookie, // q: ULONG
		ProcessImageInformation, // q: SECTION_IMAGE_INFORMATION
		ProcessCycleTime, // q: PROCESS_CYCLE_TIME_INFORMATION // since VISTA
		ProcessPagePriority, // q: ULONG
		ProcessInstrumentationCallback, // 40
		ProcessThreadStackAllocation, // s: PROCESS_STACK_ALLOCATION_INFORMATION, PROCESS_STACK_ALLOCATION_INFORMATION_EX
		ProcessWorkingSetWatchEx, // q: PROCESS_WS_WATCH_INFORMATION_EX[]
		ProcessImageFileNameWin32, // q: UNICODE_STRING
		ProcessImageFileMapping, // q: HANDLE (input)
		ProcessAffinityUpdateMode, // qs: PROCESS_AFFINITY_UPDATE_MODE
		ProcessMemoryAllocationMode, // qs: PROCESS_MEMORY_ALLOCATION_MODE
		ProcessGroupInformation, // q: USHORT[]
		ProcessTokenVirtualizationEnabled, // s: ULONG
		ProcessConsoleHostProcess, // q: ULONG_PTR
		ProcessWindowInformation, // 50, q: PROCESS_WINDOW_INFORMATION
		ProcessHandleInformation, // q: PROCESS_HANDLE_SNAPSHOT_INFORMATION // since WIN8
		ProcessMitigationPolicy, // s: PROCESS_MITIGATION_POLICY_INFORMATION
		ProcessDynamicFunctionTableInformation,
		ProcessHandleCheckingMode,
		ProcessKeepAliveCount, // q: PROCESS_KEEPALIVE_COUNT_INFORMATION
		ProcessRevokeFileHandles, // s: PROCESS_REVOKE_FILE_HANDLES_INFORMATION
		ProcessWorkingSetControl, // s: PROCESS_WORKING_SET_CONTROL
		ProcessHandleTable, // since WINBLUE
		ProcessCheckStackExtentsMode,
		ProcessCommandLineInformation, // 60, q: UNICODE_STRING
		ProcessProtectionInformation, // q: PS_PROTECTION
		MaxProcessInfoClass
	} PROCESSINFOCLASS;

	typedef enum _SYSTEM_INFORMATION_CLASS
	{
		SystemBasicInformation, // q: SYSTEM_BASIC_INFORMATION
		SystemProcessorInformation, // q: SYSTEM_PROCESSOR_INFORMATION
		SystemPerformanceInformation, // q: SYSTEM_PERFORMANCE_INFORMATION
		SystemTimeOfDayInformation, // q: SYSTEM_TIMEOFDAY_INFORMATION
		SystemPathInformation, // not implemented
		SystemProcessInformation, // q: SYSTEM_PROCESS_INFORMATION
		SystemCallCountInformation, // q: SYSTEM_CALL_COUNT_INFORMATION
		SystemDeviceInformation, // q: SYSTEM_DEVICE_INFORMATION
		SystemProcessorPerformanceInformation, // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION
		SystemFlagsInformation, // q: SYSTEM_FLAGS_INFORMATION
		SystemCallTimeInformation, // 10, not implemented
		SystemModuleInformation, // q: RTL_PROCESS_MODULES
		SystemLocksInformation,
		SystemStackTraceInformation,
		SystemPagedPoolInformation, // not implemented
		SystemNonPagedPoolInformation, // not implemented
		SystemHandleInformation, // q: SYSTEM_HANDLE_INFORMATION
		SystemObjectInformation, // q: SYSTEM_OBJECTTYPE_INFORMATION mixed with SYSTEM_OBJECT_INFORMATION
		SystemPageFileInformation, // q: SYSTEM_PAGEFILE_INFORMATION
		SystemVdmInstemulInformation, // q
		SystemVdmBopInformation, // 20, not implemented
		SystemFileCacheInformation, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemCache)
		SystemPoolTagInformation, // q: SYSTEM_POOLTAG_INFORMATION
		SystemInterruptInformation, // q: SYSTEM_INTERRUPT_INFORMATION
		SystemDpcBehaviorInformation, // q: SYSTEM_DPC_BEHAVIOR_INFORMATION; s: SYSTEM_DPC_BEHAVIOR_INFORMATION (requires SeLoadDriverPrivilege)
		SystemFullMemoryInformation, // not implemented
		SystemLoadGdiDriverInformation, // s (kernel-mode only)
		SystemUnloadGdiDriverInformation, // s (kernel-mode only)
		SystemTimeAdjustmentInformation, // q: SYSTEM_QUERY_TIME_ADJUST_INFORMATION; s: SYSTEM_SET_TIME_ADJUST_INFORMATION (requires SeSystemtimePrivilege)
		SystemSummaryMemoryInformation, // not implemented
		SystemMirrorMemoryInformation, // 30, s (requires license value "Kernel-MemoryMirroringSupported") (requires SeShutdownPrivilege)
		SystemPerformanceTraceInformation, // s
		SystemObsolete0, // not implemented
		SystemExceptionInformation, // q: SYSTEM_EXCEPTION_INFORMATION
		SystemCrashDumpStateInformation, // s (requires SeDebugPrivilege)
		SystemKernelDebuggerInformation, // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION
		SystemContextSwitchInformation, // q: SYSTEM_CONTEXT_SWITCH_INFORMATION
		SystemRegistryQuotaInformation, // q: SYSTEM_REGISTRY_QUOTA_INFORMATION; s (requires SeIncreaseQuotaPrivilege)
		SystemExtendServiceTableInformation, // s (requires SeLoadDriverPrivilege) // loads win32k only
		SystemPrioritySeperation, // s (requires SeTcbPrivilege)
		SystemVerifierAddDriverInformation, // 40, s (requires SeDebugPrivilege)
		SystemVerifierRemoveDriverInformation, // s (requires SeDebugPrivilege)
		SystemProcessorIdleInformation, // q: SYSTEM_PROCESSOR_IDLE_INFORMATION
		SystemLegacyDriverInformation, // q: SYSTEM_LEGACY_DRIVER_INFORMATION
		SystemCurrentTimeZoneInformation, // q
		SystemLookasideInformation, // q: SYSTEM_LOOKASIDE_INFORMATION
		SystemTimeSlipNotification, // s (requires SeSystemtimePrivilege)
		SystemSessionCreate, // not implemented
		SystemSessionDetach, // not implemented
		SystemSessionInformation, // not implemented
		SystemRangeStartInformation, // 50, q
		SystemVerifierInformation, // q: SYSTEM_VERIFIER_INFORMATION; s (requires SeDebugPrivilege)
		SystemVerifierThunkExtend, // s (kernel-mode only)
		SystemSessionProcessInformation, // q: SYSTEM_SESSION_PROCESS_INFORMATION
		SystemLoadGdiDriverInSystemSpace, // s (kernel-mode only) (same as SystemLoadGdiDriverInformation)
		SystemNumaProcessorMap, // q
		SystemPrefetcherInformation, // q: PREFETCHER_INFORMATION; s: PREFETCHER_INFORMATION // PfSnQueryPrefetcherInformation
		SystemExtendedProcessInformation, // q: SYSTEM_PROCESS_INFORMATION
		SystemRecommendedSharedDataAlignment, // q
		SystemComPlusPackage, // q; s
		SystemNumaAvailableMemory, // 60
		SystemProcessorPowerInformation, // q: SYSTEM_PROCESSOR_POWER_INFORMATION
		SystemEmulationBasicInformation, // q
		SystemEmulationProcessorInformation,
		SystemExtendedHandleInformation, // q: SYSTEM_HANDLE_INFORMATION_EX
		SystemLostDelayedWriteInformation, // q: ULONG
		SystemBigPoolInformation, // q: SYSTEM_BIGPOOL_INFORMATION
		SystemSessionPoolTagInformation, // q: SYSTEM_SESSION_POOLTAG_INFORMATION
		SystemSessionMappedViewInformation, // q: SYSTEM_SESSION_MAPPED_VIEW_INFORMATION
		SystemHotpatchInformation, // q; s
		SystemObjectSecurityMode, // 70, q
		SystemWatchdogTimerHandler, // s (kernel-mode only)
		SystemWatchdogTimerInformation, // q (kernel-mode only); s (kernel-mode only)
		SystemLogicalProcessorInformation, // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION
		SystemWow64SharedInformationObsolete, // not implemented
		SystemRegisterFirmwareTableInformationHandler, // s (kernel-mode only)
		SystemFirmwareTableInformation, // not implemented
		SystemModuleInformationEx, // q: RTL_PROCESS_MODULE_INFORMATION_EX
		SystemVerifierTriageInformation, // not implemented
		SystemSuperfetchInformation, // q: SUPERFETCH_INFORMATION; s: SUPERFETCH_INFORMATION // PfQuerySuperfetchInformation
		SystemMemoryListInformation, // 80, q: SYSTEM_MEMORY_LIST_INFORMATION; s: SYSTEM_MEMORY_LIST_COMMAND (requires SeProfileSingleProcessPrivilege)
		SystemFileCacheInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (same as SystemFileCacheInformation)
		SystemThreadPriorityClientIdInformation, // s: SYSTEM_THREAD_CID_PRIORITY_INFORMATION (requires SeIncreaseBasePriorityPrivilege)
		SystemProcessorIdleCycleTimeInformation, // q: SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION[]
		SystemVerifierCancellationInformation, // not implemented // name:wow64:whNT32QuerySystemVerifierCancellationInformation
		SystemProcessorPowerInformationEx, // not implemented
		SystemRefTraceInformation, // q; s // ObQueryRefTraceInformation
		SystemSpecialPoolInformation, // q; s (requires SeDebugPrivilege) // MmSpecialPoolTag, then MmSpecialPoolCatchOverruns != 0
		SystemProcessIdInformation, // q: SYSTEM_PROCESS_ID_INFORMATION
		SystemErrorPortInformation, // s (requires SeTcbPrivilege)
		SystemBootEnvironmentInformation, // 90, q: SYSTEM_BOOT_ENVIRONMENT_INFORMATION
		SystemHypervisorInformation, // q; s (kernel-mode only)
		SystemVerifierInformationEx, // q; s
		SystemTimeZoneInformation, // s (requires SeTimeZonePrivilege)
		SystemImageFileExecutionOptionsInformation, // s: SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION (requires SeTcbPrivilege)
		SystemCoverageInformation, // q; s // name:wow64:whNT32QuerySystemCoverageInformation; ExpCovQueryInformation
		SystemPrefetchPatchInformation, // not implemented
		SystemVerifierFaultsInformation, // s (requires SeDebugPrivilege)
		SystemSystemPartitionInformation, // q: SYSTEM_SYSTEM_PARTITION_INFORMATION
		SystemSystemDiskInformation, // q: SYSTEM_SYSTEM_DISK_INFORMATION
		SystemProcessorPerformanceDistribution, // 100, q: SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION
		SystemNumaProximityNodeInformation, // q
		SystemDynamicTimeZoneInformation, // q; s (requires SeTimeZonePrivilege)
		SystemCodeIntegrityInformation, // q // SeCodeIntegrityQueryInformation
		SystemProcessorMicrocodeUpdateInformation, // s
		SystemProcessorBrandString, // q // HaliQuerySystemInformation -> HalpGetProcessorBrandString, info class 23
		SystemVirtualAddressInformation, // q: SYSTEM_VA_LIST_INFORMATION[]; s: SYSTEM_VA_LIST_INFORMATION[] (requires SeIncreaseQuotaPrivilege) // MmQuerySystemVaInformation
		SystemLogicalProcessorAndGroupInformation, // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX // since WIN7 // KeQueryLogicalProcessorRelationship
		SystemProcessorCycleTimeInformation, // q: SYSTEM_PROCESSOR_CYCLE_TIME_INFORMATION[]
		SystemStoreInformation, // q; s // SmQueryStoreInformation
		SystemRegistryAppendString, // 110, s: SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS
		SystemAitSamplingValue, // s: ULONG (requires SeProfileSingleProcessPrivilege)
		SystemVhdBootInformation, // q: SYSTEM_VHD_BOOT_INFORMATION
		SystemCpuQuotaInformation, // q; s // PsQueryCpuQuotaInformation
		SystemNativeBasicInformation, // not implemented
		SystemSpare1, // not implemented
		SystemLowPriorityIoInformation, // q: SYSTEM_LOW_PRIORITY_IO_INFORMATION
		SystemTpmBootEntropyInformation, // q: TPM_BOOT_ENTROPY_NT_RESULT // ExQueryTpmBootEntropyInformation
		SystemVerifierCountersInformation, // q: SYSTEM_VERIFIER_COUNTERS_INFORMATION
		SystemPagedPoolInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypePagedPool)
		SystemSystemPtesInformationEx, // 120, q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemPtes)
		SystemNodeDistanceInformation, // q
		SystemAcpiAuditInformation, // q: SYSTEM_ACPI_AUDIT_INFORMATION // HaliQuerySystemInformation -> HalpAuditQueryResults, info class 26
		SystemBasicPerformanceInformation, // q: SYSTEM_BASIC_PERFORMANCE_INFORMATION // name:wow64:whNtQuerySystemInformation_SystemBasicPerformanceInformation
		SystemQueryPerformanceCounterInformation, // q: SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION // since WIN7 SP1
		SystemSessionBigPoolInformation, // since WIN8
		SystemBootGraphicsInformation,
		SystemScrubPhysicalMemoryInformation,
		SystemBadPageInformation,
		SystemProcessorProfileControlArea,
		SystemCombinePhysicalMemoryInformation, // 130
		SystemEntropyInterruptTimingCallback,
		SystemConsoleInformation,
		SystemPlatformBinaryInformation,
		SystemThrottleNotificationInformation,
		SystemHypervisorProcessorCountInformation,
		SystemDeviceDataInformation,
		SystemDeviceDataEnumerationInformation,
		SystemMemoryTopologyInformation,
		SystemMemoryChannelInformation,
		SystemBootLogoInformation, // 140
		SystemProcessorPerformanceInformationEx, // since WINBLUE
		SystemSpare0,
		SystemSecureBootPolicyInformation,
		SystemPageFileInformationEx,
		SystemSecureBootInformation,
		SystemEntropyInterruptTimingRawInformation,
		SystemPortableWorkspaceEfiLauncherInformation,
		SystemFullProcessInformation, // q: SYSTEM_PROCESS_INFORMATION with SYSTEM_PROCESS_INFORMATION_EXTENSION (requires admin)
		SystemKernelDebuggerInformationEx,
		SystemBootMetadataInformation, // 150
		SystemSoftRebootInformation,
		SystemElamCertificateInformation,
		SystemOfflineDumpConfigInformation,
		SystemProcessorFeaturesInformation,
		SystemRegistryReconciliationInformation,
		SystemEdidInformation,
		MaxSystemInfoClass
	} SYSTEM_INFORMATION_CLASS;

	typedef struct _SYSTEM_HANDLE
	{
		ULONG ProcessId;
		BYTE ObjectTypeNumber;
		BYTE Flags;
		USHORT Handle;
		PVOID Object;
		ACCESS_MASK GrantedAccess;
	} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

	typedef struct _SYSTEM_HANDLE_INFORMATION
	{
		ULONG HandleCount;
		SYSTEM_HANDLE Handles[1];
	} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

	typedef struct _PEB_LDR_DATA
	{
		ULONG           Length;
		BOOLEAN         Initialized;
		PVOID           SsHandle;
		LIST_ENTRY      InLoadOrderModuleList;
		LIST_ENTRY      InMemoryOrderModuleList;
		LIST_ENTRY      InInitializationOrderModuleList;
	} PEB_LDR_DATA, *PPEB_LDR_DATA;

	typedef struct _RTL_USER_PROCESS_PARAMETERS {
		BYTE           Reserved1[16];
		PVOID          Reserved2[10];
	} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

	typedef struct _PEB_FREE_BLOCK {
		_PEB_FREE_BLOCK          *Next;
		ULONG                   Size;
	} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;

	typedef void(*PPEBLOCKROUTINE)(
		PVOID PebLock
		);

	typedef struct _PEB {
		BOOLEAN                 InheritedAddressSpace;
		BOOLEAN                 ReadImageFileExecOptions;
		BOOLEAN                 BeingDebugged;
		BOOLEAN                 Spare;
		HANDLE                  Mutant;
		PVOID                   ImageBaseAddress;
		PPEB_LDR_DATA           LoaderData;
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
		PVOID                   SubSystemData;
		PVOID                   ProcessHeap;
		PVOID                   FastPebLock;
		PPEBLOCKROUTINE         FastPebLockRoutine;
		PPEBLOCKROUTINE         FastPebUnlockRoutine;
		ULONG                   EnvironmentUpdateCount;
		PVOID*                  KernelCallbackTable;
		PVOID                   EventLogSection;
		PVOID                   EventLog;
		PPEB_FREE_BLOCK         FreeList;
		ULONG                   TlsExpansionCounter;
		PVOID                   TlsBitmap;
		ULONG                   TlsBitmapBits[0x2];
		PVOID                   ReadOnlySharedMemoryBase;
		PVOID                   ReadOnlySharedMemoryHeap;
		PVOID*                  ReadOnlyStaticServerData;
		PVOID                   AnsiCodePageData;
		PVOID                   OemCodePageData;
		PVOID                   UnicodeCaseTableData;
		ULONG                   NumberOfProcessors;
		ULONG                   NtGlobalFlag;
		BYTE                    Spare2[0x4];
		LARGE_INTEGER           CriticalSectionTimeout;
		ULONG                   HeapSegmentReserve;
		ULONG                   HeapSegmentCommit;
		ULONG                   HeapDeCommitTotalFreeThreshold;
		ULONG                   HeapDeCommitFreeBlockThreshold;
		ULONG                   NumberOfHeaps;
		ULONG                   MaximumNumberOfHeaps;
		PVOID*                  *ProcessHeaps;
		PVOID                   GdiSharedHandleTable;
		PVOID                   ProcessStarterHelper;
		PVOID                   GdiDCAttributeList;
		PVOID                   LoaderLock;
		ULONG                   OSMajorVersion;
		ULONG                   OSMinorVersion;
		ULONG                   OSBuildNumber;
		ULONG                   OSPlatformId;
		ULONG                   ImageSubSystem;
		ULONG                   ImageSubSystemMajorVersion;
		ULONG                   ImageSubSystemMinorVersion;
		ULONG                   GdiHandleBuffer[0x22];
		ULONG                   PostProcessInitRoutine;
		ULONG                   TlsExpansionBitmap;
		BYTE                    TlsExpansionBitmapBits[0x80];
		ULONG                   SessionId;
	} PEB, *PPEB;

	typedef struct _PROCESS_BASIC_INFORMATION
	{
		NTSTATUS ExitStatus;
		PPEB PebBaseAddress;
		ULONG_PTR AffinityMask;
		KPRIORITY BasePriority;
		HANDLE UniqueProcessId;
		HANDLE InheritedFromUniqueProcessId;
	} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

	typedef struct BASE_RELOCATION_BLOCK {
		DWORD PageAddress;
		DWORD BlockSize;
	} BASE_RELOCATION_BLOCK, *PBASE_RELOCATION_BLOCK;

	typedef struct BASE_RELOCATION_ENTRY {
		USHORT Offset : 12;
		USHORT Type : 4;
	} BASE_RELOCATION_ENTRY, *PBASE_RELOCATION_ENTRY;

	typedef struct _OBJECT_ATTRIBUTES {
		ULONG           Length;
		HANDLE          RootDirectory;
		PUNICODE_STRING ObjectName;
		ULONG           Attributes;
		PVOID           SecurityDescriptor;
		PVOID           SecurityQualityOfService;
	}  OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

	typedef enum _MEMORY_INFORMATION_CLASS
	{
		MemoryBasicInformation,		
		MemoryWorkingSetInformation,
		MemoryMappedFilenameInformation,
		MemoryRegionInformation,		
		MemoryWorkingSetExInformation,	
		MemorySharedCommitInformation	
	} MEMORY_INFORMATION_CLASS;

	typedef struct _LDR_DATA_TABLE_ENTRY {
		LIST_ENTRY InLoadOrderLinks;
		LIST_ENTRY InMemoryOrderLinks;
		LIST_ENTRY InInitializationOrderLinks;
		PVOID DllBase;
		PVOID EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		ULONG Flags;
		USHORT LoadCount;
		USHORT TlsIndex;
		union {
			LIST_ENTRY HashLinks;
			struct {
				PVOID SectionPointer;
				ULONG CheckSum;
			};
		};
		union {
			struct {
				ULONG TimeDateStamp;
			};
			struct {
				PVOID LoadedImports;
			};
		};
		struct _ACTIVATION_CONTEXT * EntryPointActivationContext;

		PVOID PatchInformation;

	} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

	typedef enum _DEBUGOBJECTINFOCLASS {
		DebugObjectFlags = 1,
		MaxDebugObjectInfoClass
	} DEBUGOBJECTINFOCLASS, *PDEBUGOBJECTINFOCLASS;

};

namespace MitigationStructs
{
	typedef struct _PROCESS_MITIGATION_ASLR_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD EnableBottomUpRandomization : 1;
				DWORD EnableForceRelocateImages : 1;
				DWORD EnableHighEntropy : 1;
				DWORD DisallowStrippedImages : 1;
				DWORD ReservedFlags : 28;
			};
		};
	} PROCESS_MITIGATION_ASLR_POLICY, *PPROCESS_MITIGATION_ASLR_POLICY;
	typedef struct _PROCESS_MITIGATION_DEP_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD Enable : 1;
				DWORD DisableAtlThunkEmulation : 1;
				DWORD ReservedFlags : 30;
			};
		};
		BOOLEAN Permanent;
	} PROCESS_MITIGATION_DEP_POLICY, *PPROCESS_MITIGATION_DEP_POLICY;

	typedef struct _PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD RaiseExceptionOnInvalidHandleReference : 1;
				DWORD HandleExceptionsPermanentlyEnabled : 1;
				DWORD ReservedFlags : 30;
			};
		};
	} PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY, *PPROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY;
	typedef struct _PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD DisallowWin32kSystemCalls : 1;
				DWORD ReservedFlags : 31;
			};
		};
	} PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY, *PPROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY;
	typedef struct _PROCESS_MITIGATION_DYNAMIC_CODE_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD ProhibitDynamicCode : 1;
				DWORD ReservedFlags : 31;
			};
		};
	} PROCESS_MITIGATION_DYNAMIC_CODE_POLICY, *PPROCESS_MITIGATION_DYNAMIC_CODE_POLICY;
	typedef struct _PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD MicrosoftSignedOnly : 1;
				DWORD ReservedFlags : 31;
			};
		};
	} PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY, *PPROCESS_MITIGATION_BINARY_SIGNATURE_POLICY;
	typedef struct _PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY {
		union {
			DWORD  Flags;
			struct {
				DWORD EnableControlFlowGuard : 1;
				DWORD ReservedFlags : 31;
			};
		};
	} PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY, *PPROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY;
	typedef struct _PROCESS_MITIGATION_IMAGE_LOAD_POLICY {
		union {
			DWORD  Flags;
			struct {
				DWORD NoRemoteImages : 1;
				DWORD NoLowMandatoryLabelImages : 1;
				DWORD PreferSystem32Images : 1;
				DWORD ReservedFlags : 29;
			};
		};
	} PROCESS_MITIGATION_IMAGE_LOAD_POLICY, *PPROCESS_MITIGATION_IMAGE_LOAD_POLICY;
	typedef struct _PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY {
		union {
			DWORD  Flags;
			struct {
				DWORD DisableExtensionPoints : 1;
				DWORD ReservedFlags : 31;
			};
		};
	} PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY, *PPROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY;


	typedef enum _PROCESS_MITIGATION_POLICY {
		ProcessDEPPolicy,
		ProcessASLRPolicy,
		ProcessDynamicCodePolicy,
		ProcessStrictHandleCheckPolicy,
		ProcessSystemCallDisablePolicy,
		ProcessMitigationOptionsMask,
		ProcessExtensionPointDisablePolicy,
		ProcessControlFlowGuardPolicy,
		ProcessSignaturePolicy,
		ProcessFontDisablePolicy,
		ProcessImageLoadPolicy,
		MaxProcessMitigationPolicy
	} PROCESS_MITIGATION_POLICY, *PPROCESS_MITIGATION_POLICY;

};

