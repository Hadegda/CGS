#include "device.h"
#include "renderer.h"


using namespace DirectX;

void Renderer::SetSolidRS()
{
	RendererCore::Get()->GetDeviceContext()->RSSetState(pSolidRS);
}

void Renderer::SetWireFrameRS() {
	RendererCore::Get()->GetDeviceContext()->RSSetState(pWireFrameRS);
}

void Renderer::ChangeViewMatrix(float alpha, float beta, float r)
{
	float xDecartView = r * (float)cos(beta) * (float)sin(alpha);
	float yDecartView = r * (float)cos(beta) * (float)cos(alpha);
	float zDecartView = r * (float)sin(beta);

	XMVECTOR Eye = XMVectorSet(xDecartView, yDecartView, zDecartView, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&view, XMMatrixLookAtLH(Eye, At, Up));

	XMFLOAT4X4 viewProj;
	XMStoreFloat4x4(&viewProj, XMMatrixMultiply(XMLoadFloat4x4(&view), XMLoadFloat4x4(&projection)));
	XMFLOAT4X4 transform;
	XMStoreFloat4x4(&transform, XMMatrixTranspose(XMMatrixMultiply(XMLoadFloat4x4(&world), XMLoadFloat4x4(&viewProj))));

	cbTransform cb;
	cb.mTransform = XMLoadFloat4x4(&transform);
	cb.mViewProj = XMMatrixTranspose(XMLoadFloat4x4(&viewProj));
	RendererCore::Get()->GetDeviceContext()->UpdateSubresource(pTransformCB, 0, nullptr, &cb, 0, 0);
	RendererCore::Get()->GetDeviceContext()->VSSetConstantBuffers(0, 1, &pTransformCB);
}

Renderer::Renderer() {
	model = nullptr;
	ga = nullptr;

	pWireFrameRS = NULL;
	pSolidRS = NULL;
	pAlphaBS = NULL;

	pNormTabTexSRV = NULL;
	pNormsTexSRV = NULL;
	pTexture = NULL;
	pnTexture = NULL;

	pTransformCB = NULL;

	ClearColor[0] = ClearColor[1] = ClearColor[2] = ClearColor[3] = 1.0f;

	drawModel = false;
	drawSkelet = false;
	drawLines = true;
	drawBBox = false;
	drawResSkelet = true;
	isWireFrame = true;
	isPlaying = false;
}

Renderer::~Renderer() {
	if (model) delete model;
	if (ga) delete ga;

	if (pTransformCB) pTransformCB->Release();

	if (pNormTabTexSRV) pNormTabTexSRV->Release();
	if (pnTexture) pnTexture->Release();
	if (pNormsTexSRV) pNormsTexSRV->Release();
	if (pTexture) pTexture->Release();

	if (pAlphaBS) pAlphaBS->Release();
	if (pSolidRS) pSolidRS->Release();
	if (pWireFrameRS) pWireFrameRS->Release();
}

bool Renderer::Init(float alpha, float beta, float r)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth = sizeof(cbTransform);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;

	if (FAILED(RendererCore::Get()->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &pTransformCB))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "Failed creation constant buffer for %s \n", name);
		return false;
	}
	CopyNameToDebugObjectName(pTransformCB);

	// Initialize the world matrix
	XMStoreFloat4x4(&world, XMMatrixIdentity());

	// Initialize the view matrix
	ChangeViewMatrix(alpha, beta, r);

	// Initialize the projection matrix
	XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, RendererCore::Get()->GetScreenWidth() / RendererCore::Get()->GetScreenHeight(), 0.01f, 100.0f));

	XMStoreFloat4x4(&world, XMMatrixMultiply(XMLoadFloat4x4(&world), XMMatrixScaling(-1.0f, 1.0f, 1.0f)));
	XMStoreFloat4x4(&world, XMMatrixMultiply(XMLoadFloat4x4(&world), XMMatrixRotationZ(-XM_PIDIV2)));
	XMStoreFloat4x4(&world, XMMatrixMultiply(XMLoadFloat4x4(&world), XMMatrixRotationX(-XM_PIDIV2)));

	XMFLOAT4X4 viewProj;
	XMStoreFloat4x4(&viewProj, XMMatrixMultiply(XMLoadFloat4x4(&view), XMLoadFloat4x4(&projection)));
	XMFLOAT4X4 transform;
	XMStoreFloat4x4(&transform, XMMatrixTranspose(XMMatrixMultiply(XMLoadFloat4x4(&world), XMLoadFloat4x4(&viewProj))));

	cbTransform cb;
	cb.mTransform = XMLoadFloat4x4(&transform);
	cb.mViewProj = XMMatrixTranspose(XMLoadFloat4x4(&viewProj));
	RendererCore::Get()->GetDeviceContext()->UpdateSubresource(pTransformCB, 0, nullptr, &cb, 0, 0);
	RendererCore::Get()->GetDeviceContext()->VSSetConstantBuffers(0, 1, &pTransformCB);

	D3D11_BLEND_DESC blendDesk;
	ZeroMemory(&blendDesk, sizeof(blendDesk));
	blendDesk.AlphaToCoverageEnable = false;
	blendDesk.IndependentBlendEnable = false;

	blendDesk.RenderTarget[0].BlendEnable = true;
	blendDesk.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesk.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesk.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesk.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesk.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesk.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesk.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(RendererCore::Get()->GetDevice()->CreateBlendState(&blendDesk, &pAlphaBS))) {
		return false;
	}
	CopyNameToDebugObjectName(pAlphaBS);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthClipEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	if (FAILED(RendererCore::Get()->GetDevice()->CreateRasterizerState(&rasterizerDesc, &pWireFrameRS))) {
		return false;
	}
	CopyNameToDebugObjectName(pWireFrameRS);

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	if (FAILED(RendererCore::Get()->GetDevice()->CreateRasterizerState(&rasterizerDesc, &pSolidRS))) {
		return false;
	}
	CopyNameToDebugObjectName(pSolidRS);

	RendererCore::Get()->GetDeviceContext()->RSSetState(pWireFrameRS);

	CreateModelsNormals();
	//LoadModel(L"C:\\Users\\Надежда\\Documents\\suber\\baseq2\\pak0\\models\\items\\healing\\stimpack\\tris.md2");
	LoadModel(L"C:\\Users\\Надежда\\Documents\\suber\\baseq2\\pak0\\models\\monsters\\berserk\\tris.md2");

	return true;
}

void Renderer::Draw() {

	if (isPlaying) {
		ULONGLONG timeCur = GetTickCount64();
		if (timeCur % 8 == 0)
			ga->Optimize();
	}

	RendererCore::Get()->GetDeviceContext()->ClearRenderTargetView(RendererCore::Get()->GetRenderTargetView(), ClearColor);
	RendererCore::Get()->GetDeviceContext()->ClearDepthStencilView(RendererCore::Get()->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);


	auto pRenderTargetView = RendererCore::Get()->GetRenderTargetView();
	RendererCore::Get()->GetDeviceContext()->OMSetRenderTargets(1, &pRenderTargetView, RendererCore::Get()->GetDepthStencilView());

	RendererCore::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	RendererCore::Get()->GetDeviceContext()->OMSetBlendState(nullptr, 0, 0xffffffff);

	RendererCore::Get()->GetDeviceContext()->IASetInputLayout(RendererCore::Get()->GetVS_Layout());
	RendererCore::Get()->GetDeviceContext()->VSSetShader(RendererCore::Get()->GetVS(), NULL, 0);
	RendererCore::Get()->GetDeviceContext()->PSSetShader(RendererCore::Get()->GetPS(), NULL, 0);

	RendererCore::Get()->GetDeviceContext()->VSSetShaderResources(3, 1, &pNormTabTexSRV);
	RendererCore::Get()->GetDeviceContext()->VSSetShaderResources(4, 1, &pNormsTexSRV);

	if(model && drawModel)
		model->DrawModel();

	//RendererCore::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	RendererCore::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	RendererCore::Get()->GetDeviceContext()->IASetInputLayout(RendererCore::Get()->GetVS_GA_Layout());
	RendererCore::Get()->GetDeviceContext()->VSSetShader(RendererCore::Get()->GetVS_GA(), NULL, 0);
	RendererCore::Get()->GetDeviceContext()->PSSetShader(RendererCore::Get()->GetPS_GA(), NULL, 0);
	
	if (model && drawLines)
		ga->DrawLineModel();

	RendererCore::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	

	RendererCore::Get()->GetDeviceContext()->OMSetBlendState(pAlphaBS, 0, 0xffffffff);
	RendererCore::Get()->GetDeviceContext()->OMSetRenderTargets(1, &pRenderTargetView, RendererCore::Get()->GetNoDepthStencilView());

	
	if (model && drawBBox) {
		RendererCore::Get()->GetRenderer().SetSolidRS();
		ga->DrawModelBBox();
		RendererCore::Get()->GetRenderer().SetWireFrameRS();
		ga->DrawModelBBox();
		if(!isWireFrame)
			RendererCore::Get()->GetRenderer().SetSolidRS();
	}

	RendererCore::Get()->GetDeviceContext()->IASetInputLayout(RendererCore::Get()->GetVS_CAPS_Layout());
	RendererCore::Get()->GetDeviceContext()->VSSetShader(RendererCore::Get()->GetVS_CAPS(), NULL, 0);
	RendererCore::Get()->GetDeviceContext()->PSSetShader(RendererCore::Get()->GetPS_CAPS(), NULL, 0);

	if (model && drawSkelet)
		ga->DrawSkeleton(world, 1.0f, 1.0f);

	if (model && drawResSkelet)
		ga->DrawBestSkeleton(world, 1.0f, 1.0f);
	return;
}

bool Renderer::LoadModel(wchar_t* path) noexcept
{
	if (model)
		UnloadModel();
	model = new ModelMD2;
	ga = new GeneticAlgorithm();
	if (!model->LoadModel(path))
		return false;
	if (!ga->InitGeneticAlgorithm(path)) {
		UnloadModel();
		return false;
	}
	return true;
}

bool Renderer::UnloadModel() noexcept
{
	if (!model)
		return false;
	delete model;
	delete ga;
	ga = nullptr;
	model = nullptr;
	return true;
}

ModelMD2 * Renderer::GetModel()
{
	return model;
}

GeneticAlgorithm * Renderer::GetGA()
{
	return ga;
}

bool Renderer::CreateModelsNormals() {
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = SHADEDOT_QUANT;
	tDesc.Height = 256;
	tDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.CPUAccessFlags = 0;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.MiscFlags = 0;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// precalculated dot product results
	float	r_avertexnormal_dots[SHADEDOT_QUANT][256] =
#include "anormtab.h"
		;

	std::vector<FLOAT> tdata(tDesc.Width * tDesc.Height);
	for (int j = 0; j < tDesc.Height; ++j) {
		for (int i = 0; i < tDesc.Width; i++) {
			tdata[j * tDesc.Width + i] = r_avertexnormal_dots[i][j];
		}
	}

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = tdata.data();
	data.SysMemPitch = tDesc.Width * sizeof(FLOAT);
	data.SysMemSlicePitch = tDesc.Height * tDesc.Width * sizeof(FLOAT);

	if (FAILED(RendererCore::Get()->GetDevice()->CreateTexture2D(&tDesc, &data, &pTexture))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "CreateNormalsTex: failed create texture with normals for alias models \n");
		return false;
	}
	CopyNameToDebugObjectName(pTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = tDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = tDesc.MipLevels;

	if (FAILED(RendererCore::Get()->GetDevice()->CreateShaderResourceView(pTexture, &srvDesc, &pNormTabTexSRV))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "CreateNormalsTex: failed create SRV normals far alias models \n");
		return false;
	}
	CopyNameToDebugObjectName(pNormTabTexSRV);

	D3D11_TEXTURE2D_DESC tnDesc = {};
	tnDesc.Width = NUMVERTEXNORMALS;
	tnDesc.Height = 3;
	tnDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tnDesc.MipLevels = 1;
	tnDesc.ArraySize = 1;
	tnDesc.Usage = D3D11_USAGE_DEFAULT;
	tnDesc.CPUAccessFlags = 0;
	tnDesc.SampleDesc.Count = 1;
	tnDesc.SampleDesc.Quality = 0;
	tnDesc.MiscFlags = 0;
	tnDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// precalculated normal vectors
	float	r_avertexnormals[NUMVERTEXNORMALS][3] = {
#include "anorms.h"
	};

	std::vector<FLOAT> tndata(tnDesc.Width * tnDesc.Height);
	for (int j = 0; j < tnDesc.Height; ++j) {
		for (int i = 0; i < tnDesc.Width; i++) {
			tndata[j * tnDesc.Width + i] = r_avertexnormals[i][j];
		}
	}

	D3D11_SUBRESOURCE_DATA ndata;
	ndata.pSysMem = tndata.data();
	ndata.SysMemPitch = tnDesc.Width * sizeof(FLOAT);
	ndata.SysMemSlicePitch = tnDesc.Height * tnDesc.Width * sizeof(FLOAT);

	if (FAILED(RendererCore::Get()->GetDevice()->CreateTexture2D(&tnDesc, &ndata, &pnTexture))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "CreateNormalsTex: failed create texture with normals for alias models \n");
		return false;
	}
	CopyNameToDebugObjectName(pnTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvnDesc;
	srvnDesc.Format = tnDesc.Format;
	srvnDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvnDesc.Texture2D.MostDetailedMip = 0;
	srvnDesc.Texture2D.MipLevels = tnDesc.MipLevels;

	if (FAILED(RendererCore::Get()->GetDevice()->CreateShaderResourceView(pnTexture, &srvnDesc, &pNormsTexSRV))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "CreateNormalsTex: failed create SRV normals far alias models \n");
		return false;
	}
	CopyNameToDebugObjectName(pNormsTexSRV);
	return true;
}
