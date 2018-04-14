#ifndef _WINDOW_H_
#define _WINDOW_H_


#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include <tchar.h>

#include "singleton.h"

class Window : public Singleton<Window> {
public:

	bool Init(HINSTANCE hInstance, WNDPROC wndProc, int nCmdShow) noexcept;
	void Handler(UINT message, WPARAM wParam, LPARAM lParam);
	//bool Resize() noexcept;

	HWND hWnd;
	OPENFILENAME file;
private:
	Window() = default;
	~Window();
	friend class SingletonManager;

	bool InitMenu() noexcept;

	wchar_t name[256];
	HINSTANCE hInst;
};
#endif