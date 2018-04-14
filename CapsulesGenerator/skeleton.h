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

struct VertInCaps {
	DirectX::XMFLOAT3 pos;
	std::vector<UINT> clasters;
};

struct PartOfBody {
	DirectX::XMFLOAT4 shift; //shift from root body part (shift direction {x, y, z}, shift magnitude {w})
	DirectX::XMFLOAT4 partOfBody; //direction {x, y, z},  magnitude {w}
	float radius;
	DirectX::XMFLOAT3 color;
};

class Skeleton {
public:
	bool CreateStandart();
	bool CreateFromParents(Skeleton* parent0, Skeleton* parent1, float impact0, float impact1);

	void Mutation(int level, int n = 0);

	void Scale(float scale = 1.0f);
	void Centered(DirectX::XMFLOAT3 center = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });

	void DropForward();
	void DropBack();
	void DropLeft();
	void DropRight();
	void InvertY();
	void CreateCaps();
	void DistributeVertices();

	void DrawSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency);
	void CapsulesInObjSys(DirectX::XMFLOAT4 **capsules);

	float GetVolume();

	Skeleton(DirectX::XMFLOAT3 color);
	~Skeleton();

	float** parameters;

private:
	void CorrectionOnRadius();
	void CorrectionOnMaxShift();

	void MutationAll();
	void OneGenMutation(int n);
	void OneCapsuleMutation(int n);

	DirectX::XMFLOAT3 color;

	DirectX::XMFLOAT3 shift;

	PartOfBody torso;
	PartOfBody leftLeg0;
	PartOfBody leftLeg1;
	PartOfBody leftLeg2;
	PartOfBody rightLeg0;
	PartOfBody rightLeg1;
	PartOfBody rightLeg2;
	PartOfBody leftArm0;
	PartOfBody leftArm1;
	PartOfBody leftArm2;
	PartOfBody rightArm0;
	PartOfBody rightArm1;
	PartOfBody rightArm2;
	PartOfBody head;

	std::vector<std::vector<VertInCaps>> vertices;

	std::vector<Capsule*> caps;
	std::vector<Capsule*> capsInObjSpace;
};

#endif