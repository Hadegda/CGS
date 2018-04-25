#ifndef _CAPSULE_H_
#define _CAPSULE_H_

#include <vector>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#define Float3Length(a) (sqrt(a.x*a.x + a.y*a.y + a.z*a.z))
#define Float3DotProduct(a, b) (a.x*b.x + a.y*b.y + a.z*b.z)
#define Float3Substruct(a, b) (XMFLOAT3(a.x-b.x, a.y-b.y, a.z-b.z))
#define Float3Sum(a, b) (XMFLOAT3(a.x+b.x, a.y+b.y, a.z+b.z))
#define Float3ScalarMult(a, c) (XMFLOAT3(c*a.x, c*a.y, c*a.z))

typedef struct {
	DirectX::XMFLOAT3 pos;
} caps_vertex;

struct cbCapsule
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMFLOAT4 color;
};

class Capsule {
public:
	//bool Init(float r1/*, float r2*/, float length, DirectX::XMFLOAT3 color = DirectX::XMFLOAT3{-1.0f, -1.0f, -1.0f});
	bool Init(DirectX::XMFLOAT4 p0, DirectX::XMFLOAT4 p1, DirectX::XMFLOAT3 color = DirectX::XMFLOAT3{ -1.0f, 0.0f, 0.0f });
	static void InitStatic();
	void DrawCaps(DirectX::XMFLOAT4X4 mWorld, float light, float transparency, int nCapsule = -1);

	float GetVolume();
	float DistanceToPoint(DirectX::XMFLOAT3 p);
	bool OptimizeForPointSet(std::vector<DirectX::XMFLOAT3> points);

	Capsule(DirectX::XMFLOAT3 color);
	~Capsule();

	DirectX::XMFLOAT4 p0;  //start point; radius
	DirectX::XMFLOAT4 p1;  //finish point; radius

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