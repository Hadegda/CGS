#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "window.h"
#include "device.h"

using namespace DirectX;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();

float alphaSphereView = 0.0f, betaSphereView = -45.0f, rSphereView = 50.0f;

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Window::Get();
	RendererCore::Get();

	if (!Window::Get()->Init(hInstance, static_cast<WNDPROC>(WndProc), nCmdShow)) {
		// log
		return -1;
	}

	if (!RendererCore::Get()->Init()) {
		//log
		return -1;
	}

	if (!RendererCore::Get()->GetRenderer().Init(alphaSphereView, betaSphereView, rSphereView)) {
		return -1;
	}

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}

	SingletonManager::Shutdown();

	return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static int xPos, yPos, xPos1, yPos1;
	static short wheel;

	switch (message)
	{
	case WM_COMMAND:
		Window::Get()->Handler(message, wParam, lParam);
		break;

	case WM_MOUSEMOVE:
		if (wParam == MK_LBUTTON) {
			xPos1 = ((int)(short)LOWORD(lParam));
			yPos1 = ((int)(short)HIWORD(lParam));
			if (xPos1 > xPos)
			{
				alphaSphereView -= 0.01f;
				if (alphaSphereView < -XM_2PI) alphaSphereView += XM_2PI;
			}
			if (xPos1 < xPos)
			{
				alphaSphereView += 0.01f;
				if (alphaSphereView > XM_2PI) alphaSphereView -= XM_2PI;
			}
			if (yPos1 > yPos && betaSphereView + 0.01f < XM_PIDIV2) betaSphereView += 0.01f;
			if (yPos1 < yPos && betaSphereView - 0.01f > -XM_PIDIV2) betaSphereView -= 0.01f;

			RendererCore::Get()->GetRenderer().ChangeViewMatrix(alphaSphereView, betaSphereView, rSphereView);
			xPos = xPos1;
			yPos = yPos1;
		}
		break;

	case WM_MOUSEWHEEL:
		wheel = GET_WHEEL_DELTA_WPARAM(wParam);

		if (wheel > 0 && rSphereView < 100.0f) rSphereView += 1.0f;
		if (wheel < 0 && rSphereView > 2.0f ) rSphereView -= 1.0f;
		RendererCore::Get()->GetRenderer().ChangeViewMatrix(alphaSphereView, betaSphereView, rSphereView);
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_RIGHT:
			RendererCore::Get()->GetRenderer().GetModel()->NextFrame();
			RendererCore::Get()->GetRenderer().GetGA()->NextFrame();
			break;

		case VK_LEFT:
			RendererCore::Get()->GetRenderer().GetModel()->PrevFrame();
			RendererCore::Get()->GetRenderer().GetGA()->PrevFrame();
			break;

		case 'S':
			RendererCore::Get()->GetRenderer().SetSolidRS();
			RendererCore::Get()->GetRenderer().isWireFrame = false;
			break;

		case 'W':
			RendererCore::Get()->GetRenderer().SetWireFrameRS();
			RendererCore::Get()->GetRenderer().isWireFrame = true;
			break;

		case 'M':
			RendererCore::Get()->GetRenderer().drawModel = !RendererCore::Get()->GetRenderer().drawModel;
			break;

		case 'L':
			RendererCore::Get()->GetRenderer().drawLines = !RendererCore::Get()->GetRenderer().drawLines;
			break;

		case 'R':
			RendererCore::Get()->GetRenderer().drawResSkelet = !RendererCore::Get()->GetRenderer().drawResSkelet;
			break;

		case 'B':
			RendererCore::Get()->GetRenderer().drawBBox = !RendererCore::Get()->GetRenderer().drawBBox;
			break;

		case 'C':
			RendererCore::Get()->GetRenderer().drawSkelet = !RendererCore::Get()->GetRenderer().drawSkelet;
			break;

		case 'P':
			RendererCore::Get()->GetRenderer().isPlaying = !RendererCore::Get()->GetRenderer().isPlaying;
			break;

		case 'O':
			RendererCore::Get()->GetRenderer().GetGA()->OptimizeAll();
			break;

		case VK_SPACE :
			RendererCore::Get()->GetRenderer().GetGA()->Optimize();
			break;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// Note that this tutorial does not handle resizing (WM_SIZE) requests,
		// so we created the window without the resize border.

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


//--------------------------------------------------------------------------------------
// Render the frame
//--------------------------------------------------------------------------------------
void Render()
{

	RendererCore::Get()->GetRenderer().Draw();
	
	RendererCore::Get()->Present(0, 0);
}
