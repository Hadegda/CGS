#include "skeleton.h"
#include <random>

using namespace DirectX;

bool Skeleton::CreateFromParents(Skeleton * parent0, Skeleton * parent1, float impact0, float impact1)
{
	/*std::default_random_engine generator;
	std::normal_distribution<float> distribution(0.5f, 0.25f);

	torso.shift = XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f };
	for (int i = 0; i < 125; i++) {
		float val = distribution(generator);
		*(parameters[i]) = val * (*(parent0->parameters[i])) + (1.0f-val) * (*(parent1->parameters[i]));
	}
	CorrectionOnRadius();*/
	return true;
}

void Skeleton::Mutation(int level, int n)
{
	if (level == 0) {
		MutationAll();
	}
	else {
		if (level == 1)
			OneCapsuleMutation(n);
		else
			OneGenMutation(n);
	}

	CorrectionOnRadius();
}

void Skeleton::MutationAll()
{
	std::default_random_engine generator;
	std::normal_distribution<float> distribution(1.0f, 1.0f);

	for (int n = 0; n < 125; n++) {
		if (n % 9 == 6 || n % 9 == 7 || (n > 9 && n % 9 == 2)) {
			float val = (float)((float)rand() / RAND_MAX) * 2.0f;
			parameters[n] = val * parameters[n];
		}
		else {
			float val = (float)((float)rand() / RAND_MAX) * 2.0f;
			parameters[n] = parameters[n] + (val - 1.0f) / 2.0f;//  * (val*2.0f - 2.0f); //(val - 1.0f);// * *(parameters[n]);
		}
	}
}

void Skeleton::OneGenMutation(int n)
{
 	if (n % 9 == 6 || n % 9 == 7 || (n > 9 && n % 9 == 2)) {
		float val = (float)((float)rand() / RAND_MAX) * 2.0f;
		parameters[n] = val * parameters[n];
	}
	else {
		float val = (float)((float)rand() / RAND_MAX) * 2.0f;
		parameters[n] = parameters[n] + (val - 1.0f) / 2.0f;// *(val*2.0f - 2.0f); // * *(parameters[n]);
	}
}

void Skeleton::OneCapsuleMutation(int n)
{
  	if (n == 0) {
		parameters[0] = (float)((float)rand() / RAND_MAX) * 2.0f * parameters[0];
		parameters[1] = (float)((float)rand() / RAND_MAX) * 2.0f * parameters[1];
		parameters[2] = (float)((float)rand() / RAND_MAX) * 2.0f * parameters[2];

		parameters[3] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + parameters[6];
		parameters[4] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + parameters[7];
		parameters[5] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + parameters[8];
		parameters[6] = (float)((float)rand() / RAND_MAX) * 2.0f * parameters[6];
		parameters[7] = (float)((float)rand() / RAND_MAX) * 2.0f * parameters[7];
	}
	else {
		parameters[9 * n - 1] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + parameters[9 * n - 1];
		parameters[9 * n + 0] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + parameters[9 * n - 0];
		parameters[9 * n + 1] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + parameters[9 * n + 1];
		parameters[9 * n + 2] = (float)((float)rand() / RAND_MAX) * 2.0f * parameters[9 * n + 2];

		parameters[9 * n + 3] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f +  parameters[9 * n + 3];
		parameters[9 * n + 4] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f +  parameters[9 * n + 4];
		parameters[9 * n + 5] = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f +  parameters[9 * n + 5];
		parameters[9 * n + 6] = (float)((float)rand() / RAND_MAX) * 2.0f * parameters[9 * n + 6];
		parameters[9 * n + 7] = (float)((float)rand() / RAND_MAX) * 2.0f * parameters[9 * n + 7];
	}
}

void Skeleton::CorrectionOnRadius() {
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

void Skeleton::CorrectionOnMaxShift() {
	/*if (leftLeg0.shift.w > torso.radius || rightLeg0.shift.w > torso.radius) {
		torso.radius = (leftLeg0.shift.w > rightLeg0.shift.w) ? leftLeg0.shift.w : rightLeg0.shift.w;
	}
	leftLeg0.radius = (leftLeg1.shift.w > leftLeg0.radius) ? leftLeg1.shift.w : leftLeg0.radius;
	leftLeg1.radius = (leftLeg2.shift.w > leftLeg1.radius) ? leftLeg2.shift.w : leftLeg1.radius;
	rightLeg0.radius = (rightLeg1.shift.w > rightLeg0.radius) ? rightLeg1.shift.w : rightLeg0.radius;
	rightLeg1.radius = (rightLeg2.shift.w > rightLeg1.radius) ? rightLeg2.shift.w : rightLeg1.radius;

	if (leftArm0.shift.w > torso.radius || rightArm0.shift.w > torso.radius || head.shift.w > torso.radius) {
		torso.radius = (leftArm0.shift.w > rightArm0.shift.w) ? leftArm0.shift.w : rightArm0.shift.w;
		torso.radius = (head.shift.w > torso.radius) ? head.shift.w : torso.radius;
	}
	leftArm0.radius = (leftArm1.shift.w > leftArm0.radius) ? leftArm1.shift.w : leftArm0.radius;
	leftArm1.radius = (leftArm2.shift.w > leftArm1.radius) ? leftArm2.shift.w : leftArm1.radius;
	rightArm0.radius = (rightArm1.shift.w > rightArm0.radius) ? rightArm1.shift.w : rightArm0.radius;
	rightArm1.radius = (rightArm2.shift.w > rightArm1.radius) ? rightArm2.shift.w : rightArm1.radius;*/
}

void Skeleton::DistributeVertices() {
	
}

void Skeleton::DrawSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency)
{
	for (int i = 0; i < caps.size(); i++)
		caps[i]->DrawCaps(world, light, transparency, i);
}

Skeleton::Skeleton(PersonPattern* homo, DirectX::XMFLOAT3 color)
{
	this->color = color;
	homo->GetParameters(&parameters);

	caps.clear();
	caps.resize(homo->GetCapsCount());
	for (int i = 0; i < caps.size(); i++)
		caps[i] = new Capsule(color);
	UpdateForNewParameters();
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
	caps[0]->Init(p0, p1);
	
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
		caps[i]->Init(p0, p1);
	}

	//head
	UpdateOneCapsule(&p0, caps[0]->p1, 116, true);
	UpdateOneCapsule(&p1, p0, 120, false);
	caps[13]->Init(p0, p1);
	return;
	}