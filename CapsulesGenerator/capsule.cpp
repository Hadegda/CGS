#include "capsule.h"
#include "device.h"
#include <cmath>

#define Float3Length(a) (sqrt(a.x*a.x + a.y*a.y + a.z*a.z))
#define Float3Substruct(a, b) (XMFLOAT3(a.x-b.x, a.y-b.y, a.z-b.z))

using namespace DirectX;

int const Capsule::nVertSeg = 16;
int const Capsule::nUpHalfSeg = 12;

std::vector<float> Capsule::xCoords;
std::vector<float> Capsule::zCoords;
std::vector<float> Capsule::yCoords;
std::vector<float> Capsule::projOnXZ;
std::vector<WORD> Capsule::indices;

bool Capsule::wasStaticInit = false;

Capsule::Capsule(DirectX::XMFLOAT3 color)
{
	pCapsVertices = NULL;
	pCapsIndices = NULL;
	pCapsCBuffer = NULL;

	this->color = color;

	if (!wasStaticInit)
		InitStatic();
}

bool Capsule::Init(XMFLOAT4 p0, XMFLOAT4 p1, XMFLOAT3 color)
{
	this->p0 = p0;
	this->p1 = p1;
	if (color.x >= 0)
		this->color = color;

	XMFLOAT3 vec = Float3Substruct(p1, p0);
	float length = Float3Length(vec);
	float r = p0.w;

	HRESULT hr = S_OK;

	caps_vertex vertices[2 * nVertSeg * nUpHalfSeg + 2];
	for (int i = 0; i < nVertSeg; i++) {
		for (int j = 0; j < nUpHalfSeg; j++) {
			vertices[2 * nUpHalfSeg * i + j] = { XMFLOAT3{ r * xCoords[i] * projOnXZ[nUpHalfSeg - 1 - j], r * zCoords[i] * projOnXZ[nUpHalfSeg - 1 - j], -r * yCoords[nUpHalfSeg - 1 - j] } };
			vertices[2 * nUpHalfSeg * i + nUpHalfSeg + nUpHalfSeg - 1 - j] = { XMFLOAT3{ r * xCoords[i] * projOnXZ[nUpHalfSeg - 1 - j], r * zCoords[i] * projOnXZ[nUpHalfSeg - 1 - j], length + r * yCoords[nUpHalfSeg - 1 - j] } };
		}
	}
	vertices[2 * nVertSeg * nUpHalfSeg] = { XMFLOAT3{ 0, 0, -r } };
	vertices[2 * nVertSeg * nUpHalfSeg + 1] = { XMFLOAT3{ 0, 0, length + r } };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(caps_vertex) * (nVertSeg * nUpHalfSeg * 2 + 2);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = RendererCore::Get()->GetDevice()->CreateBuffer(&bd, &InitData, &pCapsVertices);
	if (FAILED(hr))
		return false;
	CopyNameToDebugObjectName(pCapsVertices);


	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices.data();
	hr = RendererCore::Get()->GetDevice()->CreateBuffer(&bd, &InitData, &pCapsIndices);
	if (FAILED(hr))
		return false;
	CopyNameToDebugObjectName(pCapsIndices);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(cbCapsule);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = RendererCore::Get()->GetDevice()->CreateBuffer(&bd, NULL, &pCapsCBuffer);
	if (FAILED(hr))
		return false;
	CopyNameToDebugObjectName(pCapsCBuffer);

	return true;
}

void Capsule::InitStatic()
{
	//projections for compute vertex buffer
	xCoords.clear();
	zCoords.clear();
	yCoords.clear();
	projOnXZ.clear();

	xCoords.resize(nVertSeg);
	zCoords.resize(nVertSeg);
	yCoords.resize(nUpHalfSeg);
	projOnXZ.resize(nUpHalfSeg);

	float alpha = XM_2PI / nVertSeg;
	for (int i = 0; i < nVertSeg; i++) {
		xCoords[i] = cos(i * alpha);
		zCoords[i] = sin(i * alpha);
	}

	alpha = XM_PIDIV2 / nUpHalfSeg;
	for (int i = 0; i < nUpHalfSeg; i++) {
		yCoords[i] = sin(i * alpha);
		projOnXZ[i] = cos(i * alpha);
	}

	//data for index buffer
	indices.clear();
	indices.resize(3 * 2 * nVertSeg * ((2 * nUpHalfSeg) - 1) + 2 * 3 * nVertSeg);

	int i;
	int shiftForCups = 3 * 2 * nVertSeg * ((2 * nUpHalfSeg) - 1);
	for (i = 0; i < nVertSeg - 1; i++) {
		for (int j = 0; j < (2 * nUpHalfSeg) - 1; j++) {
			indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 0] = (WORD)(j + i * (2 * nUpHalfSeg));
			indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 1] = (WORD)(j + i * (2 * nUpHalfSeg) + 1);
			indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 2] = (WORD)(j + i * (2 * nUpHalfSeg) + (2 * nUpHalfSeg));
			indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 3] = (WORD)(j + i * (2 * nUpHalfSeg) + 1);
			indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 4] = (WORD)(j + i * (2 * nUpHalfSeg) + 1 + (2 * nUpHalfSeg));
			indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 5] = (WORD)(j + i * (2 * nUpHalfSeg) + (2 * nUpHalfSeg));
		}
		//Cups
		indices[shiftForCups + i * 6 + 0] = (WORD)(i * (2 * nUpHalfSeg));
		indices[shiftForCups + i * 6 + 1] = (WORD)(i * (2 * nUpHalfSeg) + (2 * nUpHalfSeg));
		indices[shiftForCups + i * 6 + 2] = (WORD)(2 * nVertSeg * nUpHalfSeg);
										
		indices[shiftForCups + i * 6 + 3] = (WORD)((2 * nUpHalfSeg) - 2 + i * (2 * nUpHalfSeg) + 1);
		indices[shiftForCups + i * 6 + 4] = (WORD)(2 * nVertSeg * nUpHalfSeg + 1);
		indices[shiftForCups + i * 6 + 5] = (WORD)((2 * nUpHalfSeg) - 2 + i * (2 * nUpHalfSeg) + 1 + (2 * nUpHalfSeg));
	}

	for (int j = 0; j < (2 * nUpHalfSeg) - 1; j++) {
		indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 0] = (WORD)(j + i * (2 * nUpHalfSeg));
		indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 1] = (WORD)(j + i * (2 * nUpHalfSeg) + 1);
		indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 2] = (WORD)(j + 0);
		indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 3] = (WORD)(j + i * (2 * nUpHalfSeg) + 1);
		indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 4] = (WORD)(j + 1);
		indices[((2 * nUpHalfSeg) - 1) * 6 * i + j * 6 + 5] = (WORD)(j + 0);
	}
	//Cups
	indices[shiftForCups + i * 6 + 0] = (WORD)(i * (2 * nUpHalfSeg));
	indices[shiftForCups + i * 6 + 1] = (WORD)(0);
	indices[shiftForCups + i * 6 + 2] = (WORD)(2 * nVertSeg * nUpHalfSeg);
									
	indices[shiftForCups + i * 6 + 3] = (WORD)((2 * nUpHalfSeg) - 2 + i * (2 * nUpHalfSeg) + 1);
	indices[shiftForCups + i * 6 + 4] = (WORD)(2 * nVertSeg * nUpHalfSeg + 1);
	indices[shiftForCups + i * 6 + 5] = (WORD)((2 * nUpHalfSeg) - 2 + 1);

	wasStaticInit = true;
}

XMMATRIX Rotation(XMFLOAT3 rotate) {
	if (Float3Length(rotate) != 0) {
		XMVECTOR newZaxis = XMVector4Normalize(XMVectorSet(rotate.x, rotate.y, rotate.z, 0.0f));
		XMVECTOR angleZ = XMVector3AngleBetweenVectors(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), newZaxis);
		float angleZRadians = XMVectorGetX(angleZ);
		XMVECTOR proj;
		XMVECTOR perp;
		XMVector3ComponentsFromNormal(&proj, &perp, newZaxis, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
		XMVECTOR angleY = XMVector3AngleBetweenVectors(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), perp);
		float angleYRadians = XMVectorGetX(angleY);
		if (rotate.y < 0)
			angleYRadians *= -1;

		return XMMatrixRotationY(angleZRadians) * XMMatrixRotationZ(angleYRadians);
	}
	return XMMatrixIdentity();
}

void Capsule::DrawCaps(XMFLOAT4X4 mWorld, float light, float transparency, int nCapsule)
{
	UINT stride = sizeof(caps_vertex);
	UINT offset = 0;
	RendererCore::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &pCapsVertices, &stride, &offset);
	RendererCore::Get()->GetDeviceContext()->IASetIndexBuffer(pCapsIndices, DXGI_FORMAT_R16_UINT, 0);

	cbCapsule cb;
	if(light >= 0.0f)
		cb.color = XMFLOAT4{ color.x*light, color.y*light, color.z*light, 0.01f * transparency };
	else {
		if (nCapsule == 0 || nCapsule == 3)
			cb.color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.01f * transparency);
		if (nCapsule == 1 || nCapsule == 11)
			cb.color = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.01f * transparency);
		if (nCapsule == 2 || nCapsule == 10)
			cb.color = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.01f * transparency);
		if (nCapsule == 4 || nCapsule == 8)
			cb.color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.01f * transparency);
		if (nCapsule == 5 || nCapsule == 7)
			cb.color = XMFLOAT4(1.0f, 0.0f, 1.0f, 0.01f * transparency);
		if (nCapsule == 6 || nCapsule == 13)
			cb.color = XMFLOAT4(0.0f, 1.0f, 1.0f, 0.01f * transparency);
		if (nCapsule == 9 || nCapsule == 12)
			cb.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.01f * transparency);
	}

	XMMATRIX gm = XMLoadFloat4x4(&mWorld);
	XMFLOAT3 rot = Float3Substruct(p1, p0);
	gm = Rotation(rot) * XMMatrixTranslation(p0.x, p0.y, p0.z) * gm;

	cb.mWorld = XMMatrixTranspose(gm);
	RendererCore::Get()->GetDeviceContext()->UpdateSubresource(pCapsCBuffer, 0, NULL, &cb, 0, 0);
	RendererCore::Get()->GetDeviceContext()->VSSetConstantBuffers(1, 1, &pCapsCBuffer);

	RendererCore::Get()->GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
}

#define SphereVolume(r) ((4.0f / 3.0f) * XM_PI *r*r*r)
#define CylinderVolume(r, h) (h * XM_PI *r*r)

float Capsule::GetVolume()
{
	XMFLOAT3 vec = Float3Substruct(p1, p0);
	float length = Float3Length(vec);

	return SphereVolume(p0.w) + CylinderVolume(p0.w, length);
}

Capsule::~Capsule()
{
	if (pCapsCBuffer) pCapsCBuffer->Release();
	if (pCapsIndices) pCapsIndices->Release();
	if (pCapsVertices) pCapsVertices->Release();

}

bool Capsule::CreateBuffers()
{
	return false;
}
