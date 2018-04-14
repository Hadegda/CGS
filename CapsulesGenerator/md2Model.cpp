#include "md2Model.h"
#include <fstream>
#include "device.h"

using namespace DirectX;

bool ModelMD2::CreateBuffers(char* buffer) {
	const int indexCount = 3 * num_tris;

	std::vector<md2_vertex> vdata;
	vdata.clear();
	std::vector<UINT32> idata(indexCount);

	int vRegistrationNum = 0;
	std::vector<std::vector<int>> diffrentVertex(num_vertex, (std::vector<int>(num_st, -1)));

	dtriangle_t* triangles = reinterpret_cast<dtriangle_t *>((byte *)buffer);
	for (int i = 0; i < num_tris; i++) {
		for (int j = 0; j < 3; ++j) {
			int iVertex = triangles[i].index_xyz[j];
			int iTexCoord = triangles[i].index_st[j];

			if (diffrentVertex[iVertex][iTexCoord] == -1) {
				diffrentVertex[iVertex][iTexCoord] = vRegistrationNum;
				md2_vertex tmp;
				tmp.pos = iVertex;
				tmp.Tex = XMFLOAT2(0.0f, 0.0f);// texCoord[iTexCoord]; //texCoord
				vdata.push_back(tmp);
				++vRegistrationNum;
			}
			idata[3 * i + j] = diffrentVertex[iVertex][iTexCoord];
		}
	}

	D3D11_BUFFER_DESC desc = {};
	D3D11_SUBRESOURCE_DATA subdata = {};

	//vertex buffer
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(md2_vertex) * vdata.size();
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	subdata.pSysMem = vdata.data();
	if (FAILED(RendererCore::Get()->GetDevice()->CreateBuffer(&desc, &subdata, &pVertices))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "Failed creation vertex buffer for %s \n", name);
		return false;
	}
	CopyNameToDebugObjectName(pVertices);

	//indexBuffer
	indices = indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	subdata.pSysMem = idata.data();
	if (FAILED(RendererCore::Get()->GetDevice()->CreateBuffer(&desc, &subdata, &pIndices))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "Failed creation index buffer for %s \n", name);
		return false;
	}
	CopyNameToDebugObjectName(pIndices);

	//constant buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth = sizeof(cbMD2Model);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;

	if (FAILED(RendererCore::Get()->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &pCBuffer))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "Failed creation constant buffer for %s \n", name);
		return false;
	}
	CopyNameToDebugObjectName(pCBuffer);
	return true;
}

bool ModelMD2::CreateVertCoordTex(char* buffer) {
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = num_vertex;
	tDesc.Height = num_frames;
	tDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.CPUAccessFlags = 0;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.MiscFlags = 0;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	daliasframe_t * frame = reinterpret_cast<daliasframe_t *>((byte *)buffer);
	std::vector<XMUINT4> tdata(num_vertex * num_frames);
	for (int j = 0; j < num_frames; ++j) {
		frame = reinterpret_cast<daliasframe_t *>((byte *)buffer + j * frame_size);

		for (int i = 0; i < num_vertex; i++) {
			tdata[j * num_vertex + i].x = frame->verts[i].v[0];
			tdata[j * num_vertex + i].y = frame->verts[i].v[1];
			tdata[j * num_vertex + i].z = frame->verts[i].v[2];
			tdata[j * num_vertex + i].w = frame->verts[i].lightnormalindex;
		}
	}

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = tdata.data();
	data.SysMemPitch = tDesc.Width * sizeof(XMUINT4);
	data.SysMemSlicePitch = tDesc.Height * tDesc.Width * sizeof(XMUINT4);

	if (FAILED(RendererCore::Get()->GetDevice()->CreateTexture2D(&tDesc, &data, &pTexture))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "CreateVertCoordTex %s: failed create texture model \n", name);
		return false;
	}
	CopyNameToDebugObjectName(pTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = tDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = tDesc.MipLevels;

	if (FAILED(RendererCore::Get()->GetDevice()->CreateShaderResourceView(pTexture, &srvDesc, &pTexSRV))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "CreateVertCoordTex %s: failed create SRV model \n", name);
		return false;
	}
	CopyNameToDebugObjectName(pTexSRV);
	return true;
}

bool ModelMD2::CreateVertScaleTranslateTex(char* buffer) {
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = 2;
	tDesc.Height = num_frames;
	tDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.CPUAccessFlags = 0;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.MiscFlags = 0;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	daliasframe_t * frame = reinterpret_cast<daliasframe_t *>((byte *)buffer);
	std::vector<XMFLOAT3> tdata(2 * num_frames);
	for (int j = 0; j < num_frames; ++j) {
		frame = reinterpret_cast<daliasframe_t *>((byte *)buffer + j * frame_size);

		tdata[j * 2 + 0].x = frame->scale[0];
		tdata[j * 2 + 0].y = frame->scale[1];
		tdata[j * 2 + 0].z = frame->scale[2];

		tdata[j * 2 + 1].x = frame->translate[0];
		tdata[j * 2 + 1].y = frame->translate[1];
		tdata[j * 2 + 1].z = frame->translate[2];
	}

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = tdata.data();
	data.SysMemPitch = tDesc.Width * sizeof(XMFLOAT3);
	data.SysMemSlicePitch = tDesc.Height * tDesc.Width * sizeof(XMFLOAT3);

	if (FAILED(RendererCore::Get()->GetDevice()->CreateTexture2D(&tDesc, &data, &pTextureST))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "CreateVertCoordTex %s: failed create texture model \n", name);
		return false;
	}
	CopyNameToDebugObjectName(pTextureST);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = tDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = tDesc.MipLevels;

	if (FAILED(RendererCore::Get()->GetDevice()->CreateShaderResourceView(pTextureST, &srvDesc, &pTexScaleTranslateSRV))) {
		//RefImport::ConsolePrintf(PRINT_DEVELOPER, "CreateVertCoordTex %s: failed create SRV model \n", name);
		return false;
	}
	CopyNameToDebugObjectName(pTexScaleTranslateSRV);
	return true;
}

bool ModelMD2::LoadModel(wchar_t * path)
{
	std::ifstream file;
	file.open(path, std::ios::in | std::ios::binary);
	if (file.fail())
		return false;

	md2_header header;
	file.read((char *)&header, sizeof(md2_header));

	num_frames = header.num_frames;
	num_vertex = header.num_xyz;
	frame_size = header.framesize;
	num_tris = header.num_tris;
	num_st = header.num_st;

	char* buffer = new char[num_frames * frame_size];
	file.seekg(header.ofs_frames, std::ios::beg);
	file.read((char *)buffer, num_frames * frame_size);
	frames.clear();
	frames.resize(num_frames);
	daliasframe_t* frame;
	for (int i = 0; i < num_frames; i++) {
		frame = reinterpret_cast<daliasframe_t*>((byte *)buffer + i * frame_size);
		frames[i].scale = XMFLOAT3(frame->scale[0], frame->scale[1], frame->scale[2]);
		frames[i].translate = XMFLOAT3(frame->translate[0], frame->translate[1], frame->translate[2]);
		frames[i].name = frame->name;
	}
	delete[] buffer;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	RendererCore::Get()->GetDevice()->CreateSamplerState(&sampDesc, &sampler_state);

	buffer = new char[num_tris * sizeof(dtriangle_t)];
	file.seekg(header.ofs_tris, std::ios::beg);
	file.read(buffer, num_tris * sizeof(dtriangle_t));

	if (!CreateBuffers(buffer)) {
		//model = nullptr;
		return false;
	}
	delete[] buffer;

	buffer = new char[num_frames * frame_size];
	file.seekg(header.ofs_frames, std::ios::beg);
	file.read(buffer, num_frames * frame_size);
	if (!CreateVertCoordTex(buffer)) {
		//model = nullptr;
		return false;
	}
	if (!CreateVertScaleTranslateTex(buffer)) {
		//model = nullptr;
		return false;
	}

	file.close();
	return true;
}

void ModelMD2::DrawModel()
{
	//XMFLOAT3 shadelight = XMFLOAT3(0.0f, 0.0f, 0.0f); ...

	XMFLOAT4 color = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	UINT normalTabIndex = ((int)(45.0f * (SHADEDOT_QUANT / 360.0))) & (SHADEDOT_QUANT - 1);
	FLOAT backlerp = 0.1;

	cbMD2Model cb;
	cb.color = XMFLOAT4(0.1f, 0.3f, 0.1f, 1.0f);
	cb.params = XMFLOAT4((FLOAT)normalTabIndex, (FLOAT)curframe, (FLOAT)oldframe, backlerp);
	RendererCore::Get()->GetDeviceContext()->UpdateSubresource(pCBuffer, 0, nullptr, &cb, 0, 0);
	RendererCore::Get()->GetDeviceContext()->VSSetConstantBuffers(1, 1, &pCBuffer);

	UINT strides[] = { sizeof(md2_vertex) };
	UINT offsets[] = { 0 };
	RendererCore::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertices, strides, offsets);
	RendererCore::Get()->GetDeviceContext()->IASetIndexBuffer(pIndices, DXGI_FORMAT_R32_UINT, 0);

	RendererCore::Get()->GetDeviceContext()->VSSetShaderResources(1, 1, &pTexSRV);
	RendererCore::Get()->GetDeviceContext()->VSSetShaderResources(2, 1, &pTexScaleTranslateSRV);
	RendererCore::Get()->GetDeviceContext()->PSSetSamplers(0, 1, &sampler_state);

	RendererCore::Get()->GetDeviceContext()->DrawIndexed(indices, 0, 0);
}

void ModelMD2::NextFrame()
{
	curframe = (curframe + 1 < num_frames) ? curframe + 1 : 0;
}

void ModelMD2::PrevFrame()
{
	curframe = (curframe > 0) ? curframe - 1 : num_frames - 1;
}

ModelMD2::ModelMD2()
{
	sampler_state = nullptr;

	pVertices = nullptr;
	pIndices = nullptr;
	pCBuffer = nullptr;

	pTexture = nullptr;
	pTexSRV = nullptr;

	pTextureST = nullptr;
	pTexScaleTranslateSRV = nullptr;

	curframe = 0;
	oldframe = 0;
}

ModelMD2::~ModelMD2()
{
	if (pTexScaleTranslateSRV) pTexScaleTranslateSRV->Release();
	if (pTextureST) pTextureST->Release();

	if (pTexSRV) pTexSRV->Release();
	if (pTexture) pTexture->Release();

	if (pVertices) pVertices->Release();
	if (pIndices) pIndices->Release();
	if (pCBuffer) pCBuffer->Release();

	if (sampler_state) sampler_state->Release();
}
