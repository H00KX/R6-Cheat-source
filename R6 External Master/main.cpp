/*

	Entry-point for usermode, handles connection to driver,
	and essentially glues every function together.

*/

#include "Threads.h" // Includes globals.h, which in turn, includes driver.h
#include "Gui/Gui.h"
#include "Game/aimbot/Aimbot.h"

uint64_t get_module_base(uint32_t pd, char* module_name) {
	int returnValue[256];
	int moduleSize;
	if (strlen((char*)module_name) <= 256)
	{
		for (int i = 0; i < strlen(module_name); i++) {
			returnValue[i] = (int)module_name[i];
			moduleSize = i;
		}
	}
	uint64_t base_address = driver::get_process_base_address(connection, pid, returnValue, moduleSize);
	//pid = ProcessID;
	if (!base_address) {
		return 0x0;
	}
	return base_address;
}

int main()
{
	SetConsoleTitle(L"Discord");

	system("cls");

	// Driver startup
	{
		driver::initialize();
		connection = driver::connect();
		if (connection == INVALID_SOCKET) // If we cannot connect to our driver
		{
			MessageBox(NULL, L"Failed to connect to kernelmode driver!\n> Aborting task!", L"Fatal Error", MB_ICONHAND);
			exit(-1);
		}

		pid = fPid();
		if (!pid) // If the game is not running
		{
			MessageBox(NULL, L"Failed to find game process!\n> Must abort!", NULL, MB_ICONHAND);
			exit(-1);
		}

		base_address = get_module_base(pid, (char*)"RainbowSix.exew");
		if (!base_address) // If cannot get process base address
		{
			MessageBox(NULL, L"Failed to find process' base address!\n> Must abort!", NULL, MB_ICONHAND);
			exit(-1);
		}
	}

	std::cout << blue << " $ Welcome to clara.wtf " << user << "!\n\n";

	std::cout << yellow << " > Loading driver . "; sleepms(300); std::cout << ". "; sleepms(300); std::cout << ". \n"; sleepms(300);
	std::cout << green << " [>] Driver Connection: "        << white; LoadingAnim(1); std::cout << green << "Success\n";
	std::cout << green << " [>] Rainbow Six Process ID: "   << white; LoadingAnim(1); std::cout << green << pid << "\n";
	std::cout << green << " [>] Rainbow Six Base Address: " << white; LoadingAnim(1); std::cout << green << std::hex << base_address << "\n\n";

	std::cout << yellow << " > Loading main feature thread . "; sleepms(300); std::cout << ". "; sleepms(300); std::cout << ". \n"; sleepms(300);
	std::cout << green << " [>] Feature thread: " << white; LoadingAnim(1); std::cout << green << "Online\n\n";

	std::cout << yellow << " > Loading gui thread . "; sleepms(300); std::cout << ". "; sleepms(300); std::cout << ". \n"; sleepms(300);
	std::thread GuiThread(Gui);
	std::cout << green << " [>] ImGui thread: "   << white; LoadingAnim(1); std::cout << green << "Online\n\n";

	std::cout << yellow << " > When you wish to terminate the application, please consider using [END].\n";
	std::cout << green << " [!] Detaching this CMD window as it is not needed . "; sleepms(300); std::cout << ". "; sleepms(300); std::cout << ". \n"; sleepms(300);
	sleeps(5);

	//FreeConsole();
	while (!GetAsyncKeyState(VK_END))
	{
		//UpdateOutline();
		UpdateFeatures();
		sleepms(1);
	}

	driver::disconnect(connection);
	driver::deinitialize();
	return 0;
}

