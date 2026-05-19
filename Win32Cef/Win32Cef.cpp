// Win32Cef.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Win32Cef.h"

#include <include/cef_app.h>
#include "simple_app.h"
#include "simple_handler.h"
#include <atltypes.h>


#define MAX_LOADSTRING 100
HWND g_hRootWnd = nullptr;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
CefRefPtr<SimpleHandler> g_handler;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	OutputDebugStringW(L"[lsw] Win32Cef start.");

	// ========== 第 1 步：CEF 进程判断（必须最先执行）==========
	void *sandbox_info = NULL;
	CefMainArgs main_args(hInstance);

	// CEF默认是多进程，wWinMain会执行多次。通过CefExecuteProcess来过滤。
	// 如果是子进程（Renderer/GPU等），CefExecuteProcess 会阻塞并接管，
	// 返回 >= 0 表示当前进程使命已完成，应直接退出
	int exit_code = CefExecuteProcess(main_args, nullptr, sandbox_info);
	if (exit_code >= 0)
	{
		OutputDebugStringW(L"[lsw] 走到这里，说明是Win32Cef子进程，且任务已完成，直接退出。");

		return exit_code;
	}

	OutputDebugStringW(L"[lsw] 只有Win32Cef主进程，才会执行到这里。");

	// ========== 第 2 步：以下只有 Browser 主进程才会执行 ==========

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32CEF, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// InitInstance内部创建HWND。
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	// 关闭沙箱，浏览器初始化，因为接下来要把进程改为单进程模式，它与沙箱不兼容。
	CefSettings settings;
	settings.no_sandbox = true;

	// Cef初始化
	CefRefPtr<SimpleApp> app(new SimpleApp);
	CefInitialize(main_args, settings, app.get(), sandbox_info);

	// 网页加载时的处理器。
	CefRefPtr<SimpleHandler> _handler(new SimpleHandler(false));
	g_handler = _handler;

	CefWindowInfo window_info;

	CRect rcClient;
	GetClientRect(g_hRootWnd, rcClient);
	CefRect rc = {0, 0, rcClient.Width(), rcClient.Height()};
	window_info.SetAsChild(g_hRootWnd, rc);

	// 创建Browser窗口，加载百度首页。
	CefBrowserSettings browser_settings;
	CefBrowserHost::CreateBrowser(window_info, g_handler.get(), R"(https://www.baidu.com)", browser_settings, nullptr, nullptr);

	// 消息循环
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32CEF));

	MSG msg;
	bool running = true;

	while (running)
	{
		// 1. 先处理所有待处理的 Win32 消息（非阻塞）
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				running = false;
				break;
			}
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		if (!running)
		{
			break;
		}

		// 2. 让 CEF 处理任务（非阻塞）
		CefDoMessageLoopWork();

		// 3. 短暂休眠，避免 CPU 占满（关键！）
		// 如果没有消息也没有 CEF 任务，让出 CPU
		Sleep(10);  // 10ms 通常足够，可根据需要调整
	}

	//MSG msg;

	//// Main message loop:
	//while (GetMessage(&msg, nullptr, 0, 0))
	//{
	//	CefDoMessageLoopWork();

	//	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//}

	CefShutdown();

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32CEF));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32CEF);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = nullptr;

	OutputDebugStringW(L"[lsw] create hwnd");

	hWnd = CreateWindowExW(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		1000, 300, 1000, 800, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	g_hRootWnd = hWnd;

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
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
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;

	// PostQuitMessage(0)可以由WM_DESTROY调用，也可以由SimpleHandler::OnBeforeClose调用，
	// 后者机会更晚，后者在所有browser都关闭时调用。所以，我们选择在SimpleHandler::OnBeforeClose调用PostQuitMessage(0)，
	// 而注释掉WM_DESTROY中的PostQuitMessage(0)调用。
	// 另注：因为现在settings.multi_threaded_message_loop == false;
	// 所以不能调用CefQuitMessageLoop()，必须调用PostQuitMessage(0)来退出消息循环。
	//case WM_DESTROY:
	//	PostQuitMessage(0);
	//	break;

	case WM_SIZE:
	{
		if (g_handler && g_handler->GetBrowser())
		{
			HWND hBrowserWnd = g_handler->GetBrowser()->GetHost()->GetWindowHandle();
			if (hBrowserWnd)
			{
				RECT rc;
				GetClientRect(hWnd, &rc);
				SetWindowPos(hBrowserWnd, nullptr, 0, 0, rc.right, rc.bottom,
					SWP_NOZORDER | SWP_SHOWWINDOW);
			}
		}
		break;
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
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
