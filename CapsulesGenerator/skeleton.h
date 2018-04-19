#ifndef _SKELETON_H_
#define _SKELETON_H_

#include <DirectXMath.h>
#include <vector>
#include <map>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include "capsule.h"
#include "personPattern.h"

class Skeleton {
public:
	bool CreateFromParents(Skeleton* parent0, Skeleton* parent1, float impact0, float impact1);
	void Mutation(int level, int n = 0);
	float Distance(std::vector<UINT>* vertexClasters);
	void OptimizeCapsules();

	void DrawSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency);

	void DistributeVertices();

	void UpdateForNewParameters();
	void UpdateOneCapsule(DirectX::XMFLOAT4* cCur, DirectX::XMFLOAT4 cPrev, int nFirstParam, bool isShiftInterpret);

	float GetVolume();

	Skeleton(PersonPattern* homo, std::vector<DirectX::XMFLOAT3> vertices, std::vector<UINT32> lineModelIndices, DirectX::XMFLOAT3 color);
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

	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<std::vector<UINT>> verticesForCaps; // for each capsule vector of nearest vertex number
	std::vector<std::map<float, UINT>> capsForVertices; // for each vertex sorted(by distance) vector with capsule numbers

	std::vector<UINT32> lineList;
};

#endif