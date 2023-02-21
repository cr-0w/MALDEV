## `💉` Shellcode Injection

This technique is as vanilla as it gets; the simplest. It's also *quite* elegant. By attaching to an existing process, we're able to inject our own shellcode into it and have our shellcode run. A bit of an oversimplification since there's actually a lot going on under the covers; but we'll talk about that next. We're going to be using a lot of **Win32 API** calls for our malware development until we get more advanced in our craft. I've created another repo called [low-level](https://github.com/cr-0w/low-level) that also documents my experience with low-level languages including: `Assembly`, `C`, `C++`, `Win32 API`, etc. You could take a gander at it before coming here in case you're brand-brand new.

A shellcode injection consists of the following API calls:

- `OpenProcess()`
- `VirtualAllocEx()`
- `WriteProcessMemory()`
- `CreateRemoteThread()`

> **Note**:
> *All of the documentation for these functions, as well as the Win32 API can be found/read from [Microsoft themselves](https://learn.microsoft.com/en-us/windows/win32/apiindex/windows-api-list). It's an amazing resource; although it may seem extremely daunting at first. However, I promise that if you take the time to actually read it - as well as read the related documents that use your function, you'll come to appreciate this resource.*



## `💖` Extra Resources

Some other resources you can use to read up on this are:

- [CreateRemoteThread Shellcode Injection](https://www.ired.team/offensive-security/code-injection-process-injection/process-injection)
- [Process Injection using CreateRemoteThread API](https://tbhaxor.com/createremotethread-process-injection/)
- [Process Injection: Remote Thread Injection or CreateRemoteThread](https://aliongreen.github.io/posts/remote-thread-injection.html)
