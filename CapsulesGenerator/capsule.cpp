#include "capsule.h"
#include "device.h"
#include <cmath>

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

Capsule::Capsule(Capsule * cap, DirectX::XMFLOAT3 color)
{
	pCapsVertices = NULL;
	pCapsIndices = NULL;
	pCapsCBuffer = NULL;

	Init(cap->p0, cap->p1, XMFLOAT3(-1.0f, 0.0f, 0.0f));
}

bool Capsule::Init(XMFLOAT4 p0, XMFLOAT4 p1, XMFLOAT3 color)
{
	this->p0 = p0;
	this->p1 = p1;
	if (color.x >= 0)
		this->color = color;

	XMFLOAT3 dir = Float3Substruct(p1, p0);
	float length = Float3Length(dir);
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

bool Capsule::Reshape()
{
	
	XMFLOAT3 dir = Float3Substruct(p1, p0);
	float length = Float3Length(dir);
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

	if (pCapsVertices) pCapsVertices->Release();
	hr = RendererCore::Get()->GetDevice()->CreateBuffer(&bd, &InitData, &pCapsVertices);
	if (FAILED(hr))
		return false;
	CopyNameToDebugObjectName(pCapsVertices);

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
	XMFLOAT3 dir = Float3Substruct(p1, p0);
	float length = Float3Length(dir);

	return SphereVolume(p0.w) + CylinderVolume(p0.w, length);
}

float Capsule::DistanceToPoint(DirectX::XMFLOAT3 p)
{
	XMFLOAT3 dir = Float3Substruct(p1, p0);
	XMFLOAT3 vec = Float3Substruct(p, p0);

	float t = Float3DotProduct(vec, dir) / Float3DotProduct(dir, dir);
	t = (t < 0) ? 0 : ((t > 1) ? 1 : t);
	return sqrt((-vec.x + dir.x*t)*(-vec.x + dir.x*t) + (-vec.y + dir.y*t)*(-vec.y + dir.y*t) + (-vec.z + dir.z*t)*(-vec.z + dir.z*t)) - p0.w;
}

float Capsule::DistanceFromAxisToPoint(DirectX::XMFLOAT3 p)
{
	XMFLOAT3 dir = Float3Substruct(p1, p0);
	XMFLOAT3 vec = Float3Substruct(p, p0);

	float t = Float3DotProduct(vec, dir) / Float3DotProduct(dir, dir);
	return sqrt((-vec.x + dir.x*t)*(-vec.x + dir.x*t) + (-vec.y + dir.y*t)*(-vec.y + dir.y*t) + (-vec.z + dir.z*t)*(-vec.z + dir.z*t)) - p0.w;
}

float Capsule::AxialDistanceToPoint(DirectX::XMFLOAT3 p)
{
	XMFLOAT3 dir = Float3Substruct(p1, p0);
	XMFLOAT3 vec = Float3Substruct(p, p0);

	return Float3DotProduct(vec, dir) / sqrt(Float3DotProduct(dir, dir));
}

void Capsule::ToSphere(XMFLOAT3 centre, float radius) {
	p0 = XMFLOAT4(centre.x, centre.y, centre.z, radius);
	p1 = XMFLOAT4(centre.x, centre.y, centre.z, radius);

	Reshape();
}

bool Capsule::OptimizeBy2PointsAndSet(DirectX::XMFLOAT3 c0, DirectX::XMFLOAT3 c1, std::vector<DirectX::XMFLOAT3> points)
{
	p0 = XMFLOAT4(c0.x, c0.y, c0.z, 0.0f);
	p1 = XMFLOAT4(c1.x, c1.y, c1.z, 0.0f);

	float dist = 0.0f;
	for (int i = 0; i < points.size(); i++) {
		dist += DistanceToPoint(points[i]);
	}
	dist = dist / points.size();

	p0.w = p1.w = dist;
	Reshape();
	return true;
}

bool Capsule::OptimizeBy2PointsAnd2Sets(DirectX::XMFLOAT3 c0, DirectX::XMFLOAT3 c1, std::vector<DirectX::XMFLOAT3> points, std::vector<DirectX::XMFLOAT3> mainPoints)
{
	p0 = XMFLOAT4(c0.x, c0.y, c0.z, 0.0f);
	p1 = XMFLOAT4(c1.x, c1.y, c1.z, 0.0f);

	float dist = 0.0f;
	for (int i = 0; i < points.size(); i++) {
		dist += DistanceFromAxisToPoint(points[i]);
	}
	for (int i = 0; i < mainPoints.size(); i++) {
		//dist += 3.0f * AxisDistanceToPoint(mainPoints[i]);
		dist += ((float)points.size() / mainPoints.size()) * DistanceFromAxisToPoint(mainPoints[i]);
	}
	dist = dist / (1.5f*points.size()/* + mainPoints.size()*/);

	XMFLOAT3 dir = Float3Substruct(p1, p0);
	dir = Float3ScalarMult(dir, (1.0f/Float3Length(dir))*0.5f*dist);
	XMFLOAT3 vec = Float3Sum(p0, dir);
	p0 = XMFLOAT4(vec.x, vec.y, vec.z, dist);
	vec = Float3Substruct(p1, dir);
	p1 = XMFLOAT4(vec.x, vec.y, vec.z, dist);

	p0.w = p1.w = dist;
	Reshape();
	return true;
}

bool Capsule::OptimizeByPointAndSet(DirectX::XMFLOAT3 c0, std::vector<DirectX::XMFLOAT3> points)
{
	p0 = XMFLOAT4(c0.x, c0.y, c0.z, 0.0f);

	XMFLOAT3 p = XMFLOAT3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < points.size(); i++) {
		p = Float3Sum(p, points[i]);
	}
	p = Float3ScalarMult(p, 1.0f / points.size());
	p1 = XMFLOAT4(p.x, p.y, p.z, 0.0f);

	float dist = 0.0f;
	float maxAxialDistanse = 0;
	float axialDistanse = 0;
	for (int i = 0; i < points.size(); i++) {
		dist += DistanceFromAxisToPoint(points[i]);
		axialDistanse = AxialDistanceToPoint(points[i]);
		if (axialDistanse > maxAxialDistanse)
			maxAxialDistanse = axialDistanse;
	}
	dist = dist / points.size();

	XMFLOAT3 dir = Float3Substruct(p1, p0);
	dir = Float3ScalarMult(dir, (1.0f / Float3Length(dir)));
	XMFLOAT3 vec = Float3Sum(p0, Float3ScalarMult(dir, (maxAxialDistanse - 0.5f*dist)));
	p1 = XMFLOAT4(vec.x, vec.y, vec.z, dist);

	p0.w = p1.w = dist;
	Reshape();
	return true;
}

bool Capsule::ShiftToPoint(DirectX::XMFLOAT4 p) {
	float r = p0.w;
	XMFLOAT3 dir = Float3Substruct(p, p0);

	XMFLOAT3 vec = Float3Sum(p0, dir);
	p0 = XMFLOAT4(vec.x, vec.y, vec.z, r);

	vec = Float3Sum(p1, dir);
	p1 = XMFLOAT4(vec.x, vec.y, vec.z, r);
	Reshape();
	return true;
}

bool Capsule::OptimizeForPointSet(std::vector<DirectX::XMFLOAT3> points)
{
	XMFLOAT3 m = XMFLOAT3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < points.size(); i++) {
		m = Float3Sum(m, points[i]);
	}
	m = Float3ScalarMult(m, 1.0f / points.size());

	//correlation matrix
	float c11 = 0.0f, c22 = 0.0f, c33 = 0.0f, c12 = 0.0f, c23 = 0.0f, c13 = 0.0f;
	for (int i = 0; i < points.size(); i++) {
		c11 += (points[i].x - m.x)*(points[i].x - m.x);
		c22 += (points[i].y - m.y)*(points[i].y - m.y);
		c33 += (points[i].z - m.z)*(points[i].z - m.z);

		c12 += (points[i].x - m.x)*(points[i].y - m.y);
		c13 += (points[i].x - m.x)*(points[i].z - m.z);
		c23 += (points[i].y - m.y)*(points[i].z - m.z);
	}
	c11 /= points.size();
	c22 /= points.size();
	c33 /= points.size();
			 
	c12 /= points.size();
	c13 /= points.size();
	c23 /= points.size();

	//eigen values
	float eig1, eig2, eig3;
	float P1 = c12*c12 + c13*c13 + c23*c23;
	if (P1 == 0) { //diagonal.
		eig1 = c11;
		eig2 = c22;
		eig3 = c33;
	}
	else {
		float q = (c11 + c22 + c33) / 3.0f;
		float p2 = (c11 - q)*(c11 - q) + (c22 - q)*(c22 - q) + (c33 - q)*(c33 - q) + 2.0f * P1;
		float p = sqrt(p2 / 6.0f);
		XMMATRIX B = XMMatrixSet((1 / p) *(c11 - q), (1 / p) *c12, (1 / p) *c13, 0.0f, (1 / p) *c12, (1 / p) *(c22 - q), (1 / p) *c23, 0.0f, (1 / p) *c13, (1 / p) *c23, (1 / p) *(c33 - q), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		//B = (1 / p) * (A - q * E); // E is the identity matrix
		float r = XMVectorGetX(XMMatrixDeterminant(B)) / 2.0f;

			// In exact arithmetic for a symmetric matrix - 1 <= r <= 1
			// but computation error can leave it slightly outside this range.
		float phi;
		if (r <= -1.0f)
			phi = XM_PI / 3.0f;
		else
			if (r >= 1.0f)
				phi = 0.0f;
			else
				phi = acos(r) / 3.0f;

				// the eigenvalues satisfy eig3 <= eig2 <= eig1
		eig1 = q + 2.0f * p * cos(phi);
		eig3 = q + 2.0f * p * cos(phi + (2.0f * XM_PI / 3.0f));
		eig2 = 3.0f * q - eig1 - eig3;     // since trace(A) = eig1 + eig2 + eig3
	}
	float denum = (c11 - eig1)*(c22 - eig1) - c12*c12;
	XMVECTOR eigVec1 = XMVector3Normalize(XMVectorSet((c12*c23 - c13*(c22 - eig1)) / denum, (c12*c13 - c23*(c11 - eig1)) / denum, 1.0f, 0.0f));
	denum = (c11 - eig2)*(c22 - eig2) - c12*c12;
	XMVECTOR eigVec2 = XMVector3Normalize(XMVectorSet((c12*c23 - c13*(c22 - eig2)) / denum, (c12*c13 - c23*(c11 - eig2)) / denum, 1.0f, 0.0f));
	denum = (c11 - eig3)*(c22 - eig3) - c12*c12;
	XMVECTOR eigVec3 = XMVector3Normalize(XMVectorSet((c12*c23 - c13*(c22 - eig3)) / denum, (c12*c13 - c23*(c11 - eig3)) / denum, 1.0f, 0.0f));
	XMVECTOR Vec4 = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX transformBehind = XMMATRIX(eigVec1, eigVec2, eigVec3, Vec4);
	XMMATRIX transformToNew = XMMatrixTranspose(transformBehind);

	std::vector<XMVECTOR> newPoints(points.size());
	float y = 0.0f, z = 0.0f;
	float xMin = 0.0f, xMax = 0.0f;
	for (int i = 0; i < points.size(); i++) {
		newPoints[i] = XMVector3TransformCoord(XMVectorSet(points[i].x, points[i].y, points[i].z, 1.0f), transformToNew);
		y += XMVectorGetY(newPoints[i]);
		z += XMVectorGetZ(newPoints[i]);
		if (xMin == xMax && xMax == 0.0f) {
			xMin = xMax =  XMVectorGetX(newPoints[i]);
		}
		else {
			xMin = (xMin > XMVectorGetX(newPoints[i])) ? XMVectorGetX(newPoints[i]) : xMin;
			xMax = (xMax < XMVectorGetX(newPoints[i])) ? XMVectorGetX(newPoints[i]) : xMax;
		}
	}
	y /= points.size();
	z /= points.size();



	XMVECTOR V1 = XMVector3TransformCoord(XMVectorSet(xMin, y, z, 1.0f), transformBehind);
	XMVECTOR V2 = XMVector3TransformCoord(XMVectorSet(xMax, y, z, 1.0f), transformBehind);

	XMStoreFloat4(&p0, V1);
	p0.w = 1.0f;
	XMStoreFloat4(&p1, V2);
	p1.w = 1.0f;

	Reshape();
	return true;
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
