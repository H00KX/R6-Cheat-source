/*

	Definitions and declerations for project-wide
	functions and variables

*/
#pragma warning(disable:4996)
#pragma once
#include "Driver/Driver.h"
#include "ConsoleColour.h"

#include <iostream>
#include <string>
#include <thread>
#include <TlHelp32.h>
#include <winternl.h>
#include <ntstatus.h>
#include <minwindef.h>
#include <math.h>
// For read and write
SOCKET connection;
uint32_t pid;
uint64_t base_address;
//in Globals.h
std::string user = "Default";

struct ProcessEntry {
	wchar_t name[60];
	uint32_t id;
};
// Enumerates the process list.
class ProcessEnumerator {
public:
	ProcessEnumerator();
	bool next(ProcessEntry& entry);
private:
	std::unique_ptr<uint8_t[]> buffer;
	size_t buffer_len = 0;
	size_t next_offset = 0;
};

// Options
namespace Options
{
	bool CavESPStat = false;
	bool bOutlineESPStatus = false;

	bool f_GlowStat = false;
	bool f_GlowWeapon = false;
	float fGlowValue[3] = { 0.0f, 0.0f, 0.0f };

	bool f_BGlowStat = false;
	bool f_PGlowStat = false;
	bool f_NoFlash = false;

	float f_CharFov = 1.f;
	float f_WeaponFov = 0.8f;
	float f_Recoil = 1.f;

	bool f_Aimbot = false;

}

const wchar_t* GetWC(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

ProcessEnumerator::ProcessEnumerator() {
	using NtQuerySystemInformationFn = NTSTATUS WINAPI(IN SYSTEM_INFORMATION_CLASS, OUT PVOID, IN ULONG, OUT PULONG);
	const auto NtQuerySystemInformation = reinterpret_cast<NtQuerySystemInformationFn*>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQuerySystemInformation"));
	ULONG return_length;
	while (NtQuerySystemInformation(SystemProcessInformation, buffer.get(), static_cast<ULONG>(buffer_len), &return_length) < 0) {
		buffer = std::unique_ptr<uint8_t[]>(new uint8_t[return_length]);
		buffer_len = return_length;
	}
}
bool ProcessEnumerator::next(ProcessEntry& entry) {
	if (next_offset >= buffer_len) {
		return false;
	}
	const auto pi = reinterpret_cast<const SYSTEM_PROCESS_INFORMATION*>(buffer.get() + next_offset);
	next_offset += pi->NextEntryOffset != 0 ? pi->NextEntryOffset : buffer_len - next_offset;

	entry.id = static_cast<uint32_t>((size_t)pi->UniqueProcessId);
	memset(&entry.name, 0, sizeof(entry.name));
	memcpy(&entry.name, pi->ImageName.Buffer, min(pi->ImageName.Length, sizeof(entry.name) - 1));

	return true;
}

std::uint32_t fPid()
{

	ProcessEntry entry;
	for (ProcessEnumerator processes{}; processes.next(entry); ) {
		// Ignore the last process id until it has gone away
		if (!wcscmp(entry.name, L"RainbowSix.exe")) {
			return entry.id;

		}
	}
	return 0;
}


// For getting game process
std::uint32_t find_process_by_id(const std::string& name)
{
	const auto snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap == INVALID_HANDLE_VALUE) {
		return 0;
	}

	PROCESSENTRY32 proc_entry{};
	proc_entry.dwSize = sizeof proc_entry;

	auto found_process = false;
	if (!!Process32First(snap, &proc_entry)) {
		do {
			if (GetWC(name.c_str()) == proc_entry.szExeFile) {
				found_process = true;
				break;
			}
		} while (!!Process32Next(snap, &proc_entry));
	}

	CloseHandle(snap);
	return found_process
		? proc_entry.th32ProcessID
		: 0;
}

// Shorter for reading and writing to process memory
template <typename T>
T mRead(const uintptr_t address)
{
	T buffer{ };
	driver::read_memory(connection, pid, address, uint64_t(&buffer), sizeof(T));

	return buffer;
}
template <typename T>
void mWrite(const uintptr_t address, const T& buffer)
{
	driver::write_memory(connection, pid, address, uint64_t(&buffer), sizeof(T));
}

// Shorter for sleeping (pausing code)
void sleepms(const int time)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(time));
}
void sleeps(const int time)
{
	std::this_thread::sleep_for(std::chrono::seconds(time));
}



// Loading animation
void LoadingAnim(int iterations)
{
	std::cout << '-' << std::flush;
	for (int i = 0; i < iterations; i++)
	{
		sleepms(125);
		std::cout << "\b\\" << std::flush;
		sleepms(125);
		std::cout << "\b|" << std::flush;
		sleepms(125);
		std::cout << "\b/" << std::flush;
		sleepms(125);
		std::cout << "\b-" << std::flush;
	}
	std::cout << "\b";

	return;
}