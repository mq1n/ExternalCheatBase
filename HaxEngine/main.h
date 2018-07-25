#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define NOMINMAX

#pragma warning(disable: 4091 4172 4789 4996)

#include <SDKDDKVer.h>

#include "Defines.h"

#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <random>
#include <algorithm>
#include <memory>
#include <vector>
#include <map>
#include <cstdlib>
#include <mutex>
#include <Sstream>
#include <Locale>
#include <unordered_map>
#include <assert.h>
#include <list>
#include <DbgHelp.h>
#include <WbemCli.h>
#include <WbemIdl.h>
#include <time.h>
#include <ctime>
#include <chrono>
#include <stdint.h>
#include <csignal>
#include <limits>
#include <iomanip>
#include <cmath>
#include <thread>

#include <TlHelp32.h>
#include <Psapi.h>
#include <io.h>
#include <sys/stat.h>
#include <sddl.h>
#include <Iphlpapi.h>
#include <DbgHelp.h>
#include "AccCtrl.h"
#include "Aclapi.h"
#include <assert.h>
#include <intrin.h>
#include <excpt.h>

#ifdef GetProp
	#undef GetProp
#endif

#define CUSTOM_LOG_FILENAME XOR("H4x.log")

#define IS_VALID_HANDLE(handle) (handle && handle != INVALID_HANDLE_VALUE)

#define STATUS_NO_MORE_ENTRIES			((NTSTATUS)0x8000001AL)
#define STATUS_RESOURCE_NAME_NOT_FOUND	((NTSTATUS)0xC000008BL)
#define STATUS_INFO_LENGTH_MISMATCH		((NTSTATUS)0xC0000004L)
#define STATUS_NOT_ALL_ASSIGNED			((NTSTATUS)0x00000106L)
#define STATUS_SUCCESS					((NTSTATUS)0x00000000L)
#define ACCESS_DENIED					((NTSTATUS)0xC0000022L)
#define NT_SUCCESS(Status)				(((NTSTATUS)(Status)) >= 0)

#define NtCurrentProcess				((HANDLE)(LONG_PTR)-1)
#define NtCurrentThread					((HANDLE)(LONG_PTR)-2)
