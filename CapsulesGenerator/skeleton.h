#ifndef _SKELETON_H_
#define _SKELETON_H_

#include <windows.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <vector>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "capsule.h"
#include "personPattern.h"

class Skeleton {
public:
	bool CreateFromParents(Skeleton* parent0, Skeleton* parent1, float impact0, float impact1);
	void Mutation(int level, int n = 0);

	void DistributeVertices();

	void DrawSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency);

	void UpdateForNewParameters();
	void UpdateOneCapsule(DirectX::XMFLOAT4* cCur, DirectX::XMFLOAT4 cPrev, int nFirstParam, bool isShiftInterpret);

	float GetVolume();

	Skeleton(PersonPattern* homo, DirectX::XMFLOAT3 color);
	~Skeleton();

	std::vector<float> parameters;

private:
	void CorrectionOnRadius();
	void CorrectionOnMaxShift();

	void MutationAll();
	void OneGenMutation(int n);
	void OneCapsuleMutation(int n);

	DirectX::XMFLOAT3 color;
	std::vector<Capsule*> caps;
};

#endif