#ifndef _CAPSULE_H_
#define _CAPSULE_H_

#include <windows.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <vector>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

typedef struct {
	DirectX::XMFLOAT3 pos;
} caps_vertex;

struct cbCapsule
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMFLOAT4 color;
};

struct cbCapsule1
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMFLOAT4 color;
};

class Capsule {
public:
	bool Init(float r1/*, float r2*/, float length, DirectX::XMFLOAT3 color = DirectX::XMFLOAT3{-1.0f, -1.0f, -1.0f});
	static void InitStatic();
	void DrawCaps(DirectX::XMFLOAT4X4 mWorld, float light, float transparency, int nCapsule = -1);

	Capsule(DirectX::XMFLOAT3 color);
	~Capsule();


private:
	DirectX::XMFLOAT3 color;
	bool CreateBuffers();

	ID3D11Buffer* pCapsVertices;
	ID3D11Buffer* pCapsIndices;
	ID3D11Buffer* pCapsCBuffer;

	static int const nVertSeg;
	static int const nUpHalfSeg;

	static std::vector<WORD> indices;

	static std::vector<float> xCoords;
	static std::vector<float> zCoords;
	static std::vector<float> yCoords;
	static std::vector<float> projOnXZ;

	static bool wasStaticInit;
};

#endif