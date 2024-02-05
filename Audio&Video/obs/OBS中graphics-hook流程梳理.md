

obs中的graphics-hook是一个dll库，用于hook各类图形绘制函数。

主入口函数dllMain位于graphics-hook.c中，dll在加载到一个进程之后（DLL_PROCESS_ATTACH），会初始化hook。初始化过程中创建了一个线程main_capture_thread，用于执行hook任务。

main_capture_thread做两件事初始化hook，并开始抓屏

```cpp
static DWORD WINAPI main_capture_thread(HANDLE thread_handle)
{
	if (!init_hook(thread_handle)) {
		DbgOut("Failed to init hook\n");
		free_hook();
		return 0;
	}

	capture_loop();
	return 0;
}
```

## init_hook

初始化hook工作主要是创建一个管道（名为：CaptureHook_HookInfo+当前进程ID），用于传输抓到的数据。然后再创建一个线程，在线程中创建一个1x1大小的窗口，做消息泵。做完之后，激活事件signal_restart，表示hook开始工作了。

```cpp
static inline bool init_hook(HANDLE thread_handle)
{
	wait_for_dll_main_finish(thread_handle);

	_snwprintf(keepalive_name, sizeof(keepalive_name) / sizeof(wchar_t),
		   L"%s%lu", WINDOW_HOOK_KEEPALIVE, GetCurrentProcessId());

	init_pipe();

	init_dummy_window_thread();
	log_current_process();

	SetEvent(signal_restart);
	return true;
}
```

## capture_loop

这个函数就是用来抓屏的。实现也很简单：

```cpp
static inline void capture_loop(void)
{
	WaitForSingleObject(signal_init, INFINITE);

	while (!attempt_hook())
		Sleep(40);

	for (size_t n = 0; !stop_loop; n++) {
		/* this causes it to check every 4 seconds, but still with
		 * a small sleep interval in case the thread needs to stop */
		if (n % 100 == 0)
			attempt_hook();
		Sleep(40);
	}
}
```

主要工作在attempt_hook中。这个函数会安装dx8~dx12,dxgi,opengl,vulkan的hook。

要想让game capturer生效需要启动一个dx的程序。比如Dx2010 SDK中的PostProcess.exe，并不是所有dx程序都可以。

obs中有好些dll子项目中并没有DllMain入口，而是通过LoadLibrary加载dll，然后直接调用其中的导出函数进行初始化的。

obs通过obs_load_all_modules函数加载所有dll，具体由:

- obs_find_modules
- find_modules_in_path
- process_found_module

搜索固定的路径下的dll实现，相关Win32 API：[SetDllDirectoryA](https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setdlldirectorya)。

