#include "window.h"

bool Window::Init(HINSTANCE hInstance, WNDPROC wndProc, int nCmdShow) noexcept
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"Capsules generator";
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex))
		return false;

	// Create window
	hInst = hInstance;
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindow(L"Capsules generator", L"Capsules generator", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return false;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	InitMenu();

	return true;
}

#include "device.h"
#include <string>

void Window::Handler(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case 10:
		if (RendererCore::Get()->GetRenderer().GetModel()) {
			if (IDNO == MessageBox(NULL, L"You have open model.\nDo you want to close it and open another?", L"Warning", MB_YESNO))
				break;
		}

		file.lpstrTitle = _T("Open File");
		file.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		if (!GetOpenFileName(&file)) {
			if(CommDlgExtendedError())
				MessageBox(NULL, L"Error open file", L"Error", MB_OK);
			break;
		}
		if (!RendererCore::Get()->GetRenderer().LoadModel(file.lpstrFile))
			MessageBox(NULL, L"Error load model", L"ERROR", MB_OK);
		break;

	case 11:
		if (!RendererCore::Get()->GetRenderer().UnloadModel())
			MessageBox(NULL, L"You have no open models", L"Error", MB_OK);
		break;
	}
}

Window::~Window() {
	if (hWnd) {
		DestroyWindow(hWnd);
	}
	if (hInst) {
		UnregisterClass(L"Capsules generator", hInst);
	}
}

bool Window::InitMenu() noexcept
{
	HMENU hmenu1 = CreateMenu();
	HMENU hPopMenuFile = CreatePopupMenu();

	AppendMenu(hmenu1, MF_STRING, 0, L"&File");
	AppendMenu(hmenu1, MF_STRING | MF_POPUP, (UINT)hPopMenuFile, L"&Model");
	AppendMenu(hmenu1, MF_STRING, 2, L"&Capsule");
	AppendMenu(hmenu1, MF_STRING, 3, L"&Config");
	AppendMenu(hmenu1, MF_STRING, 4, L"&Help");

	AppendMenu(hPopMenuFile, MF_STRING, 10, L"Open");
	AppendMenu(hPopMenuFile, MF_STRING, 11, L"Close");

	SetMenu(hWnd, hmenu1);
	SetMenu(hWnd, hPopMenuFile);

	file.lStructSize = sizeof(OPENFILENAME);
	file.hInstance = hInst;
	file.hwndOwner = hWnd;
	file.lpstrFilter = _T("md2 (*.md2)\0*.md2\0\0");
	file.lpstrFile = name;
	file.nMaxFile = 256;
	file.lpstrInitialDir = _T(".\\");
	return true;
}
