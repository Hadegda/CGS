#ifndef _GENETIC_ALGORITHM_H_
#define _GENETIC_ALGORITHM_H_

#include <vector>
#include <map>
#include "md2Model.h"
#include "skeleton.h"

#define AGE_FOR_DEAD 200
#define LIKELIHOOD_MUTATION 40.0f
#define FREQUENCY_MUTATION 9
#define DEAD_PART 0.5f
#define COUNT_BIRTH_FROM_TWO_PARENTS 20
#define COUNT_TRIES_BIRTH_FROM_ONE_PARENT 5
#define COUNT_ITERATIONS_IN_STEP 1
#define COUNT_ITERATIONS 17

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
	~GAdataForFrame() { if (pBBoxVertices) pBBoxVertices->Release(); if (pGenVertices) pGenVertices->Release(); delete bestSkeleton; };

	std::vector<ga_vertex> vertexData;
	BoundaredBox bBox;
	ID3D11Buffer* pGenVertices;
	ID3D11Buffer* pBBoxVertices;

	Skeleton* bestSkeleton;

	DirectX::XMFLOAT3 center;
	float volume;
};

class GeneticAlgorithm {
public:
	GeneticAlgorithm();
	~GeneticAlgorithm();
	bool InitGeneticAlgorithm(ModelMD2* model);
	bool InitGeneticAlgorithm(wchar_t* path);
	void Optimize();
	void NextOptimize(bool mutationTime);
	void OptimizeAll();

	void NextFrame();
	void PrevFrame();

	float Distance(Skeleton *skeleton);

	void DrawLineModel();
	void DrawModelBBox();
	void DrawSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency);
	void DrawBestSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency);

	bool SaveAll();
	bool Save();

private:
	bool InitGAFrame(daliasframe_t* frame, GAdataForFrame* curFrame, std::vector<DirectX::XMUINT3> tris, const int nVertices);

	void CreateBaseSkeleton();
	void CreatefirstBreed();
	void CreateNextBreed(int count, int countOfOneParentsChildsTry, std::vector<Skeleton*> prevBreed, std::vector<float> individSurvival);
	void Select(float deadPart);
	void Mutation(float likelihoodMutation);

	std::vector<GAdataForFrame> allFrameData;
	int curFrame;

	ID3D11Buffer* pBBoxIndices;
	ID3D11Buffer* pLineModelIndices;
	int nLineModelIndices;
	std::vector<UINT32> lineModelIndices;

	std::map<float, Data*> breed;

	bool hasSkeletonFile;
	std::wstring sFileName;

	int nIter;
};


struct sHeader {
	int nFrames;
	int nCaps;
	int sizeOfcaps;
};

#endif