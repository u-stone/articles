// exception-test.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "exception-test.h"
#include <minidumpapiset.h>
#include <stdio.h>
#include <crtdbg.h>
#include <exception>
#include <new.h>
#include <iostream>
#include <vector>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void FooCode();
LONG MyUnhandledExceptionFilter(_EXCEPTION_POINTERS *ExceptionInfo);

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(
    LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
    return NULL;
}

BOOL PreventSetUnhandledExceptionFilter()
{
    HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
    if (hKernel32 == NULL) return FALSE;
    void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
    if (pOrgEntry == NULL) return FALSE;
    unsigned char newJump[100];
    DWORD dwOrgEntryAddr = (DWORD)pOrgEntry;
    dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far
    void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
    DWORD dwNewEntryAddr = (DWORD)pNewFunc;
    DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;

    newJump[0] = 0xE9;  // JMP absolute
    memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
    SIZE_T bytesWritten;
    BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
        pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
    return bRet;
}

LONG WINAPI VectoredHandler(struct _EXCEPTION_POINTERS *ExceptionInfo);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
    //SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)MyUnhandledExceptionFilter);
    //BOOL bRet = PreventSetUnhandledExceptionFilter();
    //_tprintf(_T("Prevented: %d"), bRet);
    //PVOID h1 = AddVectoredContinueHandler(1, VectoredHandler);
    FooCode();
    //std::cout << h1;

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_EXCEPTIONTEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXCEPTIONTEST));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXCEPTIONTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_EXCEPTIONTEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_LBUTTONUP:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


LONG MyUnhandledExceptionFilter(_EXCEPTION_POINTERS *ExceptionInfo)
{
    HANDLE hFile = CreateFile(_T("E:/MiniDump.dmp"), GENERIC_READ | GENERIC_WRITE,
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
    {
        // Create the minidump 

        MINIDUMP_EXCEPTION_INFORMATION mdei;

        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = ExceptionInfo;
        mdei.ClientPointers = FALSE;

        //MINIDUMP_CALLBACK_INFORMATION mci;

        //mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback;
        //mci.CallbackParam = 0;

        MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory);

        BOOL rv = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
            hFile, mdt, (ExceptionInfo != 0) ? &mdei : 0, 0, NULL);

        if (!rv)
            _tprintf(_T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError());
        else
            _tprintf(_T("Minidump created.\n"));

        // Close the file 

        CloseHandle(hFile);

    }
    else
    {
        _tprintf(_T("CreateFile failed. Error: %u \n"), GetLastError());
    }
    //MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, );
    return EXCEPTION_CONTINUE_SEARCH;
}

void myInvalidParameterHandler(const wchar_t* expression,
    const wchar_t* function,
    const wchar_t* file,
    unsigned int line,
    uintptr_t pReserved)
{
    //wprintf(L"Invalid parameter detected in function %s."
    //    L" File: %s Line: %d\n", function, file, line);
    //wprintf(L"Expression: %s\n", expression);

    // Suppress the abort message， then no popup dialog shown.
    //_set_abort_behavior(0, _WRITE_ABORT_MSG);
    //_set_abort_behavior(0, _CALL_REPORTFAULT);
    abort();
    wprintf(L"Expression: %s\n", expression);
}

void term_func() {
    RaiseException(0x4001, EXCEPTION_NONCONTINUABLE, 0, NULL);

    exit(-1);
}

#pragma warning(disable : 4996)   // for strcpy use
void vulnerable(const char *str) {
    char buffer[10];    
    strcpy(buffer, str); // overrun buffer !!!
    std::cout << buffer << std::endl;
    // use a secure CRT function to help prevent buffer overruns
    // truncate string to fit a 10 byte buffer
    // strncpy_s(buffer, _countof(buffer), str, _TRUNCATE);
}

class CDerived;
class CBase
{
public:
    CBase(CDerived *derived) : m_pDerived(derived) {};
    ~CBase();
    virtual void function(void) = 0;

    CDerived * m_pDerived;
};

class CDerived : public CBase
{
public:
    CDerived() : CBase(this) {};   // C4355
    virtual void function(void) {};
};

CBase::~CBase()
{
    m_pDerived->function();
}

void myPurecallHandler(void)
{
    printf("In _purecall_handler.");

    /// way 1: exit process directly, no dump caught.
    exit(0);
    /// way 2: Raise an exception, make an unhandled exception, and cause catch a dump.
    //RaiseException(0x4000, EXCEPTION_NONCONTINUABLE, 0, NULL);
}

int coalesced = 0;

int CoalesceHeap()
{
    coalesced = 1;  // Flag RecurseAlloc to stop
    // do some work to free memory
    return 0;
}
// Define a function to be called if new fails to allocate memory.
int MyNewHandler(size_t size)
{
    printf("Allocation failed. Coalescing heap.\n");

    // Call a function to recover some heap space.
    return CoalesceHeap();
}

const auto kBigNum = 0x1fffffff;
int RecurseAlloc() {
    /// make new operation failed for the first time.
    /// show 3 versions of new operation:

    /// version 1: process crashed.
    int *pi = new int[kBigNum];

    /// version 2: process not crashed.
    //try
    //{
    //    int *pi = new int[kBigNum];
    //}
    //catch (std::bad_alloc) {
    //    //
    //    return 0;
    //}

    /// version 3: same as version 2.
    /// modify as std::nothrow version new, MyNewHandler called
    //int *pi = new (std::nothrow) int[kBigNum];

    if (!coalesced)
        RecurseAlloc();
    return 0;
}

// crt_signal.c
// compile with: /EHsc /W4
// Use signal to attach a signal handler to the abort routine
#include <stdlib.h>
#include <signal.h>

void SignalHandler(int signal)
{
    if (signal == SIGABRT) {
        RaiseException(0x4006, EXCEPTION_NONCONTINUABLE, 0, NULL);
    }
    else if (signal == SIGSEGV) {
        RaiseException(0x4007, EXCEPTION_NONCONTINUABLE, 0, NULL);
    }
    else {
        // ...
    }
}


int test_signal()
{
    typedef void(*SignalHandlerPointer)(int);

    SignalHandlerPointer previousHandler;
    previousHandler = signal(SIGABRT, SignalHandler);

    //raise(SIGSEGV);

    /// make SIGABRT signal handler called
    abort();
    return 0;
}

void dummy(int& r) {
    r += 1;
}
void my_terminate() {
    std::cerr << "my terminate handler";
    std::exit(0);
}
void my_unexpected() {
    RaiseException(0x4005, EXCEPTION_NONCONTINUABLE, 0, NULL);
    std::cerr << "my unexpected handler";
    std::exit(EXIT_FAILURE);
}
void function() throw() // no exception in this example, but it could be another spec
{
    throw std::exception();
}
void FooCode() {
    __security_init_cookie();
    /// Disabling the program crash dialog 
    /// ref: https://devblogs.microsoft.com/oldnewthing/20040727-00/?p=38323
    DWORD mode_new = SEM_FAILCRITICALERRORS/* | SEM_NOGPFAULTERRORBOX*/;
    DWORD dwMode = SetErrorMode(mode_new);
    dwMode = SetErrorMode(dwMode | mode_new);

    MyUnhandledExceptionFilter(nullptr);
    return;

    /// high priority command.
    //_asm {cli};

    //_set_abort_behavior(0, _CALL_REPORTFAULT);
    //abort();

    /// access violation -- dump catched
    //int* p = nullptr;
    //*p = 0;

    /// infinite loop -- dump catched
    //int a[2048] = {25};
    //a[2000] = 50;
    //FooCode();

    /// throw a C++ exception -- dump catched.
    //throw std::exception("i am an exception");

    ///======== crt error handler ==========
    //-- invalid parameter -- dump not catched
    // Disable the message box for assertions(in debug mode, not in release mode).
    // If not diabled, it will popup dialog : Microsoft Visual C++ Runtime Library -- Debug Assertion Failed!
    //_invalid_parameter_handler oldHandler;
    //oldHandler = _set_invalid_parameter_handler(myInvalidParameterHandler);
    //_CrtSetReportMode(_CRT_ASSERT, 0);
    //const char* formatString = NULL;
    //printf(formatString);
    

    //-- pure call -- dump not catched.
    // popup Microsoft Visual C++ Runtime Library, just only for debug mode,
    // and does not popup any dialog for release mode.
    //_set_purecall_handler(myPurecallHandler); // used for catch the pure call exception.
    //CDerived myDerived;

    //-- new -- dump catched, and popup nothing no matter for debug/release mode
    // Set the failure handler for new to be MyNewHandler.
    //_set_new_handler(MyNewHandler);
    //RecurseAlloc();

    //-- Divide by zero! -- dump catched.
    //int i = 10, j = 0, result = 0;
    //set_terminate(term_func);
    //try
    //{
    //    throw 1;
    //    throw "hello";

    //    /// C++ try-catch can not catch the divide-by-zero exception
    //    /// if want, use __try-__exception SEH code
    //    result = i / j;
    //}
    //catch(int)
    //{
    //    /// catch throw 1
    //    RaiseException(0x4002, EXCEPTION_NONCONTINUABLE, 0, NULL);
    //}
    //catch (const char*) {
    //    /// catch throw "hello"
    //    RaiseException(0x4003, EXCEPTION_NONCONTINUABLE, 0, NULL);
    //}

    ///// compiler is so clevel that the following code can not make release mode generate result = i/j asm code.
    //result++;
    //dummy(result);

    ///// force compiler to generate result = i/j asm code.
    //std::cout << "This should never print.\n" << result;

    /// my_unexpected is not called. and this will make an VC++ style exception(exception code is 0xE06D7376 means 'msc')
    //std::set_unexpected(my_unexpected);
    //try {
    //    function();
    //}
    //catch (const std::logic_error&) {
    //    RaiseException(0x4004, EXCEPTION_NONCONTINUABLE, 0, NULL);
    //}
    //catch (const std::exception&) {
    //    RaiseException(0x4005, EXCEPTION_NONCONTINUABLE, 0, NULL);
    //}

    //-- SIGABRT signal test --no dump catched, but 
    // i can set SIGABRT handler to catch the exception.
    //test_signal();

    /// c++ style terminate.
    //std::set_terminate(nullptr);
    //std::terminate();

    //-- buffer overrun
    //char large_buffer[] = "This string is longer than 10 characters!!";
    //vulnerable(large_buffer);

    std::vector<int> v;
    v[0] = 0;

    /// and more...

    std::cout << "end of program" << v[0] << std::endl;
}



LONG WINAPI VectoredHandler(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    std::cout << "catch an exception" << std::endl;
    RaiseException(0x4011, EXCEPTION_NONCONTINUABLE, 0, NULL);
    //throw std::exception("hello");
    return EXCEPTION_CONTINUE_SEARCH; /// or EXCEPTION_CONTINUE_EXECUTION 
}