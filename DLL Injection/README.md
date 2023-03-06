## `💉` DLL Injection

This type of process injection technique is the natural successor to the simple [shellcode injection](https://github.com/cr-0w/MALDEV/tree/main/Shellcode%20Injection) we did a while ago. Instead of getting our dirty little hands on a process, opening it, allocating some memory inside of it, writing to that previously allocated memory with our shellcode, and finally creating a thread to run our shellcode, we instead utilize the [`LoadLibraryA()`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya?redirectedfrom=MSDN) function (after finding its address, as we'll see later on). From the documentation, we can see that it simply just takes the library that we'd like to load, and it *just works*. Magically!

```c
HMODULE LoadLibraryA(
  [in] LPCSTR lpLibFileName
);
```

> **Warning**: *Please note that although this function is great for our purposes; it's got a couple of caveats that we'll see later on. The biggest being that if we've opened a process, and injected into it with this function, [we won't be able to rerun our exploit](http://blog.opensecurityresearch.com/2013/01/windows-dll-injection-basics.html).*
>
> *"The major downside to LoadLibraryA() is that it registers the loaded DLL with the program and thus can be easily detected. **Another slightly annoying caveat is that if a DLL has already been loaded once with LoadLibraryA(), it will not execute it.** You can work around this issue but it's more code."* - [Brad Antoniewicz, Open Security Research](http://blog.opensecurityresearch.com/2013/01/windows-dll-injection-basics.html)

I'll assume that you already know what a DLL is, however, if you don't then fret not because we'll be making our own in the next section and I'll provide a short little synopsis of what DLLs are and what they do.

## `🔖` Example DLL

A DLL is a "`Dynamic Link Library`". A DLL is a **collection of data or executable functions that a Windows application can use**. DLLs enable several apps to **share the same code rather than having each application contain all of the necessary code**, one-by-one.

DLLs are crucial because they let programmers modularize their work and share similar code among several apps. This can lower the size and complexity of the codebase while also saving time and effort when creating new software. Pretty much any process that you open is going to have *some* DLL that it uses/needs in order to work properly.

With a quick little synopsis now done, let's move on to creating a specific DLL for our exploit. You *could* 100% just [generate a DLL](https://pentestlab.blog/2017/04/04/dll-injection/) from a tool like `msfvenom`, but I think in this case, since we're learning, it could be benificial to just **create our own**. First, let's take a look at a simple C-file (it'll just be a super simple [message box script](https://github.com/cr-0w/low-level/blob/main/C/winapi/winapi.c)):

```cpp
#include <stdio.h>
#include <windows.h>

int main(void) {
     MessageBoxW(NULL, L"you have enough to enter the book shop!", L"COME IN!", MB_OK);
     return EXIT_SUCCESS;
}
```

This program simply just opens up a message box with the [MessageBoxW()](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox) WIN API function, as we can see in the picture below:

![image](https://user-images.githubusercontent.com/59679082/223012301-23fae22e-c3aa-41cc-ac34-534d9d452346.png)

Now, what if we wanted to create a DLL that did the exact same thing? The way a DLL works is a *bit* different but it's not too bad. Just like with normal EXEs or C-scripts or whatever, the program has a `main()`, for DLL's, it's called: [`DllMain`](https://learn.microsoft.com/en-us/windows/win32/dlls/dllmain). From the documentation:

```cpp
BOOL WINAPI DllMain(
  _In_ HINSTANCE hinstDLL,
  _In_ DWORD     fdwReason,
  _In_ LPVOID    lpvReserved
);
```

This will be the DLL's **entry point**, similar to how our C-files have the `main()` entry point. Now, let's look at the example from the documentation:

```cpp
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
        
            if (lpvReserved != nullptr)
            {
                break; // do not do cleanup if process termination scenario
            }
            
         // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
```

For our purposes, we'll just have a super simple DLL, nothing extra. We only care about the `case DLL_PROCESS_ATTACH` in this case (*pun, 1000% intended*). So, let's define our DLL as:

![image](https://user-images.githubusercontent.com/59679082/223013305-3bae9d51-547c-429a-b0fc-1762735a10ad.png)

> **Note**: *If you'd like to follow along, the code can be found in this repo. More specifically, the [example.c](https://github.com/cr-0w/MALDEV/blob/main/DLL%20Injection/example.c) file*.

Let's compile this, making sure to specify the output as a `DLL`:

```
PS C:\DLL Injection\src> mingw32-g++.exe -shared -o exampleDLL.dll exampleDLL.c      
```

Congratulations, you've just compiled your first DLL! 🎉

## `💉` DLL Injection (Continued)

A DLL injection consists of the pretty much the same API calls as the ones we saw from the [shellcode injection](https://github.com/cr-0w/MALDEV/tree/main/Shellcode%20Injection) example:

- [`OpenProcess()`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess)
- [`VirtualAllocEx()`](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex)
- [`WriteProcessMemory()`](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory)
- [`CreateRemoteThread()`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethread)

Except, we also have to include the path to the DLL in our script:

```cpp
wchar_t dllLocation[MAX_PATH] = L"C:\\Users\\Niko Bellic\\Desktop\\Tools\\DLL Injection\\example.dll";
```

Moreover, there's also the added obstacle of needing to find the address of the [`LoadLibraryA()`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya?redirectedfrom=MSDN) API we were talking about. This API is responsible for loading our DLL into the target process. Therefore, we *desperately* need to find and utilize it. How? Well, we can get the address of this function with the following lines of code (as seen in Cocomelonc's [**amazing blog post**](https://cocomelonc.github.io/tutorial/2021/09/20/malware-injection-2.html)):

```c
HMODULE hKernel32 = GetModuleHandle("Kernel32");
VOID *lb = GetProcAddress(hKernel32, "LoadLibraryA");
```

What we're doing here is first using [`GetModuleHandle()`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandlea) to get a handle to [`Kernel32`](https://www.techopedia.com/definition/3379/kernel32dll#:~:text=operations%20and%20interrupts.-,Kernel32.,other%20system%20or%20user%20processes.) which is a DLL/Module used by... you guessed it, the Windows Kernel! It's actually one of the most important files that your computer requires in order to function properly. The next part, is the usage of [`GetProcAddress()`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress). This is really cool so listen up! With this, we supply the handle of the module that we want to search inside of, in this case, **Kernel32**, and specify the function that we'd like to get the address for! So, in other words, we can see it as:

```c
HMODULE hKernel32 = GetModuleHandle("Kernel32"); // get a handle to the Kernel32 Module
VOID *lb = GetProcAddress(hKernel32, "LoadLibraryA"); // inside of the Kernel32 module, look for "LoadLibraryA()"
```

Pretty straight forward! 😄 

> **Note**: *In [my script](dll.c), I have it setup in a bit of a different way; I'm using but just know that the premise is the exact same:*

```c
    [snip...]
    
    PTHREAD_START_ROUTINE startRoutine = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW"); 
    
    [snip...]
```

All in all, that brings all of the functions that we needed for this specific technique to the following:

- [`OpenProcess()`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess)
- [`VirtualAllocEx()`](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex)
- [`GetModuleHandle()`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandlea)
- [`LoadLibraryA()`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya?redirectedfrom=MSDN)
- [`GetProcAddress()`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress)
- [`WriteProcessMemory()`](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory)
- [`CreateRemoteThread()`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethread)

## `💽` Demo

> **Warning**: *Once again, you could include whatever you want in your DLL, I opted to not go for a reverse shell or anything because I'm more interested in the PoC as of right now.* 

https://user-images.githubusercontent.com/59679082/223062495-22dab688-a5dd-4478-ab3d-ce660b495a1e.mp4

And a neat little thing once we see when we start inspecting the memory, is that we find some cool little artefacts like the `lpText` and `lpCaption` parameters of the [`MessageBoxW()`](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messageboxw) function.

![image](https://user-images.githubusercontent.com/59679082/223063205-0f81ffda-1c52-4e8e-8412-7b6886a3b0f2.png)

## `💖` Extra Resources

> **Note**: *I will be making a YouTube video about this technique and the [Shellcode Injection](https://github.com/cr-0w/MALDEV/tree/main/Shellcode%20Injection) on my [YouTube Channel](https://www.youtube.com/@crr0ww), once I do, I'll update this line with the link to the video. Until then, keep your eyes peeled* 😄 

- [`DLL Injection`](https://www.ired.team/offensive-security/code-injection-process-injection/dll-injection)
- [`Windows DLL Injection Basics`](http://blog.opensecurityresearch.com/2013/01/windows-dll-injection-basics.html)
- [`DLL Injection Attacks in a Nutshell`](https://medium.com/bug-bounty-hunting/dll-injection-attacks-in-a-nutshell-71bc84ac59bd)
- [`Classic DLL Injection Into The Process`](https://cocomelonc.github.io/tutorial/2021/09/20/malware-injection-2.html)
