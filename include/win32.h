#ifndef Atlas_Win32_H
#define Atlas_Win32_H

#include <syscalls.h>

#if _WIN64
    #define PPEB_PTR __readgsqword(0x60)
#else
    #define PPEB_PTR __readfsword(0x30)
#endif

typedef struct BASE_RELOCATION_ENTRY {
	USHORT Offset : 12;
	USHORT Type : 4;
} BASE_RELOCATION_ENTRY, * PBASE_RELOCATION_ENTRY;

typedef struct C_PEB_LDR_DATA{
    ULONG Length;
    BOOLEAN Initialized;
    HANDLE SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID EntryInProgress;
    BOOLEAN ShutdownInProgress;
    HANDLE ShutdownThreadId;
} C_PEB_LDR_DATA, *C_PPEB_LDR_DATA;

typedef struct C_LDR_DATA_TABLE_ENTRY{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID entry_point;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    WORD LoadCount;
    WORD TlsIndex;
    union
    {
        LIST_ENTRY HashLinks;
        struct
        {
            PVOID SectionPointer;
            ULONG CheckSum;
        };
    };
    union
    {
        ULONG TimeDateStamp;
        PVOID LoadedImports;
    };
    _ACTIVATION_CONTEXT * entry_pointActivationContext;
    PVOID PatchInformation;
    LIST_ENTRY ForwarderLinks;
    LIST_ENTRY ServiceTagLinks;
    LIST_ENTRY StaticLinks;
} C_LDR_DATA_TABLE_ENTRY, *C_PLDR_DATA_TABLE_ENTRY;

typedef NTSTATUS(NTAPI* fnLdrLoadDll)(
    PWCHAR PathToFile,
    ULONG Flags,
    PUNICODE_STRING ModuleFileName,
    HANDLE* ModuleHandle
);

typedef NTSTATUS(NTAPI* fnNtWriteVirtualMemory)(
    HANDLE ProcessHandle,
    PVOID BaseAddress,
    PVOID Buffer,
    ULONG NumberOfBytesToWrite,
    PULONG  NumberOfBytesWritten 
);

struct ATLAS_UTILS {
    fnLdrLoadDll pLdrLoadDll;
    fnNtWriteVirtualMemory pNtWriteVirtualMemory;
    ATLAS_SYSCALLS atlas_syscalls;
};


//Main functions
DWORD FindTargetPid(LPCWSTR target, ATLAS_UTILS* atlas_utils);
void PatchIAT(PVOID pLocalMappingAddr, DLL_DATA dll_data, ATLAS_UTILS* atlas_utils, TARGET_DATA* target_data);
void FixRelocations(PVOID pTargetAddr, PVOID pLocalMappingAddr, DLL_DATA dll_data);
void FixMemoryProtections(TARGET_DATA* target_data, DLL_DATA dll_data, ATLAS_UTILS* atlas_utils);

//Utilities
void ErrorCallback(const char* msg, DWORD lastError, NTSTATUS status, ATLAS_UTILS* atlas_utils, TARGET_DATA* target_data, BOOL freeMem);
void RetrieveDLL_DATA(PVOID pDllAddr, DLL_DATA* dll_data);
void RetrieveUtils(ATLAS_UTILS* atlas_utils);

//Custom implementations
size_t C_GetProcAddress(DLL_DATA dll_data, DWORD targetFuncHash, WORD ordinal);
PVOID C_GetModuleHandle(DWORD dll);
BOOL C_LoadLibrary(char* dllName, PHANDLE hDll, ATLAS_UTILS* atlas_utils);

#endif