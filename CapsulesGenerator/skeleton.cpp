#include "skeleton.h"

using namespace DirectX;

float Skeleton::Distance(std::vector<UINT>* vertexClasters, float volume) {
	float distance = 0.0f;
	for (int i = 0; i < vertices.size(); i++) {
		(*vertexClasters)[i] = (*capsForVertices[i].begin()).second;
		float d = (*capsForVertices[i].begin()).first;
		if(d > 0)
			distance += d*d;
	}
	distance = sqrt(distance);

	float skeletonVolume = 0.0f;
	for (int i = 0; i < caps.size(); i++) {
		skeletonVolume += caps[i]->GetVolume();
	}

	distance += 20 * (abs(volume - skeletonVolume) / volume)*(abs(volume - skeletonVolume) / volume);

	/*float distance = 1.0f;
	std::vector<float> dist(caps.size());
	for (int i = 0; i < dist.size(); i++)
		dist[i] = 0.0f;
	for (int i = 0; i < vertices.size(); i++) {
		(*vertexClasters)[i] = (*capsForVertices[i].begin()).second;
		float d = (*capsForVertices[i].begin()).first;
		dist[(*vertexClasters)[i]] += d*d;
	}
	for (int i = 0; i < dist.size(); i++) {
		dist[i] = sqrt(dist[i]);
		distance *= (1.0f + dist[i]);
	}
	distance = 20.0f * 1.0f / pow(distance, 1.0f/dist.size());*/

	for (int i = 0; i < lineList.size(); i += 2) {
		UINT c0 = (*capsForVertices[lineList[i]].begin()).second;
		UINT c1 = (*capsForVertices[lineList[i+1]].begin()).second;
		switch (c0) {
		case 0:
			if (c1 != 0 && c1 != 1 && c1 != 4 && c1 != 7 && c1 != 10 && c1 != 13)
				distance += 0.2f;
			break;

		case 1:
			if (c1 != 0 && c1 != 1 && c1 != 2)
				distance += 0.2f;
			break;

		case 2:
			if (c1 != 1 && c1 != 2 && c1 != 3)
				distance += 0.2f;
			break;

		case 3:
			if (c1 != 2 && c1 != 3)
				distance += 0.2f;
			break;

		case 4:
			if (c1 != 0 && c1 != 4 && c1 != 5)
				distance += 0.2f;
			break;

		case 5:
			if (c1 != 4 && c1 != 5 && c1 != 6)
				distance += 0.2f;
			break;

		case 6:
			if (c1 != 5 && c1 != 6)
				distance += 0.2f;
			break;

		case 7:
			if (c1 != 0 && c1 != 7 && c1 != 8)
				distance += 0.2f;
			break;

		case 8:
			if (c1 != 7 && c1 != 8 && c1 != 9)
				distance += 0.2f;
			break;

		case 9:
			if (c1 != 8 && c1 != 9)
				distance += 0.2f;
			break;

		case 10:
			if (c1 != 0 && c1 != 10 && c1 != 11)
				distance += 0.2f;
			break;

		case 11:
			if (c1 != 00 && c1 != 11 && c1 != 12)
				distance += 0.2f;
			break;

		case 12:
			if (c1 != 11 && c1 != 12)
				distance += 0.2f;
			break;

		case 13:
			if (c1 != 0 && c1 != 13)
				distance += 0.2f;
			break;
		}
	}

	return distance;
}

bool Skeleton::OptimizeCapsules() {

	std::vector<std::set<UINT>> pointsOfAdjacentCaps;
	pointsOfAdjacentCaps.clear();
	pointsOfAdjacentCaps.resize(caps.size() - 1);

	UINT min, max;
	for (int i = 0; i < lineList.size(); i += 2) {
		UINT c0 = (*capsForVertices[lineList[i]].begin()).second;
		UINT c1 = (*capsForVertices[lineList[i + 1]].begin()).second;

		if (c0 < c1) {
			min = c0, max = c1;
		}
		else {
			min = c1, max = c0;
		}

		if (max - min == 1 && min != 3 && min != 6 && min != 9 && min != 12) {
			pointsOfAdjacentCaps[min].insert(lineList[i]);
			pointsOfAdjacentCaps[min].insert(lineList[i+1]);
			continue;
		}
		if (min == 0) {
			if (max == 4 || max == 7 || max == 10 || max == 13) {
				pointsOfAdjacentCaps[max - 1].insert(lineList[i]);
				pointsOfAdjacentCaps[max - 1].insert(lineList[i+1]);
			}
		}
	}

	std::vector<XMFLOAT3> junctionPoints;
	junctionPoints.clear();
	junctionPoints.resize(pointsOfAdjacentCaps.size());
	std::vector<bool> hasJunctionPoint;
	hasJunctionPoint.clear();
	hasJunctionPoint.resize(pointsOfAdjacentCaps.size());
	for (int i = 0; i < pointsOfAdjacentCaps.size(); i++) {
		junctionPoints[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);
		std::set<UINT>::iterator pcur, pend;
		pcur = pointsOfAdjacentCaps[i].begin();
		pend = pointsOfAdjacentCaps[i].end();
		while (pcur != pend)
		{
			XMFLOAT3 cur = vertices[(*pcur)];
			junctionPoints[i] = Float3Sum(junctionPoints[i], cur);
			pcur++;
		}
		if (pointsOfAdjacentCaps[i].size() != 0) {
			junctionPoints[i] = Float3ScalarMult(junctionPoints[i], 1.0f / (pointsOfAdjacentCaps[i].size()));
			hasJunctionPoint[i] = true;
		}
		else {
			hasJunctionPoint[i] = false;
		}
	}

	if (!hasJunctionPoint[0] || !hasJunctionPoint[3] || !hasJunctionPoint[6] || !hasJunctionPoint[9])
		return false;

	std::vector<XMFLOAT3> pointSet;
	for (int j = 0; j < caps.size(); j++) {
		pointSet.clear();
		pointSet.resize(verticesForCaps[j].size());
		for (int i = 0; i < pointSet.size(); i++) {
			pointSet[i] = vertices[verticesForCaps[j][i]];
		}

		if (j == 1 || j == 2 || j == 4 || j == 5 || j == 7 || j == 8 || j == 10 || j == 11) {
			if (hasJunctionPoint[j - 1] == true && hasJunctionPoint[j] == true)
				caps[j]->OptimizeBy2PointsAndSet(junctionPoints[j - 1], junctionPoints[j], pointSet);
			else {
				if(j == 1 || j == 4)
					caps[j]->ShiftToPoint(caps[j - 1]->p0);
				else
					caps[j]->ShiftToPoint(caps[j - 1]->p1);
			}
		}
		else
			if (j == 0) {
				std::vector<XMFLOAT3> mainPointSet(4);
				XMFLOAT3 p0 = Float3Sum(junctionPoints[0], junctionPoints[3]);
				p0 = Float3ScalarMult(p0, 0.5f);
				XMFLOAT3 p1 = Float3Sum(junctionPoints[6], junctionPoints[9]);
				if (hasJunctionPoint[12]) {
					p1 = Float3Sum(p1, XMFLOAT3(junctionPoints[12]));
					p1 = Float3ScalarMult(p1, 1.0f / 3.0f);
					mainPointSet.resize(5);
					mainPointSet[4] = junctionPoints[12];
				}
				else {
					p1 = Float3ScalarMult(p1, 1.0f / 2.0f);
				}
				mainPointSet[0] = junctionPoints[0];
				mainPointSet[1] = junctionPoints[3];
				mainPointSet[2] = junctionPoints[6];
				mainPointSet[3] = junctionPoints[9];
				caps[j]->OptimizeBy2PointsAnd2Sets (p0, p1, pointSet, mainPointSet);
			}
			else
				if (hasJunctionPoint[j - 1] == true && pointSet.size() > 2)
					caps[j]->OptimizeByPointAndSet(junctionPoints[j - 1], pointSet);
				else {
					if(j != caps.size() - 1)
						caps[j]->ShiftToPoint(caps[j - 1]->p1);
					else {
						XMFLOAT3 dir = Float3Substruct(caps[0]->p1, caps[0]->p0);
						XMFLOAT3 p = Float3Sum(caps[0]->p1, Float3ScalarMult(dir, (1.0f / Float3Length(dir)) * caps[0]->p0.w));
						caps[j]->ShiftToPoint(XMFLOAT4(p.x, p.y, p.z, 1.0f));
					}
				}
	}
	return true;
}

void Skeleton::Mutation(int level, int n) {
	if (level == 0) {
		MutationAll();
	}
	else {
		if (level == 1)
			OneCapsuleMutation(n);
		else
			OneGenMutation(n);
	}

	UpdateForNewParameters();
	CorrectionSkeleton();
	UpdateForNewCapsules();

	capsForVertices.clear();
	capsForVertices.resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		capsForVertices[i].clear();
	}
	DistributeVertices();

	verticesForCaps.clear();
	verticesForCaps.resize(caps.size());
	for (int i = 0; i < caps.size(); i++) {
		verticesForCaps[i].clear();
	}
	for (int i = 0; i < vertices.size(); i++) {
		verticesForCaps[(*capsForVertices[i].begin()).second].push_back((UINT)i);
	}
}

void Skeleton::MutationAll() {
	for (int n = 0; n < 125; n++) {
		if (n % 9 == 6 || n % 9 == 7 || (n > 9 && n % 9 == 2)) {
			float val = (float)((float)rand() / RAND_MAX) * 1.5f + 0.5;
			parameters[n] = val * parameters[n];
		}
		else {
			float val = (float)((float)rand() / RAND_MAX) * 2.0f;
			parameters[n] = parameters[n] * (val - 1.0f);//  * (val*2.0f - 2.0f); //(val - 1.0f);// * *(parameters[n]);
		}
	}
}

void Skeleton::OneGenMutation(int n)
{
 	if (n % 9 == 6 || n % 9 == 7 || (n > 9 && n % 9 == 2)) {
		float val = (float)((float)rand() / RAND_MAX) * 1.5f + 0.5;
		parameters[n] = val * parameters[n];
	}
	else {
		float val = (float)((float)rand() / RAND_MAX) * 2.0f;
		parameters[n] = parameters[n] * (val - 1.0f);// *(val*2.0f - 2.0f); // * *(parameters[n]);
	}
}

void Skeleton::OneCapsuleMutation(int n)
{
  	if (n == 0) {
		parameters[0] = ((float)((float)rand() / RAND_MAX) * 1.5f + 0.5) * parameters[0];
		parameters[1] = ((float)((float)rand() / RAND_MAX) * 1.5f + 0.5) * parameters[1];
		parameters[2] = ((float)((float)rand() / RAND_MAX) * 1.5f + 0.5) * parameters[2];

		parameters[3] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) * parameters[6];
		parameters[4] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) * parameters[7];
		parameters[5] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) * parameters[8];
		parameters[6] = ((float)((float)rand() / RAND_MAX) * 1.5f + 0.5) * parameters[6];
		parameters[7] = ((float)((float)rand() / RAND_MAX) * 1.5f + 0.5) * parameters[7];
	}
	else {
		parameters[9 * n - 1] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) * parameters[9 * n - 1];
		parameters[9 * n + 0] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) * parameters[9 * n - 0];
		parameters[9 * n + 1] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) * parameters[9 * n + 1];
		parameters[9 * n + 2] = ((float)((float)rand() / RAND_MAX) * 1.5f + 0.5) * parameters[9 * n + 2];

		parameters[9 * n + 3] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) * parameters[9 * n + 3];
		parameters[9 * n + 4] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) * parameters[9 * n + 4];
		parameters[9 * n + 5] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) * parameters[9 * n + 5];
		parameters[9 * n + 6] = ((float)((float)rand() / RAND_MAX) * 1.5f + 0.5) * parameters[9 * n + 6];
		parameters[9 * n + 7] = ((float)((float)rand() / RAND_MAX) * 1.5f + 0.5) * parameters[9 * n + 7];
	}
}


void Skeleton::CorrectionSkeleton() {

	/*if (leftLeg0.shift.w > torso.radius || rightLeg0.shift.w > torso.radius) {
		if (leftLeg0.shift.w > torso.radius) leftLeg0.shift.w = torso.radius;
		if (rightLeg0.shift.w > torso.radius) rightLeg0.shift.w = torso.radius;
	}
	leftLeg1.shift.w = (leftLeg1.shift.w > leftLeg0.radius) ? leftLeg0.radius : leftLeg1.shift.w;
	leftLeg2.shift.w = (leftLeg2.shift.w > leftLeg1.radius) ? leftLeg1.radius : leftLeg2.shift.w;
	rightLeg1.shift.w = (rightLeg1.shift.w > rightLeg0.radius) ? rightLeg0.radius : rightLeg1.shift.w;
	rightLeg2.shift.w = (rightLeg2.shift.w > rightLeg1.radius) ? rightLeg1.radius : rightLeg2.shift.w;

	if (leftArm0.shift.w > torso.radius || rightArm0.shift.w > torso.radius || head.shift.w > torso.radius) {
		if (leftArm0.shift.w > torso.radius) leftArm0.shift.w = torso.radius;
		if (rightArm0.shift.w > torso.radius) rightArm0.shift.w = torso.radius;
		if (head.shift.w > torso.radius) head.shift.w = torso.radius;
	}
	leftArm1.shift.w = (leftArm1.shift.w > leftArm0.radius) ? leftArm0.radius : leftArm1.shift.w;
	leftArm2.shift.w = (leftArm2.shift.w > leftArm1.radius) ? leftArm1.radius : leftArm2.shift.w;
	rightArm1.shift.w = (rightArm1.shift.w > rightArm0.radius) ? rightArm0.radius : rightArm1.shift.w;
	rightArm2.shift.w = (rightArm2.shift.w > rightArm1.radius) ? rightArm1.radius : rightArm2.shift.w;*/
}

void Skeleton::DrawSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency)
{
	for (int i = 0; i < caps.size(); i++)
		caps[i]->DrawCaps(world, light, transparency, i);
}

void Skeleton::DistributeVertices() {
	for (int i = 0; i < vertices.size(); i++) {
		for (int j = 0; j < caps.size(); j++) {
			float d = caps[j]->DistanceToPoint(vertices[i]);
			if (d < 0)
				d = -0.1f * j;
			capsForVertices[i].insert({ d, (UINT)j });
		}
	}
}

int Skeleton::GetCapsCount() {
	return caps.size();
}

void Skeleton::GetVertices(std::vector<DirectX::XMFLOAT3> *dstVertices) {
	dstVertices->clear();
	dstVertices->resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++)
		(*dstVertices)[i] = vertices[i];
}

void Skeleton::GetLineModelIndices(std::vector<UINT32> *dstLineModelIndices) {
	dstLineModelIndices->clear();
	dstLineModelIndices->resize(lineList.size());
	for (int i = 0; i < lineList.size(); i++)
		(*dstLineModelIndices)[i] = lineList[i];
}

void Skeleton::GetCaps(std::vector<Capsule*> *dstCaps, DirectX::XMFLOAT3 color) {
	dstCaps->clear();
	dstCaps->resize(caps.size());
	for (int i = 0; i < caps.size(); i++)
		(*dstCaps)[i] = new Capsule(caps[i], color);
}

Skeleton::Skeleton(PersonPattern* homo, std::vector<DirectX::XMFLOAT3> vertices, std::vector<UINT32> lineModelIndices, DirectX::XMFLOAT3 color)
{
	this->color = color;
	homo->GetParameters(&parameters);

	caps.clear();
	caps.resize(homo->GetCapsCount());
	for (int i = 0; i < caps.size(); i++)
		caps[i] = new Capsule(color);
	UpdateForNewParameters();

	this->vertices.clear();
	this->vertices.resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++)
		this->vertices[i] = vertices[i];

	lineList.clear();
	lineList.resize(lineModelIndices.size());
	for (int i = 0; i < lineModelIndices.size(); i++)
		lineList[i] = lineModelIndices[i];

	capsForVertices.clear();
	capsForVertices.resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		capsForVertices[i].clear();
	}
	DistributeVertices();

	verticesForCaps.clear();
	verticesForCaps.resize(caps.size());
	for (int i = 0; i < caps.size(); i++) {
		verticesForCaps[i].clear();
	}
	for (int i = 0; i < vertices.size(); i++) {
		verticesForCaps[(*capsForVertices[i].begin()).second].push_back((UINT)i);
	}
}

Skeleton::Skeleton(stCapsule* pCaps, int nCaps, std::vector<DirectX::XMFLOAT3> vertices, std::vector<UINT32> lineModelIndices, DirectX::XMFLOAT3 color)
{
	this->color = color;

	caps.clear();
	caps.resize(nCaps);
	for (int i = 0; i < caps.size(); i++)
		caps[i] = new Capsule(&(pCaps[i]), color);
	
	parameters.clear();
	parameters.resize(nCaps * 9 - 1);
	UpdateForNewCapsules();

	this->vertices.clear();
	this->vertices.resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++)
		this->vertices[i] = vertices[i];

	lineList.clear();
	lineList.resize(lineModelIndices.size());
	for (int i = 0; i < lineModelIndices.size(); i++)
		lineList[i] = lineModelIndices[i];

	capsForVertices.clear();
	capsForVertices.resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		capsForVertices[i].clear();
	}
	DistributeVertices();

	verticesForCaps.clear();
	verticesForCaps.resize(caps.size());
	for (int i = 0; i < caps.size(); i++) {
		verticesForCaps[i].clear();
	}
	for (int i = 0; i < vertices.size(); i++) {
		verticesForCaps[(*capsForVertices[i].begin()).second].push_back((UINT)i);
	}
}

Skeleton::Skeleton(Skeleton * parent0, Skeleton * parent1, DirectX::XMFLOAT3 color)
{
	this->color = color;

	std::random_device randomDevice;
	std::mt19937 generator(randomDevice());
	std::normal_distribution<float> distribution(0.5f, 1.0f / 6.0f);


	parameters.clear();
	parameters.resize(parent0->parameters.size());
	float val = 0.0f;
	for (int i = 0; i < parent0->parameters.size(); i++) {
		val = distribution(generator);
		parameters[i] = val * (parent0->parameters[i]) + (1.0f - val) * (parent1->parameters[i]);
	}

	caps.clear();
	caps.resize(parent0->GetCapsCount());
	for (int i = 0; i < caps.size(); i++)
		caps[i] = new Capsule(color);
	UpdateForNewParameters();

	CorrectionSkeleton();
	UpdateForNewCapsules();

	parent0->GetVertices(&vertices);
	parent0->GetLineModelIndices(&lineList);

	capsForVertices.clear();
	capsForVertices.resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		capsForVertices[i].clear();
	}
	DistributeVertices();

	verticesForCaps.clear();
	verticesForCaps.resize(caps.size());
	for (int i = 0; i < caps.size(); i++) {
		verticesForCaps[i].clear();
	}
	for (int i = 0; i < vertices.size(); i++) {
		verticesForCaps[(*capsForVertices[i].begin()).second].push_back((UINT)i);
	}
}

Skeleton::Skeleton(Skeleton * parent, bool onlyCopy, DirectX::XMFLOAT3 color)
{
	this->color = color;

	parent->GetCaps(&caps, color);

	parameters.clear();
	parameters.resize(parent->parameters.size());
	UpdateForNewCapsules();

	parent->GetVertices(&vertices);
	parent->GetLineModelIndices(&lineList);

	capsForVertices.clear();
	capsForVertices.resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		capsForVertices[i].clear();
	}
	DistributeVertices();

	verticesForCaps.clear();
	verticesForCaps.resize(caps.size());
	for (int i = 0; i < caps.size(); i++) {
		verticesForCaps[i].clear();
	}
	for (int i = 0; i < vertices.size(); i++) {
		verticesForCaps[(*capsForVertices[i].begin()).second].push_back((UINT)i);
	}

	if (!onlyCopy) {
		OptimizeCapsules();
		CorrectionSkeleton();
		UpdateForNewCapsules();

		capsForVertices.clear();
		capsForVertices.resize(vertices.size());
		for (int i = 0; i < vertices.size(); i++) {
			capsForVertices[i].clear();
		}
		DistributeVertices();

		verticesForCaps.clear();
		verticesForCaps.resize(caps.size());
		for (int i = 0; i < caps.size(); i++) {
			verticesForCaps[i].clear();
		}
		for (int i = 0; i < vertices.size(); i++) {
			verticesForCaps[(*capsForVertices[i].begin()).second].push_back((UINT)i);
		}
	}

}

Skeleton::~Skeleton()
{
	for (int i = 0; i < caps.size(); i++)
		delete caps[i];
}

float Skeleton::GetVolume() {
	float volume = 0.0f;
	for (int i = 0; i < caps.size(); i++)
		volume += caps[i]->GetVolume();
 	return volume;
}

void Skeleton::GetSkeletStruct(stCapsule ** pCapsule)
{
	if (*pCapsule == nullptr)
		(*pCapsule) = new stCapsule[caps.size()];
	for (int i = 0; i < caps.size(); i++)
		(*pCapsule)[i] = caps[i]->GetCapsule();
}

void Skeleton::UpdateOneCapsule(DirectX::XMFLOAT4* cCur, DirectX::XMFLOAT4 cPrev, int nFirstParam, bool isShiftInterpret) {
	float length = sqrt(parameters[nFirstParam] * parameters[nFirstParam] + parameters[nFirstParam+1] * parameters[nFirstParam+1] + parameters[nFirstParam+2] * parameters[nFirstParam+2]);
	cCur->x = cPrev.x + parameters[nFirstParam+0] * parameters[nFirstParam+3] / length;
	cCur->y = cPrev.y + parameters[nFirstParam+1] * parameters[nFirstParam+3] / length;
	cCur->z = cPrev.z + parameters[nFirstParam+2] * parameters[nFirstParam+3] / length;
	if(isShiftInterpret)
		cCur->w = parameters[nFirstParam + 8];
	else
		cCur->w = parameters[nFirstParam + 4];
}

void Skeleton::UpdateForNewParameters()
{
	XMFLOAT4 p0, p1;
	p0.x = parameters[0];
	p0.y = parameters[1];
	p0.z = parameters[2];
	p0.w = parameters[7];

	UpdateOneCapsule(&p1, p0, 3, false);
	caps[0]->Init(p0, p1, 2);
	
	for (int i = 1, k = 8; i < 13; i++, k +=9 ) {
		if(i == 1 || i == 4)  //begin legs
			UpdateOneCapsule(&p0, caps[0]->p0, k, true);
		else {
			if (i == 7 || i == 10)  //begin arms
				UpdateOneCapsule(&p0, caps[0]->p1, k, true);
			else
				UpdateOneCapsule(&p0, caps[i - 1]->p1, k, true);
		}
		UpdateOneCapsule(&p1, p0, k+4, false);
		caps[i]->Init(p0, p1, 2);
	}

	//head
	UpdateOneCapsule(&p0, caps[0]->p1, 116, true);
	UpdateOneCapsule(&p1, p0, 120, false);
	caps[13]->Init(p0, p1, 2);
	return;
	}

void Skeleton::UpdateOneParametrGroup(int nPrevCaps, int nCurCapsule, bool isFirstLegCaps, int startNum) {
	XMFLOAT3 dir;
	if (isFirstLegCaps)
		dir = Float3Substruct(caps[nCurCapsule]->p0, caps[nPrevCaps]->p0);
	else
		dir = Float3Substruct(caps[nCurCapsule]->p0, caps[nPrevCaps]->p1);
	if (Float3Length(dir) == 0) {
		parameters[startNum + 0] = 1.0f;
		parameters[startNum + 1] = 1.0f;
		parameters[startNum + 2] = 1.0f;
	}
	else {
		parameters[startNum + 0] = dir.x;
		parameters[startNum + 1] = dir.y;
		parameters[startNum + 2] = dir.z;
	}
	parameters[startNum + 3] = Float3Length(dir);

	dir = Float3Substruct(caps[nCurCapsule]->p1, caps[nCurCapsule]->p0);
	parameters[startNum + 4] = dir.x;
	parameters[startNum + 5] = dir.y;
	parameters[startNum + 6] = dir.z;
	parameters[startNum + 7] = Float3Length(dir);
	parameters[startNum + 8] = caps[nCurCapsule]->p0.w;
}

void Skeleton::UpdateForNewCapsules()
{
	parameters[0] = caps[0]->p0.x;
	parameters[1] = caps[0]->p0.y;
	parameters[2] = caps[0]->p0.z;

	XMFLOAT3 dir = Float3Substruct(caps[0]->p1, caps[0]->p0);
	parameters[3] = dir.x;
	parameters[4] = dir.y;
	parameters[5] = dir.z;
	parameters[6] = Float3Length(dir);
	parameters[7] = caps[0]->p0.w;

	//left leg
	UpdateOneParametrGroup(0, 1, true, 8);
	UpdateOneParametrGroup(1, 2, false, 17);
	UpdateOneParametrGroup(2, 3, false, 26);

	//right leg
	UpdateOneParametrGroup(0, 4, true, 35);
	UpdateOneParametrGroup(4, 5, false, 44);
	UpdateOneParametrGroup(5, 6, false, 53);

	//left arm
	UpdateOneParametrGroup(0, 7, false, 62);
	UpdateOneParametrGroup(7, 8, false, 71);
	UpdateOneParametrGroup(8, 9, false, 80);

	//right arm
	UpdateOneParametrGroup(0, 10, false, 89);
	UpdateOneParametrGroup(10, 11, false, 98);
	UpdateOneParametrGroup(11, 12, false, 107);

	//head
	UpdateOneParametrGroup(0, 13, false, 116);
}
