#include "GeneticAlgorithm.h"

#include <fstream>
#include "device.h"

using namespace std;

bool GeneticAlgorithm::InitGeneticAlgorithm(ModelMD2* model)
{
	return true;
}

bool GeneticAlgorithm::InitGAFrame(daliasframe_t* frame, GAdataForFrame* curFrame, std::vector<DirectX::XMUINT3> tris, const int nVertices) {
	curFrame->vertexData.clear();
	curFrame->vertexData.resize(nVertices);

	curFrame->bBox.min.x = curFrame->bBox.max.x = frame->scale[0] * frame->verts[0].v[0] + frame->translate[0];
	curFrame->bBox.min.y = curFrame->bBox.max.y = frame->scale[1] * frame->verts[0].v[1] + frame->translate[1];
	curFrame->bBox.min.z = curFrame->bBox.max.z = frame->scale[2] * frame->verts[0].v[2] + frame->translate[2];
	curFrame->center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	std::vector<int> cVertexTris(nVertices);
	for (int j = 0; j < nVertices; j++) {
		curFrame->vertexData[j].vert.pos.x = frame->scale[0] * frame->verts[j].v[0] + frame->translate[0];
		curFrame->vertexData[j].vert.pos.y = frame->scale[1] * frame->verts[j].v[1] + frame->translate[1];
		curFrame->vertexData[j].vert.pos.z = frame->scale[2] * frame->verts[j].v[2] + frame->translate[2];
		curFrame->vertexData[j].weight = 0.0f;
		curFrame->vertexData[j].vert.claster = 10;
		cVertexTris[j] = 0;

		curFrame->bBox.min.x = (curFrame->bBox.min.x > curFrame->vertexData[j].vert.pos.x) ? curFrame->vertexData[j].vert.pos.x : curFrame->bBox.min.x;
		curFrame->bBox.min.y = (curFrame->bBox.min.y > curFrame->vertexData[j].vert.pos.y) ? curFrame->vertexData[j].vert.pos.y : curFrame->bBox.min.y;
		curFrame->bBox.min.z = (curFrame->bBox.min.z > curFrame->vertexData[j].vert.pos.z) ? curFrame->vertexData[j].vert.pos.z : curFrame->bBox.min.z;

		curFrame->bBox.max.x = (curFrame->bBox.max.x < curFrame->vertexData[j].vert.pos.x) ? curFrame->vertexData[j].vert.pos.x : curFrame->bBox.max.x;
		curFrame->bBox.max.y = (curFrame->bBox.max.y < curFrame->vertexData[j].vert.pos.y) ? curFrame->vertexData[j].vert.pos.y : curFrame->bBox.max.y;
		curFrame->bBox.max.z = (curFrame->bBox.max.z < curFrame->vertexData[j].vert.pos.z) ? curFrame->vertexData[j].vert.pos.z : curFrame->bBox.max.z;

		curFrame->bBox.center.x = (curFrame->bBox.min.x + curFrame->bBox.max.x) / 2;
		curFrame->bBox.center.y = (curFrame->bBox.min.y + curFrame->bBox.max.y) / 2;
		curFrame->bBox.center.z = (curFrame->bBox.min.z + curFrame->bBox.max.z) / 2;

		curFrame->center.x += curFrame->vertexData[j].vert.pos.x;
		curFrame->center.y += curFrame->vertexData[j].vert.pos.y;
		curFrame->center.z += curFrame->vertexData[j].vert.pos.z;
	}

	curFrame->center.x /= (float)nVertices;
	curFrame->center.y /= (float)nVertices;
	curFrame->center.z /= (float)nVertices;

	curFrame->volume = 0.0f;

	DirectX::XMVECTOR v1;
	DirectX::XMVECTOR v2;
	DirectX::XMVECTOR v3;
	DirectX::XMVECTOR cross;
	DirectX::XMFLOAT3 top = DirectX::XMFLOAT3(curFrame->vertexData[0].vert.pos.x, curFrame->vertexData[0].vert.pos.y, curFrame->vertexData[0].vert.pos.z );
	DirectX::XMFLOAT3 res;
	for (int j = 0; j < tris.size(); j++) {
		v1 = DirectX::XMVectorSet(curFrame->vertexData[tris[j].y].vert.pos.x - curFrame->vertexData[tris[j].x].vert.pos.x, curFrame->vertexData[tris[j].y].vert.pos.y - curFrame->vertexData[tris[j].x].vert.pos.y, curFrame->vertexData[tris[j].y].vert.pos.z - curFrame->vertexData[tris[j].x].vert.pos.z, 1.0f);
		v2 = DirectX::XMVectorSet(curFrame->vertexData[tris[j].z].vert.pos.x - curFrame->vertexData[tris[j].x].vert.pos.x, curFrame->vertexData[tris[j].z].vert.pos.y - curFrame->vertexData[tris[j].x].vert.pos.y, curFrame->vertexData[tris[j].z].vert.pos.z - curFrame->vertexData[tris[j].x].vert.pos.z, 1.0f);
		v3 = DirectX::XMVectorSet(top.x - curFrame->vertexData[tris[j].x].vert.pos.x, top.y - curFrame->vertexData[tris[j].x].vert.pos.y, top.z - curFrame->vertexData[tris[j].x].vert.pos.z, 1.0f);

		cross = DirectX::XMVector3Cross(v1, v2);
		DirectX::XMStoreFloat3(&res, DirectX::XMVector3Length(cross));

		float s = res.x / 2.0f;
		curFrame->vertexData[tris[j].x].weight += s;
		cVertexTris[tris[j].x]++;
		curFrame->vertexData[tris[j].y].weight += s;
		cVertexTris[tris[j].y]++;
		curFrame->vertexData[tris[j].z].weight += s;
		cVertexTris[tris[j].z]++;

		DirectX::XMStoreFloat3(&res, DirectX::XMVector3Dot(cross, v3));
		curFrame->volume += res.x / 6.0f;
	}

	float maxWeight = 0.0f;
	for (int j = 0; j < nVertices; j++) {
		if (maxWeight < curFrame->vertexData[j].weight)
			maxWeight = curFrame->vertexData[j].weight;
		curFrame->vertexData[j].weight /= (float)cVertexTris[j];
	}

	for (int j = 0; j < nVertices; j++) {
		curFrame->vertexData[j].weight /= maxWeight;
	}

	HRESULT hr = S_OK;
	vector<gen_vertex> vertices;
	vertices.clear();
	vertices.resize(nVertices);
	for (int j = 0; j < nVertices; j++) {
		vertices[j] = curFrame->vertexData[j].vert;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(gen_vertex) * (nVertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices.data();
	hr = RendererCore::Get()->GetDevice()->CreateBuffer(&bd, &InitData, &curFrame->pGenVertices);
	if (FAILED(hr))
		return false;
	CopyNameToDebugObjectName(curFrame->pGenVertices);

	vertices.clear();
	vertices.resize(8);
	vertices[0] = { DirectX::XMFLOAT3{ curFrame->bBox.min.x, curFrame->bBox.min.y, curFrame->bBox.min.z}, (UINT)100 };
	vertices[1] = { DirectX::XMFLOAT3{ curFrame->bBox.min.x, curFrame->bBox.min.y, curFrame->bBox.max.z}, (UINT)100 };
	vertices[2] = { DirectX::XMFLOAT3{ curFrame->bBox.max.x, curFrame->bBox.min.y, curFrame->bBox.max.z}, (UINT)100 };
	vertices[3] = { DirectX::XMFLOAT3{ curFrame->bBox.max.x, curFrame->bBox.min.y, curFrame->bBox.min.z}, (UINT)100 };
	vertices[4] = { DirectX::XMFLOAT3{ curFrame->bBox.min.x, curFrame->bBox.max.y, curFrame->bBox.min.z}, (UINT)100 };
	vertices[5] = { DirectX::XMFLOAT3{ curFrame->bBox.min.x, curFrame->bBox.max.y, curFrame->bBox.max.z}, (UINT)100 };
	vertices[6] = { DirectX::XMFLOAT3{ curFrame->bBox.max.x, curFrame->bBox.max.y, curFrame->bBox.max.z}, (UINT)100 };
	vertices[7] = { DirectX::XMFLOAT3{ curFrame->bBox.max.x, curFrame->bBox.max.y, curFrame->bBox.min.z}, (UINT)100 };

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(gen_vertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices.data();
	hr = RendererCore::Get()->GetDevice()->CreateBuffer(&bd, &InitData, &curFrame->pBBoxVertices);
	if (FAILED(hr))
		return false;
	CopyNameToDebugObjectName(curFrame->pBBoxVertices);
}

bool GeneticAlgorithm::InitGeneticAlgorithm(wchar_t * path)
{
	HRESULT hr = S_OK;

	std::ifstream file;
	file.open(path, std::ios::in | std::ios::binary);
	if (file.fail())
		return false;

	md2_header header;
	file.read((char *)&header, sizeof(md2_header));

	const int nFrames = header.num_frames;
	const int nVertices = header.num_xyz;
	const int nTris = header.num_tris;

	std::vector<DirectX::XMUINT3> tris(nTris);
	char* buffer = new char[nTris * sizeof(dtriangle_t)];
	file.seekg(header.ofs_tris, std::ios::beg);
	file.read((char *)buffer, nTris * sizeof(dtriangle_t));

	dtriangle_t* triangle;
	for (int i = 0; i < nTris; i++) {
		triangle = reinterpret_cast<dtriangle_t*>((byte *)buffer + i * sizeof(dtriangle_t));

		tris[i].x = triangle->index_xyz[0];
		tris[i].y = triangle->index_xyz[1];
		tris[i].z = triangle->index_xyz[2];
	}
	delete[] buffer;

	buffer = new char[nFrames * header.framesize];
	file.seekg(header.ofs_frames, std::ios::beg);
	file.read((char *)buffer, nFrames * header.framesize);

	allFrameData.clear();
	allFrameData.resize(nFrames);
	daliasframe_t* frame;
	for (int i = 0; i < nFrames; i++) {
		frame = reinterpret_cast<daliasframe_t*>((byte *)buffer + i * header.framesize);
		InitGAFrame(frame, &(allFrameData[i]), tris, nVertices);
	}
	delete[] buffer;

	UINT32 indices[36] = {
		0, 1, 2,
		0, 2, 3,
		0, 4, 5,
		0, 5, 1,
		1, 5, 6,
		1, 6, 2,
		2, 6, 7,
		2, 7, 3,
		3, 7, 4,
		3, 4, 0,
		4, 7, 6,
		4, 6, 5
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT32) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = RendererCore::Get()->GetDevice()->CreateBuffer(&bd, &InitData, &pBBoxIndices);
	if (FAILED(hr))
		return false;
	CopyNameToDebugObjectName(pBBoxIndices);

	nLineModelIndices = (2 * nTris * 3) / 2;
	std::vector<std::vector<bool>> mModelIndecies(nVertices);
	for (int i = 0; i < mModelIndecies.size(); i++) {
		mModelIndecies[i].clear();
		mModelIndecies[i].resize(nVertices);
		for (int j = 0; j < mModelIndecies[i].size(); j++) {
			mModelIndecies[i][j] = false;
		}
	}

	lineModelIndices.clear();
	lineModelIndices.resize(nLineModelIndices);
	for (int i = 0, k = 0; i < nTris; i++) {
		if (mModelIndecies[tris[i].y][tris[i].x] == false) {
			mModelIndecies[tris[i].x][tris[i].y] = true;
			lineModelIndices[k + 0] = tris[i].x;
			lineModelIndices[k + 1] = tris[i].y;
			k += 2;
		}

		if (mModelIndecies[tris[i].z][tris[i].y] == false) {
			mModelIndecies[tris[i].y][tris[i].z] = true;
			lineModelIndices[k + 0] = tris[i].y;
			lineModelIndices[k + 1] = tris[i].z;
			k += 2;
		}

		if (mModelIndecies[tris[i].x][tris[i].z] == false) {
			mModelIndecies[tris[i].z][tris[i].x] = true;
			lineModelIndices[k + 0] = tris[i].z;
			lineModelIndices[k + 1] = tris[i].x;
			k += 2;
		}
	}

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT32) * nLineModelIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = lineModelIndices.data();
	hr = RendererCore::Get()->GetDevice()->CreateBuffer(&bd, &InitData, &pLineModelIndices);
	if (FAILED(hr))
		return false;
	CopyNameToDebugObjectName(pLineModelIndices);

	Optimize(true);

	file.close();
	return true;
}

void GeneticAlgorithm::CreatefirstBreed()
{
	for (const auto& elem : breed)
		delete elem.second;
	breed.clear();

	Skeleton* tmp;
	tmp = new Skeleton(DirectX::XMFLOAT3{ 1.0f, 0.0f, 0.0f });
	tmp->CreateStandart();
	tmp->Scale((allFrameData[curFrame].bBox.max.z - allFrameData[curFrame].bBox.min.z));
	//tmp->Centered(allFrameData[curFrame].bBox.center);
	tmp->Centered(allFrameData[curFrame].center);
	tmp->CreateCaps();
	breed.insert({ Distance(tmp), new Data(tmp) });

	tmp = new Skeleton(DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f });
	tmp->CreateStandart();
	tmp->Scale((allFrameData[curFrame].bBox.max.z - allFrameData[curFrame].bBox.min.z));
	//tmp->Centered(allFrameData[curFrame].bBox.center);
	tmp->Centered(allFrameData[curFrame].center);
	tmp->InvertY();
	tmp->CreateCaps();
	breed.insert({ Distance(tmp), new Data(tmp) });

	tmp = new Skeleton(DirectX::XMFLOAT3{ 0.0f, 0.0f, 1.0f });
	tmp->CreateStandart();
	tmp->Scale((allFrameData[curFrame].bBox.max.y - allFrameData[curFrame].bBox.min.y));
	//tmp->Centered(allFrameData[curFrame].bBox.center);
	tmp->Centered(allFrameData[curFrame].center);
	tmp->DropLeft();
	tmp->CreateCaps();
	breed.insert({ Distance(tmp), new Data(tmp) });

	tmp = new Skeleton(DirectX::XMFLOAT3{ 1.0f, 1.0f, 0.0f });
	tmp->CreateStandart();
	tmp->Scale((allFrameData[curFrame].bBox.max.y - allFrameData[curFrame].bBox.min.y));
	//tmp->Centered(allFrameData[curFrame].bBox.center);
	tmp->Centered(allFrameData[curFrame].center);
	tmp->DropRight();
	tmp->CreateCaps();
	breed.insert({ Distance(tmp), new Data(tmp) });

	tmp = new Skeleton(DirectX::XMFLOAT3{ 0.0f, 1.0f, 1.0f });
	tmp->CreateStandart();
	tmp->Scale((allFrameData[curFrame].bBox.max.x - allFrameData[curFrame].bBox.min.x));
	//tmp->Centered(allFrameData[curFrame].bBox.center);
	tmp->Centered(allFrameData[curFrame].center);
	tmp->DropBack();
	tmp->CreateCaps();
	breed.insert({ Distance(tmp), new Data(tmp) });

	tmp = new Skeleton(DirectX::XMFLOAT3{ 1.0f, 0.0f, 1.0f });
	tmp->CreateStandart();
	tmp->Scale((allFrameData[curFrame].bBox.max.x - allFrameData[curFrame].bBox.min.x));
	//tmp->Centered(allFrameData[curFrame].bBox.center);
	tmp->Centered(allFrameData[curFrame].center);
	tmp->DropForward();
	tmp->CreateCaps();
	breed.insert({ Distance(tmp), new Data(tmp) });
}

unsigned ChooseIndex(vector<float> individSurvival) {
	float val = (float)(rand() % 101);
	float sum = 0;
	for (int i = 0; i < individSurvival.size(); i++) {
		if (sum <= val && sum + individSurvival[i] >= val) {
			return i;
		}
		else {
			sum += individSurvival[i];
		}
	}

	return individSurvival.size() - 1;
}

void GeneticAlgorithm::CreateNextBreed(int count, vector<Skeleton*> prevBreed, vector<float> individSurvival) {
	vector<float> likelihoodSurvival;
	likelihoodSurvival.resize(individSurvival.size());
	float breedSurvival = 0;
	for (int i = 0; i < breed.size(); i++) {
		likelihoodSurvival[i] = 1 / individSurvival[i];
		breedSurvival += likelihoodSurvival[i];
	}
	for (int i = 0; i < breed.size(); i++) {
		likelihoodSurvival[i] = 100 * likelihoodSurvival[i] / breedSurvival;
	}

	for (int i = 0; i < count;) {
		unsigned nParent0 = ChooseIndex(likelihoodSurvival);
		unsigned nParent1 = ChooseIndex(likelihoodSurvival);

		if (nParent0 == nParent1)
			continue;
		Skeleton* tmp = new Skeleton(DirectX::XMFLOAT3{ (float)i / count, 0.4f, 0.8f });
		tmp->CreateFromParents(prevBreed[nParent0], prevBreed[nParent1], individSurvival[nParent0] / (individSurvival[nParent0] + individSurvival[nParent1]), individSurvival[nParent1] / (individSurvival[nParent0] + individSurvival[nParent1]));
		tmp->CreateCaps();
		breed.insert({ Distance(tmp), new Data(tmp) });
		
		i++;
	}
}

//#include <iterator>
void GeneticAlgorithm::Select(float deadPart) {
	int deadCount = breed.size() * deadPart;

	auto last = prev(breed.end());
	for (int i = 0; i < deadCount; i++) {
		(*last).second->mustDie = true;
		last = prev(last);
	}
}

void GeneticAlgorithm::Mutation(float likelihoodMutation) {

	vector<Data*> tmp(0);
	vector<float> tmpKeys(0);

	for (const auto& elem : breed) {
		if ((float)(rand() % 101) < likelihoodMutation) {
			tmp.push_back(elem.second);
			tmpKeys.push_back(elem.first);
		}
	}

	for (int i = 0; i < tmp.size(); i++) {
		breed.erase(tmpKeys[i]);
	}
	int level = 2;
	int n = 0;
	if ((*breed.begin()).first > LEVEL_MUTATION0) {
		level = 0;
	}
	else {
		if ((*breed.begin()).first > LEVEL_MUTATION1) {
			n = rand() % 14;
			level = 1;
		}
		else {
			n = rand() % 125;
			level = 2;
		}
	}
	for (int i = 0; i < tmp.size(); i++) {
		tmp[i]->individ->Mutation(level, n);
		breed.insert({ Distance(tmp[i]->individ), tmp[i] });
	}
}

void GeneticAlgorithm::NextOptimize(bool mutationTime) {
	if(mutationTime)
		Mutation(LIKELIHOOD_MUTATION);

	vector<Skeleton*> oldBreed(breed.size());
	vector<float> individSurvival(breed.size());

	int i = 0;
	for (const auto& elem : breed) {
		individSurvival[i] = elem.first;
		oldBreed[i] = elem.second->individ;
		if (elem.second->age == AGE_FOR_DEAD)
			elem.second->mustDie = true;
		else
			elem.second->age++;
		i++;
	}

	Select(DEAD_PART);
	CreateNextBreed(COUNT_NEW_INDIVIDS, oldBreed, individSurvival);

	std::map<float, Data*>::iterator cur, end;
	cur = breed.begin();
	end = breed.end();

	while (cur != end)
	{
		if ((*cur).second->mustDie) {
			delete (*cur).second;
			breed.erase(cur++);
		}
		else
			cur++;
	}
	return;
}

void GeneticAlgorithm::Optimize(bool isItStartGA)
{
	if (isItStartGA) {
		CreatefirstBreed();
		nIter = 0;
		return;
	}

	for (int k = 0; k < COUNT_ITERATIONS; k++, nIter++) {
		if ((*breed.begin()).first < 1.0f) {
			break;
		}
		if( nIter % FREQUENCY_MUTATION == (FREQUENCY_MUTATION - 1))
			NextOptimize(true);
		else
			NextOptimize(false);
	}
}

void GeneticAlgorithm::NextFrame(){
	curFrame = (curFrame + 1 < allFrameData.size()) ? curFrame + 1 : 0;
	Optimize(true);
}

void GeneticAlgorithm::PrevFrame(){
	curFrame = (curFrame > 0) ? curFrame - 1 : allFrameData.size() - 1;
	Optimize(true);
}

float GeneticAlgorithm::Distance(Skeleton *skeleton)
{
	const int nCapsules = 28;
	DirectX::XMFLOAT4* capsules;
	capsules = new DirectX::XMFLOAT4[nCapsules];

	skeleton->CapsulesInObjSys(&capsules);
	std::vector<int> nVertInsideCapsule(nCapsules / 2);

	GAdataForFrame* frameData = &allFrameData[curFrame];

	float qdist = 0.0f;
	float dist = 0.0f;
	float t = 0.0f;
	for (int i = 0; i < frameData->vertexData.size(); i++) {
		frameData->vertexData[i].clasters.clear();
		DirectX::XMFLOAT3 vertex = DirectX::XMFLOAT3{ frameData->vertexData[i].vert.pos.x, frameData->vertexData[i].vert.pos.y, frameData->vertexData[i].vert.pos.z };
		float minDistance = 10000.0f;
		int nBestCaps = -1;
		bool insideSkelet = false;
		for (int j = 0; j < nCapsules; j += 2) {
			DirectX::XMFLOAT3 dir = DirectX::XMFLOAT3{ capsules[j + 1].x - capsules[j].x, capsules[j + 1].y - capsules[j].y, capsules[j + 1].z - capsules[j].z };
			t = ((vertex.x - capsules[j].x)*dir.x + (vertex.y - capsules[j].y)*dir.y + (vertex.z - capsules[j].z)*dir.z) / (dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
			t = (t < 0) ? 0 : ((t > 1) ? 1 : t);
			dist = sqrt((capsules[j].x - vertex.x + dir.x*t)*(capsules[j].x - vertex.x + dir.x*t) + (capsules[j].y - vertex.y + dir.y*t)*(capsules[j].y - vertex.y + dir.y*t) + (capsules[j].z - vertex.z + dir.z*t)*(capsules[j].z - vertex.z + dir.z*t)) - capsules[j].w;// (capsules[j].w + t * (capsules[j + 1].w - capsules[j].w));
			if (dist <= 0.0f) {
				insideSkelet = true;
				nBestCaps = -1;
				minDistance = dist;
				frameData->vertexData[i].vert.claster = (UINT)(j / 2);
				frameData->vertexData[i].clasters.push_back((UINT)(j / 2));

				nVertInsideCapsule[j / 2]++;
				//break;
			}
			if (!insideSkelet && minDistance > dist) {
				minDistance = dist;
				nBestCaps = j / 2;
				frameData->vertexData[i].vert.claster = (UINT)(j / 2);
			}
		}
		if(nBestCaps != -1)
			nVertInsideCapsule[nBestCaps]++;
		qdist += (minDistance*minDistance)/* * frameData->vertexData[i].pos.w* frameData->vertexData[i].pos.w*/;
	}
	qdist = sqrt(qdist);

	/*for (int i = 0; i < nVertInsideCapsule.size(); i++) {
		if (nVertInsideCapsule[i] < 5) {
			qdist += 50.0f;
			continue;
		}
	}*/
	
	for (int i = 0; i < nLineModelIndices; i += 2) {
		UINT c0 = frameData->vertexData[lineModelIndices[i]].vert.claster;
		UINT c1 = frameData->vertexData[lineModelIndices[i + 1]].vert.claster;
		switch (c0) {
		case 0:
			if (c1 != 0 && c1 != 1 && c1 != 4 && c1 != 7 && c1 != 10 && c1 != 13)
				qdist += 0.2f;
			break;

		case 1:
			if (c1 != 0 && c1 != 1 && c1 != 2)
				qdist += 0.2f;
			break;

		case 2:
			if (c1 != 1 && c1 != 2 && c1 != 3)
				qdist += 0.2f;
			break;

		case 3:
			if (c1 != 2 && c1 != 3)
				qdist += 0.2f;
			break;

		case 4:
			if (c1 != 0 && c1 != 4 && c1 != 5)
				qdist += 0.2f;
			break;

		case 5:
			if (c1 != 4 && c1 != 5 && c1 != 6)
				qdist += 0.2f;
			break;

		case 6:
			if (c1 != 5 && c1 != 6)
				qdist += 0.2f;
			break;

		case 7:
			if (c1 != 0 && c1 != 7 && c1 != 8)
				qdist += 0.2f;
			break;

		case 8:
			if (c1 != 7 && c1 != 8 && c1 != 9)
				qdist += 0.2f;
			break;

		case 9:
			if (c1 != 8 && c1 != 9)
				qdist += 0.2f;
			break;

		case 10:
			if (c1 != 0 && c1 != 10 && c1 != 11)
				qdist += 0.2f;
			break;

		case 11:
			if (c1 != 00 && c1 != 11 && c1 != 12)
				qdist += 0.2f;
			break;

		case 12:
			if (c1 != 11 && c1 != 12)
				qdist += 0.2f;
			break;

		case 13:
			if (c1 != 0 && c1 != 13)
				qdist += 0.2f;
			break;
		}
	}

	if (frameData->pGenVertices) frameData->pGenVertices->Release();

	vector<gen_vertex> vertices(frameData->vertexData.size());
	for (int j = 0; j < frameData->vertexData.size(); j++) {
		vertices[j] = frameData->vertexData[j].vert;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(gen_vertex) * (frameData->vertexData.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = S_OK;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices.data();
	hr = RendererCore::Get()->GetDevice()->CreateBuffer(&bd, &InitData, &frameData->pGenVertices);
	if (FAILED(hr))
		return -1.0f;
	CopyNameToDebugObjectName(frameData->pGenVertices);

	if (qdist < 0.1f) {
		return qdist;
	}
	delete[] capsules;
	return qdist;
}

void GeneticAlgorithm::DrawModelBBox()
{
	UINT strides[] = { sizeof(gen_vertex) };
	UINT offsets[] = { 0 };
	RendererCore::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &allFrameData[curFrame].pBBoxVertices, strides, offsets);
	RendererCore::Get()->GetDeviceContext()->IASetIndexBuffer(pBBoxIndices, DXGI_FORMAT_R32_UINT, 0);

	RendererCore::Get()->GetDeviceContext()->DrawIndexed(36, 0, 0);
}

void GeneticAlgorithm::DrawSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency){
	for (const auto& elem : breed)
		elem.second->individ->DrawSkeleton(world, light, 20.0f);
}

void GeneticAlgorithm::DrawBestSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency) {
	(*breed.begin()).second->individ->DrawSkeleton(world, -1.0f, 20.0f);
}

void GeneticAlgorithm::DrawLineModel()
{
	Distance((*breed.begin()).second->individ);
	UINT strides[] = { sizeof(gen_vertex) };
	UINT offsets[] = { 0 };
	RendererCore::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &allFrameData[curFrame].pGenVertices, strides, offsets);
	RendererCore::Get()->GetDeviceContext()->IASetIndexBuffer(pLineModelIndices, DXGI_FORMAT_R32_UINT, 0);
	
	RendererCore::Get()->GetDeviceContext()->DrawIndexed(nLineModelIndices, 0, 0);
	//RendererCore::Get()->GetDeviceContext()->Draw(allFrameData[curFrame].vertexData.size(), 0);
}

#include <time.h>
GeneticAlgorithm::GeneticAlgorithm() {
	curFrame = 0;
	pBBoxIndices = NULL;
	pLineModelIndices = NULL;
	nLineModelIndices = 0;

	srand(time(NULL));
}

GeneticAlgorithm::~GeneticAlgorithm(){
	if (pBBoxIndices) pBBoxIndices->Release();
	if (pLineModelIndices) pLineModelIndices->Release();

	/*for (int i = 0; i < breed.size(); i++)
		delete breed[i];*/
	std::map<float, Data*>::iterator cur, end;
	cur = breed.begin();
	end = breed.end();

	while (cur != end)
	{
		delete (*cur).second;
		cur++;
	}
	breed.clear();
}