#include "device.h"
#include "window.h"


extern "C" {
	_declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
}

extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

ID3D11Device*  RendererCore::GetDevice() noexcept {
	return pDevice;
}

ID3D11DeviceContext*  RendererCore::GetDeviceContext() noexcept {
	return pDeviceContext;
}

ID3D11RenderTargetView*  RendererCore::GetRenderTargetView() noexcept {
	return pRenderTargetView;
}

ID3D11DepthStencilView * RendererCore::GetDepthStencilView() noexcept
{
	return pDepthStencilView;
}

ID3D11DepthStencilView * RendererCore::GetNoDepthStencilView() noexcept
{
	return pNoDepthStencilView;
}

ID3D11VertexShader * RendererCore::GetVS() noexcept
{
	return pVertexShader;
}

ID3D11InputLayout * RendererCore::GetVS_Layout() noexcept
{
	return pVertexLayout;
}

ID3D11VertexShader * RendererCore::GetVS_CAPS() noexcept
{
	return pCapsVertexShader;
}

ID3D11InputLayout * RendererCore::GetVS_CAPS_Layout() noexcept
{
	return pCapsVertexLayout;
}

ID3D11VertexShader * RendererCore::GetVS_GA() noexcept
{
	return pGAVertexShader;
}

ID3D11InputLayout * RendererCore::GetVS_GA_Layout() noexcept
{
	return pGAVertexLayout;
}

ID3D11PixelShader * RendererCore::GetPS() noexcept
{
	return pPixelShader;
}

ID3D11PixelShader * RendererCore::GetPS_CAPS() noexcept
{
	return pCapsPixelShader;
}

ID3D11PixelShader * RendererCore::GetPS_GA() noexcept
{
	return pGAPixelShader;
}

bool RendererCore::Init() noexcept {
	HRESULT hr = S_OK;

	if (FAILED(hr = CreateDeviceAndContext())) {
		// Log
	}
	if (SUCCEEDED(hr) && FAILED(hr = CreateSwapChain(Window::Get()->hWnd))) {
		// Log
	}

	if (SUCCEEDED(hr) && FAILED(hr = CreateBackBuffer())) {
		// Log
	}

	if (SUCCEEDED(hr) && FAILED(hr = CreateDepthBuffer())) {
		// Log
	}

	if (SUCCEEDED(hr) && FAILED(hr = CreateVertexShader())) {
		// Log
	}

	if (SUCCEEDED(hr) && FAILED(hr = CreatePixelShader())) {
		// Log
	}

	return SUCCEEDED(hr);
}

void RendererCore::Present(UINT syncInterval, UINT flags) const noexcept {
	pSwapChain->Present(syncInterval, flags);
}

float RendererCore::GetScreenWidth() const noexcept {
	return screenWidth;
}

float RendererCore::GetScreenHeight() const noexcept {
	return screenHeight;
}

Renderer & RendererCore::GetRenderer()
{
	return renderer;
}

RendererCore::RendererCore() {
	pDevice = NULL;
	pDevice1 = NULL;
	pDeviceContext = NULL;
	pDeviceContext1 = NULL;
	pSwapChain = NULL;
	pSwapChain1 = NULL;
	pRenderTargetView = NULL;
	pDepthStencil = NULL;
	pVertexShader = NULL;
	pVertexLayout = NULL;
	pPixelShader = NULL;
	pCapsVertexShader = NULL;
	pCapsVertexLayout = NULL;
	pCapsPixelShader = NULL;
	pGAVertexShader = NULL;
	pGAVertexLayout = NULL;
	pGAPixelShader = NULL;
	pDepthStencil = NULL;
	pDepthStencilView = NULL;
	pNoDepthStencilView = NULL;
}

RendererCore::~RendererCore() {
	if (pDeviceContext) pDeviceContext->ClearState();

	if (pGAVertexLayout) pGAVertexLayout->Release();
	if (pGAVertexShader) pGAVertexShader->Release();
	if (pGAPixelShader) pGAPixelShader->Release();
	if (pGAVertexLayout) pCapsVertexLayout->Release();
	if (pCapsVertexShader) pCapsVertexShader->Release();
	if (pCapsPixelShader) pCapsPixelShader->Release();
	if (pVertexLayout) pVertexLayout->Release();
	if (pVertexShader) pVertexShader->Release();
	if (pPixelShader) pPixelShader->Release();
	if (pNoDepthStencilView) pNoDepthStencilView->Release();
	if (pDepthStencilView) pDepthStencilView->Release();
	if (pDepthStencil) pDepthStencil->Release();
	if (pRenderTargetView) pRenderTargetView->Release();
	if (pSwapChain1) pSwapChain1->Release();
	if (pSwapChain) pSwapChain->Release();
	if (pDeviceContext1) pDeviceContext1->Release();
	if (pDeviceContext) pDeviceContext->Release();
	if (pDevice1) pDevice1->Release();
	if (pDevice) pDevice->Release();
}

HRESULT RendererCore::CreateDeviceAndContext() noexcept {
	HRESULT hr = S_OK;

	D3D_DRIVER_TYPE         driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &pDevice, &featureLevel, &pDeviceContext);

		if (hr == E_INVALIDARG)
		{
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			hr = D3D11CreateDevice(NULL, driverType, NULL, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, &pDevice, &featureLevel, &pDeviceContext);
		}

		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

#ifdef _DEBUG
	typedef HRESULT(__stdcall *fPtrDXGIGetDebugInterface)(const IID&, void**);
	HMODULE hMod = GetModuleHandle(L"Dxgidebug.dll");
	fPtrDXGIGetDebugInterface DXGIGetDebugInterface = (fPtrDXGIGetDebugInterface)GetProcAddress(hMod, "DXGIGetDebugInterface");
	if (FAILED(DXGIGetDebugInterface(__uuidof(IDXGIDebug), reinterpret_cast<void**>(&(debug.ptr))))) {
		//continue to execute program
		;// RefImport::ConsolePrintf(PRINT_DEVELOPER, "DirectX debug interface creation failed");
	}
#endif

	return hr;
}

HRESULT RendererCore::CreateSwapChain(HWND hWnd) noexcept {
	HRESULT hr = S_OK;

	// Obtain DXGI factory from device (since we used NULL for pAdapter above)
	IDXGIFactory1* dxgiFactory = NULL;
	{
		IDXGIDevice* dxgiDevice = NULL;
		hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr))
		{
			IDXGIAdapter* adapter = NULL;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr))
			{
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
				adapter->Release();
			}
			dxgiDevice->Release();
		}
	}

	if (FAILED(hr))
		return hr;

	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	IDXGIFactory2* dxgiFactory2 = NULL;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
	if (dxgiFactory2)
	{
		// DirectX 11.1 or later
		hr = pDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&pDevice1));
		if (SUCCEEDED(hr))
		{
			(void)pDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&pDeviceContext1));
		}

		DXGI_SWAP_CHAIN_DESC1 sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Width = width;
		sd.Height = height;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;

		hr = dxgiFactory2->CreateSwapChainForHwnd(pDevice, hWnd, &sd, NULL, NULL, &pSwapChain1);
		if (SUCCEEDED(hr))
		{
			hr = pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&pSwapChain));
		}

		dxgiFactory2->Release();
	}
	else
	{
		// DirectX 11.0 systems
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		hr = dxgiFactory->CreateSwapChain(pDevice, &sd, &pSwapChain);
	}

	// Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
	dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

	dxgiFactory->Release();

	if (FAILED(hr))
		return hr;

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pDeviceContext->RSSetViewports(1, &vp);

	screenWidth = width;
	screenHeight = height;
	return hr;
}

HRESULT RendererCore::CreateBackBuffer() noexcept {
	HRESULT hr = S_OK;

	ID3D11Texture2D* pBackBuffer = NULL;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;

	hr = pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	CopyNameToDebugObjectName(pRenderTargetView);

	return hr;
}

HRESULT RendererCore::CreateDepthBuffer() noexcept
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = screenWidth;
	descDepth.Height = screenHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);
	if (FAILED(hr))
		return hr;
	CopyNameToDebugObjectName(pDepthStencil);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = pDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &pDepthStencilView);
	if (FAILED(hr))
		return hr;
	CopyNameToDebugObjectName(pDepthStencilView);

	descDSV.Flags = D3D11_DSV_READ_ONLY_DEPTH;
	hr = pDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &pNoDepthStencilView);
	if (FAILED(hr))
		return hr;
	CopyNameToDebugObjectName(pNoDepthStencilView);

	return hr;
}

HRESULT RendererCore::CreateVertexShader() noexcept
{
	HRESULT hr = S_OK;

	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(L"shaders.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = pDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	pDeviceContext->IASetInputLayout(pVertexLayout);

	CopyNameToDebugObjectName(pVertexShader);
	CopyNameToDebugObjectName(pVertexLayout);

	hr = CompileShaderFromFile(L"capsShaders.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &pCapsVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC capsLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = ARRAYSIZE(capsLayout);

	// Create the input layout
	hr = pDevice->CreateInputLayout(capsLayout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pCapsVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	CopyNameToDebugObjectName(pCapsVertexShader);
	CopyNameToDebugObjectName(pCapsVertexLayout);

	hr = CompileShaderFromFile(L"gaShaders.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &pGAVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC gaLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "CLASTER", 0, DXGI_FORMAT_R32_UINT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = ARRAYSIZE(gaLayout);

	// Create the input layout
	hr = pDevice->CreateInputLayout(gaLayout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pGAVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;


	CopyNameToDebugObjectName(pGAVertexShader);
	CopyNameToDebugObjectName(pGAVertexLayout);
	return hr;
}

HRESULT RendererCore::CreatePixelShader() noexcept
{
	HRESULT hr = S_OK;

	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"shaders.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pPixelShader);
	pPSBlob->Release();
	CopyNameToDebugObjectName(pPixelShader);

	hr = CompileShaderFromFile(L"capsShaders.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pCapsPixelShader);
	pPSBlob->Release();
	CopyNameToDebugObjectName(pCapsPixelShader);

	hr = CompileShaderFromFile(L"gaShaders.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pGAPixelShader);
	pPSBlob->Release();
	CopyNameToDebugObjectName(pGAPixelShader);
	return hr;
}

HRESULT RendererCore::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut) noexcept
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = NULL;
	hr = D3DCompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}