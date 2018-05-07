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
	std::vector<DirectX::XMFLOAT3> vertices(allFrameData[curFrame].vertexData.size());
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].x = allFrameData[curFrame].vertexData[i].vert.pos.x;
		vertices[i].y = allFrameData[curFrame].vertexData[i].vert.pos.y;
		vertices[i].z = allFrameData[curFrame].vertexData[i].vert.pos.z;
	}

	for (const auto& elem : breed)
		delete elem.second;
	breed.clear();

	PersonPattern* person;
	person = new PersonPattern();
	person->Scale((allFrameData[curFrame].bBox.max.z - allFrameData[curFrame].bBox.min.z));
	person->Centered(allFrameData[curFrame].center);
	Skeleton* tmp = new Skeleton(person, vertices, lineModelIndices, DirectX::XMFLOAT3{ 1.0f, 0.0f, 0.0f });
	breed.insert({ Distance(tmp), new Data(tmp) });
	delete person;

	person = new PersonPattern();
	person->Scale((allFrameData[curFrame].bBox.max.z - allFrameData[curFrame].bBox.min.z));
	person->Centered(allFrameData[curFrame].center);
	person->InvertY();
	tmp = new Skeleton(person, vertices, lineModelIndices, DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f });
	breed.insert({ Distance(tmp), new Data(tmp) });
	delete person;

	person = new PersonPattern();
	person->Scale((allFrameData[curFrame].bBox.max.y - allFrameData[curFrame].bBox.min.y));
	person->Centered(allFrameData[curFrame].center);
	person->DropLeft();
	tmp = new Skeleton(person, vertices, lineModelIndices, DirectX::XMFLOAT3{ 0.0f, 0.0f, 1.0f });
	breed.insert({ Distance(tmp), new Data(tmp) });
	delete person;

	person = new PersonPattern();
	person->Scale((allFrameData[curFrame].bBox.max.y - allFrameData[curFrame].bBox.min.y));
	person->Centered(allFrameData[curFrame].center);
	person->DropRight();
	tmp = new Skeleton(person, vertices, lineModelIndices, DirectX::XMFLOAT3{ 1.0f, 1.0f, 0.0f });
	breed.insert({ Distance(tmp), new Data(tmp) });
	delete person;

	person = new PersonPattern();
	person->Scale((allFrameData[curFrame].bBox.max.x - allFrameData[curFrame].bBox.min.x));
	person->Centered(allFrameData[curFrame].center);
	person->DropBack();
	tmp = new Skeleton(person, vertices, lineModelIndices, DirectX::XMFLOAT3{ 1.0f, 0.0f, 1.0f });
	breed.insert({ Distance(tmp), new Data(tmp) });
	delete person;

	person = new PersonPattern();
	person->Scale((allFrameData[curFrame].bBox.max.x - allFrameData[curFrame].bBox.min.x));
	person->Centered(allFrameData[curFrame].center);
	person->DropForward();
	tmp = new Skeleton(person, vertices, lineModelIndices, DirectX::XMFLOAT3{ 0.0f, 1.0f, 1.0f });
	breed.insert({ Distance(tmp), new Data(tmp) });
	delete person;
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
	/*vector<float> likelihoodSurvival;
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
	}*/
}

void GeneticAlgorithm::Select(float deadPart) {
	/*int deadCount = breed.size() * deadPart;

	auto last = prev(breed.end());
	for (int i = 0; i < deadCount; i++) {
		(*last).second->mustDie = true;
		last = prev(last);
	}*/
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
	/*int level = 2;
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
	}*/
	for (int i = 0; i < tmp.size(); i++) {
		tmp[i]->individ->OptimizeCapsules();
		breed.insert({ Distance(tmp[i]->individ), tmp[i] });
	}
}

void GeneticAlgorithm::NextOptimize(bool mutationTime) {
	//if(mutationTime)
		//Mutation(LIKELIHOOD_MUTATION);

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

	std::map<float, Data*>::iterator pcur, pend;
	pcur = breed.begin();
	pend = breed.end();

	while (pcur != pend)
	{
		(*pcur).second->individ->OptimizeCapsules();
		Distance((*pcur).second->individ);
		pcur++;
	}

	//Select(DEAD_PART);
	//CreateNextBreed(COUNT_NEW_INDIVIDS, oldBreed, individSurvival);

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
	/*for (int i = 0; i < nVertInsideCapsule.size(); i++) {
		if (nVertInsideCapsule[i] < 5) {
			qdist += 50.0f;
			continue;
		}
	}*/
	
	std::vector<UINT> clasters(allFrameData[curFrame].vertexData.size());

	float dist = skeleton->Distance(&clasters);

	if (allFrameData[curFrame].pGenVertices) allFrameData[curFrame].pGenVertices->Release();

	vector<gen_vertex> vertices(allFrameData[curFrame].vertexData.size());
	for (int j = 0; j < allFrameData[curFrame].vertexData.size(); j++) {
		vertices[j] = allFrameData[curFrame].vertexData[j].vert;
		vertices[j].claster = clasters[j];
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(gen_vertex) * (allFrameData[curFrame].vertexData.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = S_OK;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices.data();
	hr = RendererCore::Get()->GetDevice()->CreateBuffer(&bd, &InitData, &allFrameData[curFrame].pGenVertices);
	if (FAILED(hr))
		return -1.0f;
	CopyNameToDebugObjectName(allFrameData[curFrame].pGenVertices);

	if (dist < 0.1f) {
		return dist;
	}
	return dist;
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

/*void GeneticAlgorithm::Mutation(float likelihoodMutation) {
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
}*/