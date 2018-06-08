#ifndef _DEVICE_H_
#define _DEVICE_H_

//#define PROFILE_NAME

#include <d3d11_1.h>
#include <dxgi.h>
#include <vector>
#include <memory>
#include "window.h"

#ifdef _DEBUG
#include <DXGIDebug.h>
#endif

#pragma comment(lib, "D3D11.lib")

#include "singleton.h"
#include "renderer.h"

template<size_t length>
__forceinline void SetDebugObjectName(ID3D11DeviceChild* resource, const char(&name)[length]) {
#if defined(_DEBUG) || defined(PROFILE_NAME)
	resource->SetPrivateData(WKPDID_D3DDebugObjectName, length - 1, name);
#endif
}

#define CopyNameToDebugObjectName(resource)  SetDebugObjectName((resource), #resource)
#define CopyResNameToDebugObjectName(resource, name)  SetDebugObjectName((resource), #name)

class RendererCore : public Singleton<RendererCore> {
public:

	bool Init() noexcept;

	ID3D11Device* GetDevice() noexcept;
	ID3D11DeviceContext* GetDeviceContext() noexcept;
	ID3D11RenderTargetView* GetRenderTargetView() noexcept;
	ID3D11DepthStencilView* GetDepthStencilView() noexcept;
	ID3D11DepthStencilView* GetNoDepthStencilView() noexcept;

	ID3D11VertexShader* GetVS() noexcept;
	ID3D11InputLayout * GetVS_Layout() noexcept;
	ID3D11VertexShader* GetVS_CAPS() noexcept;
	ID3D11InputLayout * GetVS_CAPS_Layout() noexcept;
	ID3D11VertexShader* GetVS_GA() noexcept;
	ID3D11InputLayout * GetVS_GA_Layout() noexcept;
	ID3D11PixelShader* GetPS() noexcept;
	ID3D11PixelShader * GetPS_CAPS() noexcept;
	ID3D11PixelShader * GetPS_GA() noexcept;

	void Present(UINT syncInterval, UINT flags) const noexcept;
	float GetScreenWidth() const noexcept;
	float GetScreenHeight() const noexcept;

	Renderer& GetRenderer();

private:
	RendererCore();
	~RendererCore();
	friend class SingletonManager;

	HRESULT CreateDeviceAndContext() noexcept;
	HRESULT CreateSwapChain(HWND hWnd) noexcept;
	HRESULT CreateBackBuffer() noexcept;
	HRESULT CreateDepthBuffer() noexcept;
	HRESULT CreateVertexShader() noexcept;
	HRESULT CreatePixelShader() noexcept;

	/*WARNING DON`T DECLARE ANY MEMBERS BEFORE debug*/
#ifdef _DEBUG
	struct DebugHandler {
		~DebugHandler() {
			HRESULT hr = S_OK;
			if (ptr && FAILED(hr = ptr->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL))) {
				OutputDebugString(L"ReportLiveObjects failed");
			}
		}
		IDXGIDebug * ptr;
	} debug;
#endif

	ID3D11Device* pDevice;
	ID3D11Device1* pDevice1;
	ID3D11DeviceContext* pDeviceContext;
	ID3D11DeviceContext1* pDeviceContext1;
	IDXGISwapChain* pSwapChain;
	IDXGISwapChain1* pSwapChain1;

	ID3D11RenderTargetView* pRenderTargetView;
	ID3D11Texture2D*        pDepthStencil;
	ID3D11DepthStencilView* pDepthStencilView;
	ID3D11DepthStencilView* pNoDepthStencilView;

	ID3D11VertexShader* pVertexShader;
	ID3D11InputLayout* pVertexLayout;
	ID3D11PixelShader* pPixelShader;
	ID3D11VertexShader* pCapsVertexShader;
	ID3D11InputLayout* pCapsVertexLayout;
	ID3D11PixelShader* pCapsPixelShader;
	ID3D11VertexShader* pGAVertexShader;
	ID3D11InputLayout* pGAVertexLayout;
	ID3D11PixelShader* pGAPixelShader;

	Renderer renderer;
	float screenWidth, screenHeight;

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut) noexcept;
};

#endif
