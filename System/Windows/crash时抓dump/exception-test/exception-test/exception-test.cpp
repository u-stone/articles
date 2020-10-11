// exception-test.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "exception-test.h"
#include <minidumpapiset.h>
#include <stdio.h>
#include <crtdbg.h>
#include <exception>
#include <new.h>

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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)MyUnhandledExceptionFilter);
    FooCode();
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
    exit(0);
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
    /// make new failed for the first time.
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

void FooCode() {

    /// Disabling the program crash dialog 
    /// ref: https://devblogs.microsoft.com/oldnewthing/20040727-00/?p=38323
    DWORD dwMode = SetErrorMode(SEM_NOGPFAULTERRORBOX);
    SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);

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
    RecurseAlloc();

    /// and more...
}