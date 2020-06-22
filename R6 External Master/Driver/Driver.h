#pragma once
#include <WinSock2.h>
#include <cstdint>
#include <sstream>
#include "stdint.h"
namespace driver
{
	void	initialize();
	void	deinitialize();

	SOCKET	connect();
	void	disconnect(SOCKET connection);

	uint32_t read_memory(SOCKET connection, uint32_t process_id, uintptr_t address, uintptr_t buffer, size_t size);
	uint32_t write_memory(SOCKET connection, uint32_t process_id, uintptr_t address, uintptr_t buffer, size_t size);
	uint64_t get_process_base_address(const SOCKET connection, const uint32_t process_id, int* nameModule, uint32_t moduleSize);
	uint64_t clean_piddbcachetable(SOCKET connection);
	uint64_t clean_mmunloadeddrivers(SOCKET connection);
	uint64_t hwid_spoofing(SOCKET connection);
	uint32_t copy_memory(const SOCKET connection, const uint32_t src_process_id, const uintptr_t src_address, const uint32_t dest_process_id, const uintptr_t dest_address, const size_t size);
	bool isHex(char c);

	template <typename T>
	T read(const SOCKET connection, const uint32_t process_id, const uintptr_t address)
	{
		T buffer{ };
		read_memory(connection, process_id, address, uint64_t(&buffer), sizeof(T));

		return buffer;
	}

	template <typename T>
	void write(const SOCKET connection, const uint32_t process_id, const uintptr_t address, const T& buffer)
	{
		write_memory(connection, process_id, address, uint64_t(&buffer), sizeof(T));
	}
}