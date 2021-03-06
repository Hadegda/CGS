#ifndef _GENETIC_ALGORITHM_H_
#define _GENETIC_ALGORITHM_H_

#include <vector>
#include <map>
#include "md2Model.h"
#include "skeleton.h"

#define AGE_FOR_DEAD 20
#define LIKELIHOOD_MUTATION 40.0f
#define FREQUENCY_MUTATION 9
#define DEAD_PART 0.5f
#define COUNT_NEW_INDIVIDS 25
#define COUNT_ITERATIONS 1

#define LEVEL_MUTATION0 50.0f
#define LEVEL_MUTATION1 25.5f

typedef struct {
	DirectX::XMFLOAT3 pos;
	UINT claster;
} gen_vertex;

typedef struct {
	gen_vertex vert;
	std::vector<UINT> clasters;
	float weight;
} ga_vertex;

typedef struct {
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
	DirectX::XMFLOAT3 center;
} BoundaredBox;

struct Data{
	Data(Skeleton* individ) : individ(individ), age(0), mustDie(false) {};
	~Data() { delete individ; };
	Skeleton* individ;
	int age;
	bool mustDie;
};

struct GAdataForFrame {
	GAdataForFrame() { pGenVertices = NULL; pBBoxVertices = NULL;};
	~GAdataForFrame() { if (pBBoxVertices) pBBoxVertices->Release(); if (pGenVertices) pGenVertices->Release(); };

	std::vector<ga_vertex> vertexData;
//	std::vector<DirectX::XMFLOAT3> vertexData;
	BoundaredBox bBox;
	ID3D11Buffer* pGenVertices;
	ID3D11Buffer* pBBoxVertices;

	DirectX::XMFLOAT3 center;
	float volume;
};

class GeneticAlgorithm {
public:
	GeneticAlgorithm();
	~GeneticAlgorithm();
	bool InitGeneticAlgorithm(ModelMD2* model);
	bool InitGeneticAlgorithm(wchar_t* path);
	void Optimize(bool isItStartGA);
	void NextOptimize(bool mutationTime);

	void NextFrame();
	void PrevFrame();

	float Distance(Skeleton *skeleton);

	void DrawLineModel();
	void DrawModelBBox();
	void DrawSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency);
	void DrawBestSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency);

private:
	bool InitGAFrame(daliasframe_t* frame, GAdataForFrame* curFrame, std::vector<DirectX::XMUINT3> tris, const int nVertices);

	void CreatefirstBreed();
	void CreateNextBreed(int count, std::vector<Skeleton*> prevBreed, std::vector<float> individSurvival);
	void Select(float deadPart);
	void Mutation(float likelihoodMutation);

	std::vector<GAdataForFrame> allFrameData;
	int curFrame;

	ID3D11Buffer* pBBoxIndices;
	ID3D11Buffer* pLineModelIndices;
	int nLineModelIndices;
	std::vector<UINT32> lineModelIndices;

	std::map<float, Data*> breed;

	int nIter;
};

#endif