// Copyright Sam Bonifacio. All Rights Reserved.

#if PLATFORM_WINDOWS && defined(__clang__)

#include "Microsoft/WindowsHWrapper.h"

// Installed UE Windows binaries are built with MSVC, so Core does not emit the
// clang-only MinimalWindowsApi wrapper exports. Project modules built with clang still
// import those symbols, so provide compatible exports from this plugin module instead.
namespace Windows
{
	AUTOSETTINGSCORE_API ::HMODULE WINAPI LoadLibraryW(::LPCTSTR lpFileName)
	{
		return ::LoadLibraryW(lpFileName);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI FreeLibrary(::HMODULE hModule)
	{
		return ::FreeLibrary(hModule);
	}

	AUTOSETTINGSCORE_API void WINAPI InitializeCriticalSection(::LPCRITICAL_SECTION lpCriticalSection)
	{
		::InitializeCriticalSection(lpCriticalSection);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI InitializeCriticalSectionAndSpinCount(::LPCRITICAL_SECTION lpCriticalSection, ::DWORD dwSpinCount)
	{
		return ::InitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
	}

	AUTOSETTINGSCORE_API ::DWORD WINAPI SetCriticalSectionSpinCount(::LPCRITICAL_SECTION lpCriticalSection, ::DWORD dwSpinCount)
	{
		return ::SetCriticalSectionSpinCount(lpCriticalSection, dwSpinCount);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI TryEnterCriticalSection(::LPCRITICAL_SECTION lpCriticalSection)
	{
		return ::TryEnterCriticalSection(lpCriticalSection);
	}

	AUTOSETTINGSCORE_API void WINAPI EnterCriticalSection(::LPCRITICAL_SECTION lpCriticalSection)
	{
		::EnterCriticalSection(lpCriticalSection);
	}

	AUTOSETTINGSCORE_API void WINAPI LeaveCriticalSection(::LPCRITICAL_SECTION lpCriticalSection)
	{
		::LeaveCriticalSection(lpCriticalSection);
	}

	AUTOSETTINGSCORE_API void WINAPI DeleteCriticalSection(::LPCRITICAL_SECTION lpCriticalSection)
	{
		::DeleteCriticalSection(lpCriticalSection);
	}

	AUTOSETTINGSCORE_API void WINAPI InitializeSRWLock(::PSRWLOCK SRWLock)
	{
		::InitializeSRWLock(SRWLock);
	}

	AUTOSETTINGSCORE_API void WINAPI AcquireSRWLockShared(::PSRWLOCK SRWLock)
	{
		::AcquireSRWLockShared(SRWLock);
	}

	AUTOSETTINGSCORE_API ::BOOLEAN WINAPI TryAcquireSRWLockShared(::PSRWLOCK SRWLock)
	{
		return ::TryAcquireSRWLockShared(SRWLock);
	}

	AUTOSETTINGSCORE_API ::BOOLEAN WINAPI TryAcquireSRWLockExclusive(::PSRWLOCK SRWLock)
	{
		return ::TryAcquireSRWLockExclusive(SRWLock);
	}

	AUTOSETTINGSCORE_API void WINAPI ReleaseSRWLockShared(::PSRWLOCK SRWLock)
	{
		::ReleaseSRWLockShared(SRWLock);
	}

	AUTOSETTINGSCORE_API void WINAPI AcquireSRWLockExclusive(::PSRWLOCK SRWLock)
	{
		::AcquireSRWLockExclusive(SRWLock);
	}

	AUTOSETTINGSCORE_API void WINAPI ReleaseSRWLockExclusive(::PSRWLOCK SRWLock)
	{
		::ReleaseSRWLockExclusive(SRWLock);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI ConnectNamedPipe(::HANDLE hNamedPipe, ::LPOVERLAPPED lpOverlapped)
	{
		return ::ConnectNamedPipe(hNamedPipe, lpOverlapped);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI GetOverlappedResult(::HANDLE hFile, ::LPOVERLAPPED lpOverlapped, ::LPDWORD lpNumberOfBytesTransferred, ::BOOL bWait)
	{
		return ::GetOverlappedResult(hFile, lpOverlapped, lpNumberOfBytesTransferred, bWait);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI WriteFile(::HANDLE hFile, ::LPCVOID lpBuffer, ::DWORD nNumberOfBytesToWrite, ::LPDWORD lpNumberOfBytesWritten, ::LPOVERLAPPED lpOverlapped)
	{
		return ::WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI ReadFile(::HANDLE hFile, ::LPVOID lpBuffer, ::DWORD nNumberOfBytesToRead, ::LPDWORD lpNumberOfBytesRead, ::LPOVERLAPPED lpOverlapped)
	{
		return ::ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI QueryPerformanceCounter(::LARGE_INTEGER* Cycles)
	{
		return ::QueryPerformanceCounter(Cycles);
	}

	AUTOSETTINGSCORE_API ::DWORD WINAPI GetCurrentThreadId()
	{
		return ::GetCurrentThreadId();
	}

	AUTOSETTINGSCORE_API ::DWORD WINAPI TlsAlloc()
	{
		return ::TlsAlloc();
	}

	AUTOSETTINGSCORE_API ::LPVOID WINAPI TlsGetValue(::DWORD dwTlsIndex)
	{
		return ::TlsGetValue(dwTlsIndex);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI TlsSetValue(::DWORD dwTlsIndex, ::LPVOID lpTlsValue)
	{
		return ::TlsSetValue(dwTlsIndex, lpTlsValue);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI TlsFree(::DWORD dwTlsIndex)
	{
		return ::TlsFree(dwTlsIndex);
	}

	AUTOSETTINGSCORE_API ::DWORD WINAPI FlsAlloc(::PFLS_CALLBACK_FUNCTION lpCallback)
	{
		return ::FlsAlloc(lpCallback);
	}

	AUTOSETTINGSCORE_API ::LPVOID WINAPI FlsGetValue(::DWORD dwFlsIndex)
	{
		return ::FlsGetValue(dwFlsIndex);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI FlsSetValue(::DWORD dwFlsIndex, ::LPVOID lpFlsValue)
	{
		return ::FlsSetValue(dwFlsIndex, lpFlsValue);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI FlsFree(::DWORD dwTlsIndex)
	{
		return ::FlsFree(dwTlsIndex);
	}

	AUTOSETTINGSCORE_API ::BOOL WINAPI IsProcessorFeaturePresent(::DWORD ProcessorFeature)
	{
		return ::IsProcessorFeaturePresent(ProcessorFeature);
	}
} // namespace Windows

#endif
