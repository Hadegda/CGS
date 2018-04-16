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

	/*DirectX::XMFLOAT4* capsules;
	capsules = new DirectX::XMFLOAT4[28];*/

	caps.clear();
	caps.resize(homo->GetCapsCount());
	for (int i = 0; i < caps.size(); i++)
		caps[i] = new Capsule(color);
	UpdateForNewParameters();

	/*caps.clear();
	caps.resize(homo->GetCapsCount());
	for (int i = 0; i < caps.size(); i++)
		caps[i] = new Capsule(color);
	for (int i = 0; i < 14; i++)
		caps[i]->Init(capsules[2 * i], capsules[2 * i + 1]);

	delete[] capsules;*/
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
	
	//left leg
	UpdateOneCapsule(&p0, caps[0]->p0, 8, true);
	UpdateOneCapsule(&p1, p0, 12, false);
	caps[1]->Init(p0, p1);

	UpdateOneCapsule(&p0, caps[1]->p1, 17, true);
	UpdateOneCapsule(&p1, p0, 21, false);
	caps[2]->Init(p0, p1);

	UpdateOneCapsule(&p0, caps[2]->p1, 26, true);
	UpdateOneCapsule(&p1, p0, 30, false);
	caps[3]->Init(p0, p1);

	//right leg
	UpdateOneCapsule(&p0, caps[0]->p0, 35, true);
	UpdateOneCapsule(&p1, p0, 39, false);
	caps[4]->Init(p0, p1);

	UpdateOneCapsule(&p0, caps[4]->p1, 44, true);
	UpdateOneCapsule(&p1, p0, 48, false);
	caps[5]->Init(p0, p1);

	UpdateOneCapsule(&p0, caps[5]->p1, 53, true);
	UpdateOneCapsule(&p1, p0, 57, false);
	caps[6]->Init(p0, p1);

	//left Arm
	UpdateOneCapsule(&p0, caps[0]->p1, 62, true);
	UpdateOneCapsule(&p1, p0, 66, false);
	caps[7]->Init(p0, p1);

	UpdateOneCapsule(&p0, caps[7]->p1, 71, true);
	UpdateOneCapsule(&p1, p0, 75, false);
	caps[8]->Init(p0, p1);

	UpdateOneCapsule(&p0, caps[8]->p1, 80, true);
	UpdateOneCapsule(&p1, p0, 84, false);
	caps[9]->Init(p0, p1);

	//right Arm
	UpdateOneCapsule(&p0, caps[0]->p1, 89, true);
	UpdateOneCapsule(&p1, p0, 93, false);
	caps[10]->Init(p0, p1);

	UpdateOneCapsule(&p0, caps[10]->p1, 98, true);
	UpdateOneCapsule(&p1, p0, 102, false);
	caps[11]->Init(p0, p1);

	UpdateOneCapsule(&p0, caps[11]->p1, 107, true);
	UpdateOneCapsule(&p1, p0, 111, false);
	caps[12]->Init(p0, p1);

	//head
	UpdateOneCapsule(&p0, caps[0]->p1, 116, true);
	UpdateOneCapsule(&p1, p0, 120, false);
	caps[13]->Init(p0, p1);
	return;
	}