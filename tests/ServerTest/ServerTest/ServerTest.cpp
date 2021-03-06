#include "stdafx.h"
#include "ServerTest.h"
#include <stdio.h>
#define MAX_LOADSTRING 100
#define SEND_BTN 1001
#define NET_ADDRESS 1002

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				Connect(std::string, int port);

std::string ErrorCode2Text(DWORD dw)
{
	TCHAR s[100];
	_stprintf_s(s, _T("%X"), dw);
	OutputDebugString(s);
	std::string error = "";
	// Put your own common error text here (give more explaination etc..) 
	switch (dw)
	{
	case WSAEFAULT:
		error = "WSAEFAULT	The buf parameter is not completely contained in a valid part of the user address space.";

		break;
	case WSAENOTCONN:
		error = "WSAENOTCONN	The socket is not connected.";
		break;
	case WSAEINTR:
		error = "WSAEINTR	The (blocking) call was canceled through WSACancelBlockingCall.	";
		break;
	case WSAENOTSOCK:
		error = " WSAENOTSOCK	The descriptor s is not a socket.";
		break;
	case WSANOTINITIALISED:
		error = "WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.";
		break;
	case WSAENETDOWN:
		error = "WSAENETDOWN	The network subsystem has failed.";
		break;
	case WSAEINPROGRESS:
		error = "WSAEINPROGRESS	A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
		break;
	case WSAENETRESET:
		error = " WSAENETRESET	The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress.";
		break;
	case WSAEOPNOTSUPP:
		error = "WSAEOPNOTSUPP	MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.	";
		break;
	case WSAESHUTDOWN:
		error = "WSAESHUTDOWN	The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.";
		break;
	case WSAEWOULDBLOCK:
		error = " WSAEWOULDBLOCK	The socket is marked as nonblocking and the receive operation would block.	";
		break;
	case WSAEMSGSIZE:
		error = " WSAENOTSOCK		The message was too large to fit into the specified buffer and was truncated.";
		break;
	case WSAEINVAL:
		error = "WSAEINVAL	The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative.	";
	case WSAECONNABORTED:
		error = " 	WSAECONNABORTED	The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.";
		break;
	case WSAETIMEDOUT:
		error = "WSAETIMEDOUT	The connection has been dropped because of a network failure or because the peer system failed to respond.	";
		break;
	case WSAECONNRESET:
		//error="WSAECONNRESET	The virtual circuit was reset by the remote side executing a hard or abortive close."; 
		error = "WSAECONNRESET Connection dropped..";
		break;

	default:
		error = "";
		break;
	}

	return error;
}
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	//
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERVERTEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVERTEST));

    MSG msg;

    // Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVERTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SERVERTEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, szWindowClass, szTitle, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 480, 400, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HWND CreateListView(HWND hwndParent)
{
	INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	RECT rcClient;                       // The parent window's client area.

	GetClientRect(hwndParent, &rcClient);

	// Create the list-view window in report view with label editing enabled.
	HWND hWndListView = CreateWindow(WC_LISTVIEW,
		L"",
		WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_EDITLABELS | WS_BORDER,
		10, 40,
		rcClient.right - rcClient.left - 20,
		rcClient.bottom - rcClient.top - 50,
		hwndParent,
		0,
		hInst,
		NULL);

	return (hWndListView);
}


// InitListViewColumns: Adds columns to a list-view control.
// hWndListView:        Handle to the list-view control. 
// Returns TRUE if successful, and FALSE otherwise. 
BOOL InitListViewColumns(HWND hWndListView)
{
	WCHAR szText[256];     // Temporary buffer.
	LVCOLUMN lvc;
	int iCol;

	// Initialize the LVCOLUMN structure.
	// The mask specifies that the format, width, text,
	// and subitem members of the structure are valid.
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	int C_COLUMNS = 4;
	// Add the columns.
	RECT rcClient;                       // The parent window's client area.

	GetClientRect(hWndListView, &rcClient);
	for (iCol = 0; iCol < C_COLUMNS; iCol++)
	{
		wchar_t str[256];
		swprintf_s(str, L"number: %d \n", iCol);

		OutputDebugString(str);

		lvc.iSubItem = iCol;
		lvc.pszText = szText;
		lvc.cx = ((rcClient.right - rcClient.left) / 4) + 1;               // Width of column in pixels.

		if (iCol < 2)
			lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
		else
			lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

		// Load the names of the column headings from the string resources.
		LoadString(hInst,
			IDS_FIRSTCOLUMN + iCol,
			szText,
			sizeof(szText) / sizeof(szText[0]));
	
		// Insert the columns into the list view.
		if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1)
			return FALSE;
	}

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
	static HWND editBox = {};

	
	switch (message) {
	case WM_CREATE: {
	//	SetWindowPos(hWnd, (HWND)0, 50, 50, 50, 50, SWP_NOSIZE | SWP_DRAWFRAME | SWP_SHOWWINDOW);
		editBox = CreateWindow(L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 15, 275, 20, hWnd, (HMENU)NET_ADDRESS, hInst, 0);
		CreateWindow(L"BUTTON", L"SEND", WS_BORDER | WS_CHILD | WS_VISIBLE, 300, 15, 70, 20, hWnd, (HMENU)SEND_BTN, hInst, 0);
		CreateWindow(L"BUTTON", L"STOP", WS_BORDER | WS_CHILD | WS_VISIBLE, 380, 15, 70, 20, hWnd, (HMENU)1, hInst, 0);
		HWND listViewhwnd = CreateListView(hWnd);
		InitListViewColumns(listViewhwnd);
		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case SEND_BTN: {
			int len = GetWindowTextLength(editBox) + 1;
			TCHAR *buff = new TCHAR[len];
			len = GetWindowText(editBox, buff, len);
			buff[len] = '\0';
			MessageBox(
				NULL,
				buff,
				L"TESTMESSAGE",
				MB_ICONEXCLAMATION | MB_YESNO
			);

			#ifdef UNICODE

			const size_t size = wcslen(buff) * 2 + 2;
			std::vector<char> buffer(size);
			size_t i;
			
			Connect("10.0.0.107", 8080);
			#else
				std::string string(text);
			#endif

			delete[] buff;
			
			break;
		}
		case IDM_ABOUT: {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break; }
		case IDM_EXIT: {
			DestroyWindow(hWnd);
			break; }
		default: {
			return DefWindowProc(hWnd, message, wParam, lParam); }
		}
	}
        break;
	case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
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
typedef struct
{
	WSAOVERLAPPED Overlapped;
	SOCKET Socket;
	WSABUF wsaBuf;
	char Buffer[1024];
	DWORD Flags;
	DWORD BytesSent;
	DWORD BytesToSend;
} PER_IO_DATA, *LPPER_IO_DATA;

static DWORD WINAPI ClientWorkerThread(LPVOID lpParameter)
{
	HANDLE hCompletionPort = (HANDLE)lpParameter;
	DWORD NumBytesRecv = 0;
	ULONG CompletionKey;
	LPPER_IO_DATA PerIoData;

	while (GetQueuedCompletionStatus(hCompletionPort, &NumBytesRecv, &CompletionKey, (LPOVERLAPPED*)&PerIoData, INFINITE))
	{
		if (!PerIoData)
			continue;

		if (NumBytesRecv == 0)
		{
			std::cout << "Server disconnected!\r\n\r\n";
		}
		else
		{
			// use PerIoData->Buffer as needed...
			std::cout << std::string(PerIoData->Buffer, NumBytesRecv);

			PerIoData->wsaBuf.len = sizeof(PerIoData->Buffer);
			PerIoData->Flags = 0;

			if (WSARecv(PerIoData->Socket, &(PerIoData->wsaBuf), 1, &NumBytesRecv, &(PerIoData->Flags), &(PerIoData->Overlapped), NULL) == 0)
				PerIoData->wsaBuf.buf;
				wchar_t filename[4096] = { 0 };
				MultiByteToWideChar(0, 0, PerIoData->wsaBuf.buf, strlen(PerIoData->wsaBuf.buf), filename, strlen(PerIoData->wsaBuf.buf));

				MessageBox(
					NULL,
					filename,
					L"recvbuf",
					MB_ICONEXCLAMATION | MB_YESNO
				);
				continue;

			if (WSAGetLastError() == WSA_IO_PENDING)
				continue;
		}
		
		closesocket(PerIoData->Socket);
		delete PerIoData;
	}

	return 0;
}



void Connect(const std::string strIPAddr, int nPort) {
	WSADATA wsa;

	WSAStartup(MAKEWORD(2, 2), &wsa);
	HANDLE hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	for (DWORD i = 0; i < systemInfo.dwNumberOfProcessors; ++i)
	{
		HANDLE hThread = CreateThread(NULL, 0, ClientWorkerThread, hCompletionPort, 0, NULL);
		CloseHandle(hThread);
	}

	SOCKADDR_IN	SockAddr;
	//SOCKET		clientSocket = INVALID_SOCKET;
	int			nRet = -1;
	int			nLen = -1;
	SOCKET Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	//clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&SockAddr, 0, sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;
	inet_pton(AF_INET, (PCSTR)("10.0.0.107"), &SockAddr.sin_addr.s_addr);
	//SockAddr.sin_addr.s_addr = inet_addr(strIPAddr.c_str());
	SockAddr.sin_port = htons(8080);
	CreateIoCompletionPort((HANDLE)Socket, hCompletionPort, 0, 0);

	nRet = WSAConnect(Socket, (LPSOCKADDR)&SockAddr, sizeof(SockAddr), NULL, NULL, NULL, NULL);

	if (nRet == SOCKET_ERROR &&
		WSAGetLastError() != WSAEWOULDBLOCK)
	{
		std::string error = ErrorCode2Text(WSAGetLastError());
		std::wstring stemp = s2ws(error);
		LPCWSTR result = stemp.c_str();
		MessageBox(
			NULL,
			result,
			L"err",
			MB_ICONEXCLAMATION | MB_YESNO
		);
	}
	for (int i = 0; i <= 4; i++) {
		PER_IO_DATA *pPerIoData = new PER_IO_DATA;
		ZeroMemory(pPerIoData, sizeof(PER_IO_DATA));
		char integer_string[32];
	
		sprintf_s(integer_string, "%d", i);
		char other_string[1024] = ""; 

		if (i == 0) {
			OutputDebugString(L"0");
			strncpy_s(other_string, "GET / HTTP / 1.1\r\n\r\n", sizeof(other_string));
		}
		if (i == 1) {
			OutputDebugString(L"1");
			strncpy_s(other_string, "POST / HTTP / 1.1\r\n\r\n", sizeof(other_string));
		}
		if (i == 2) {
			OutputDebugString(L"2");
			strncpy_s(other_string, "PUT / HTTP / 1.1\r\n\r\n", sizeof(other_string));
		}
		if (i == 3) {
			OutputDebugString(L"3");
			strncpy_s(other_string, "PATCH / HTTP / 1.1\r\n\r\n", sizeof(other_string));
		}
		if (i == 4) {
			OutputDebugString(L"4");

			strncpy_s(other_string, "DELETE / HTTP / 1.1\r\n\r\n", sizeof(other_string));
		}

		strcat_s(other_string, integer_string);
		strcpy_s(pPerIoData->Buffer, other_string);

		pPerIoData->Overlapped.hEvent = WSACreateEvent();
		pPerIoData->Socket = Socket;
		pPerIoData->wsaBuf.buf = pPerIoData->Buffer;
		pPerIoData->wsaBuf.len = strlen(pPerIoData->Buffer);
		pPerIoData->BytesToSend = pPerIoData->wsaBuf.len;

		DWORD dwNumSent;
		
		if (WSASend(pPerIoData->Socket, &(pPerIoData->wsaBuf), 1, &dwNumSent, 0, &(pPerIoData->Overlapped), NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				delete pPerIoData;
			}
		}
	}
}

