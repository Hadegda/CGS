#include "skeleton.h"
#include <random>

using namespace DirectX;

bool Skeleton::CreateStandart()
{
	shift = XMFLOAT3{ 0.0f, 0.0f, 0.0f };

	torso = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, 1.0f, 0.24f }, 0.16f };
	leftLeg0 = { XMFLOAT4{ 0.0f, 1.0f, 0.0f, 0.1f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.22f }, 0.07f };
	leftLeg1 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.22f }, 0.06f };
	leftLeg2 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 1.0f, 0.0f, 0.0f, 0.05f }, 0.06f };
	rightLeg0 = { XMFLOAT4{ 0.0f, -1.0f, 0.0f, 0.1f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.22f }, 0.07f };
	rightLeg1 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.22f }, 0.06f };
	rightLeg2 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 1.0f, 0.0f, 0.0f, 0.05f }, 0.06f };
	leftArm0 = { XMFLOAT4{ 0.0f, 1.0f, 0.4f, 0.14f }, XMFLOAT4{ 0.0f, 1.0f, 0.0f, 0.2f }, 0.05f };
	leftArm1 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.2f },0.05f };
	leftArm2 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.2f }, 0.05f };
	rightArm0 = { XMFLOAT4{ 0.0f, -1.0f, 0.4f, 0.14f }, XMFLOAT4{ 0.0f, -1.0f, 0.0f, 0.2f }, 0.05f };
	rightArm1 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.2f }, 0.05f };
	rightArm2 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.2f }, 0.05f };
	head = { XMFLOAT4{ 0.0f, 0.0f, 1.0f, 0.16f }, XMFLOAT4{ 0.0f, 0.0f, 1.0f, 0.02f }, 0.08f };
	return true;
}

bool Skeleton::CreateFromParents(Skeleton * parent0, Skeleton * parent1, float impact0, float impact1)
{
	std::default_random_engine generator;
	std::normal_distribution<float> distribution(0.5f, 0.25f);

	torso.shift = XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f };
	for (int i = 0; i < 125; i++) {
		float val = distribution(generator);
		*(parameters[i]) = val * (*(parent0->parameters[i])) + (1.0f-val) * (*(parent1->parameters[i]));
	}
	CorrectionOnRadius();
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
			*(parameters[n]) = val * *(parameters[n]);
		}
		else {
			float val = (float)((float)rand() / RAND_MAX) * 2.0f;
			*(parameters[n]) = *(parameters[n]) + (val - 1.0f) / 2.0f;//  * (val*2.0f - 2.0f); //(val - 1.0f);// * *(parameters[n]);
		}
	}
}

void Skeleton::OneGenMutation(int n)
{
 	if (n % 9 == 6 || n % 9 == 7 || (n > 9 && n % 9 == 2)) {
		float val = (float)((float)rand() / RAND_MAX) * 2.0f;
		*(parameters[n]) = val * *(parameters[n]);
	}
	else {
		float val = (float)((float)rand() / RAND_MAX) * 2.0f;
		*(parameters[n]) = *(parameters[n]) + (val - 1.0f) / 2.0f;// *(val*2.0f - 2.0f); // * *(parameters[n]);
	}
}

void Skeleton::OneCapsuleMutation(int n)
{
  	if (n == 0) {
		*(parameters[0]) = (float)((float)rand() / RAND_MAX) * 2.0f * *(parameters[0]);
		*(parameters[1]) = (float)((float)rand() / RAND_MAX) * 2.0f * *(parameters[1]);
		*(parameters[2]) = (float)((float)rand() / RAND_MAX) * 2.0f * *(parameters[2]);

		*(parameters[3]) = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + *(parameters[6]);
		*(parameters[4]) = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + *(parameters[7]);
		*(parameters[5]) = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + *(parameters[8]);
		*(parameters[6]) = (float)((float)rand() / RAND_MAX) * 2.0f * *(parameters[6]);
		*(parameters[7]) = (float)((float)rand() / RAND_MAX) * 2.0f * *(parameters[7]);
	}
	else {
		*(parameters[9 * n - 1]) = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + *(parameters[9 * n - 1]);
		*(parameters[9 * n + 0]) = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + *(parameters[9 * n - 0]);
		*(parameters[9 * n + 1]) = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f + *(parameters[9 * n + 1]);
		*(parameters[9 * n + 2]) = (float)((float)rand() / RAND_MAX) * 2.0f * *(parameters[9 * n + 2]);

		*(parameters[9 * n + 3]) = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f +  *(parameters[9 * n + 3]);
		*(parameters[9 * n + 4]) = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f +  *(parameters[9 * n + 4]);
		*(parameters[9 * n + 5]) = ((float)((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f +  *(parameters[9 * n + 5]);
		*(parameters[9 * n + 6]) = (float)((float)rand() / RAND_MAX) * 2.0f * *(parameters[9 * n + 6]);
		*(parameters[9 * n + 7]) = (float)((float)rand() / RAND_MAX) * 2.0f * *(parameters[9 * n + 7]);
	}
}

void Skeleton::CorrectionOnRadius() {
	if (leftLeg0.shift.w > torso.radius || rightLeg0.shift.w > torso.radius) {
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
	rightArm2.shift.w = (rightArm2.shift.w > rightArm1.radius) ? rightArm1.radius : rightArm2.shift.w;
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

void Skeleton::Scale(float scale){
	int i;
	for (i = 6; i < 125; i += 9)
		*(parameters[i]) *= scale;
	for (i = 7; i < 125; i += 9)
		*(parameters[i]) *= scale;

	for (i = 11; i < 125; i += 9)
		*(parameters[i]) *= scale;
}

void Skeleton::DropForward(){
	float tmp;
	int i;
	for (i = 5; i < 125; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 2]);
		*(parameters[i - 2]) = tmp;
	}
	for (i = 10; i < 125; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 2]);
		*(parameters[i - 2]) = tmp;
	}
}

void Skeleton::DropBack(){
	float tmp;
	int i;
	for (i = 5; i < 125; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 2]);
		*(parameters[i - 2]) = -tmp;
	}
	for (i = 10; i < 125; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 2]);
		*(parameters[i - 2]) = -tmp;
	}
}

void Skeleton::DropLeft(){
	float tmp;
	int i;
	for (i = 5; i < 125; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 1]);
		*(parameters[i - 1]) = -tmp;
	}
	for (i = 10; i < 125; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 1]);
		*(parameters[i - 1]) = -tmp;
	}
}

void Skeleton::DropRight() {
	float tmp;
	int i;
	for (i = 5; i < 125; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 1]);
		*(parameters[i - 1]) = tmp;
	}
	for (i = 10; i < 125; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 1]);
		*(parameters[i - 1]) = tmp;
	}
}

void Skeleton::InvertY(){
	int i;
	for (i = 5; i < 125; i += 9) {
		*(parameters[i]) = -*(parameters[i]);
	}
	for (i = 10; i < 125; i += 9) {
		*(parameters[i]) = -*(parameters[i]);
	}
}

void Skeleton::Centered(DirectX::XMFLOAT3 center)
{
	*(parameters[0]) = center.x;
	*(parameters[1]) = center.y;
	*(parameters[2]) = center.z;
}

void Skeleton::CreateCaps(){
	for (int i = 0; i < caps.size(); i++) {
		if(caps[i]) delete caps[i];
		caps[i] = new Capsule(color);
	}

	caps[0]->Init(torso.radius, torso.partOfBody.w, XMFLOAT3{ -1.0f, 0.0f, 0.0f });
	caps[1]->Init(leftLeg0.radius, leftLeg0.partOfBody.w, XMFLOAT3{ -1.0f, 1.0f, 1.0f });
	caps[2]->Init(leftLeg1.radius, leftLeg1.partOfBody.w, XMFLOAT3{ -1.0f, 0.5f, 1.0f });
	caps[3]->Init(leftLeg2.radius, leftLeg2.partOfBody.w, XMFLOAT3{ -1.0f, 0.5f, 1.0f });
	caps[4]->Init(rightLeg0.radius, rightLeg0.partOfBody.w, XMFLOAT3{ -1.0f, 0.0f, 1.0f });
	caps[5]->Init(rightLeg1.radius, rightLeg1.partOfBody.w, XMFLOAT3{ -0.5f, 0.0f, 1.0f });
	caps[6]->Init(rightLeg2.radius, rightLeg2.partOfBody.w, XMFLOAT3{ -0.5f, 0.0f, 1.0f });
	caps[7]->Init(leftArm0.radius, leftArm0.partOfBody.w, XMFLOAT3{ -0.7f, 1.0f, 0.3f });
	caps[8]->Init(leftArm1.radius, leftArm1.partOfBody.w, XMFLOAT3{ -0.9f, 1.0f, 0.1f });
	caps[9]->Init(leftArm2.radius, leftArm2.partOfBody.w, XMFLOAT3{ -0.9f, 1.0f, 0.1f });
	caps[10]->Init(rightArm0.radius, rightArm0.partOfBody.w, XMFLOAT3{ -0.3f, 1.0f, 0.7f });
	caps[11]->Init(rightArm1.radius, rightArm1.partOfBody.w, XMFLOAT3{ -0.1f, 1.0f, 0.9f });
	caps[12]->Init(rightArm2.radius, rightArm2.partOfBody.w, XMFLOAT3{ -0.1f, 1.0f, 0.9f });
	caps[13]->Init(head.radius, head.partOfBody.w, XMFLOAT3{ -1.0f, 1.0f, 0.0f });

	//capsInObjSpace
}

void Skeleton::DistributeVertices() {
	
}

XMMATRIX Rotation(XMFLOAT4 rotate, XMMATRIX* returnRot) {
	if (rotate.x*rotate.x + rotate.y*rotate.y + rotate.z*rotate.z != 0) {
		XMVECTOR newZaxis = XMVector4Normalize(XMVectorSet(rotate.x, rotate.y, rotate.z, 0.0f));
		XMVECTOR angleZ = XMVector3AngleBetweenVectors(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), newZaxis);
		float angleZRadians = XMVectorGetX(angleZ);
		XMVECTOR proj; 
		XMVECTOR perp; 
		XMVector3ComponentsFromNormal(&proj, &perp, newZaxis, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
		XMVECTOR angleY = XMVector3AngleBetweenVectors(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), perp);
		float angleYRadians = XMVectorGetX(angleY);
		if (rotate.y < 0)
			angleYRadians *= -1;
		
		*returnRot = XMMatrixRotationZ(-angleYRadians) * XMMatrixRotationY(-angleZRadians);
		return XMMatrixRotationY(angleZRadians) * XMMatrixRotationZ(angleYRadians);
	}
	*returnRot = XMMatrixIdentity();
	return XMMatrixIdentity();
}

void Skeleton::DrawSkeleton(DirectX::XMFLOAT4X4 world, float light, float transparency)
{
	DirectX::XMFLOAT4* capsules;
	capsules = new DirectX::XMFLOAT4[28];

	CapsulesInObjSys(&capsules);

	XMFLOAT4X4 mWorld;
	XMStoreFloat4x4(&mWorld, XMMatrixMultiply(XMLoadFloat4x4(&world), XMMatrixIdentity()));

	XMMATRIX retgm;
	for (int i = 0; i < 28; i+=2) {
		XMMATRIX gm = XMLoadFloat4x4(&world);
		XMFLOAT4 rot = XMFLOAT4(capsules[i + 1].x - capsules[i].x, capsules[i + 1].y - capsules[i].y, capsules[i + 1].z - capsules[i].z, 1.0f);
		gm = Rotation(rot, &retgm) * XMMatrixTranslation(capsules[i].x, capsules[i].y, capsules[i].z) * gm;
		XMStoreFloat4x4(&mWorld, gm);
		caps[i/2]->DrawCaps(mWorld, light, transparency, i / 2);
	}
}

Skeleton::Skeleton(DirectX::XMFLOAT3 color)
{
	caps.clear();
	caps.resize(14);
	for (int i = 0; i < caps.size(); i++)
		caps[i] = nullptr;

	vertices.clear();
	vertices.resize(caps.size());
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].clear();
	}

	this->color = color;

	parameters = new float*[125];
	for (int i = 0; i < 125; i++)
		parameters[i] = nullptr;

	parameters[0] = &shift.x; parameters[1] = &shift.y; parameters[2] = &shift.z;
	parameters[3] = &torso.partOfBody.x; parameters[4] = &torso.partOfBody.y; parameters[5] = &torso.partOfBody.z; parameters[6] = &torso.partOfBody.w;
	parameters[7] = &torso.radius;

	parameters[8] = &leftLeg0.shift.x; parameters[9] = &leftLeg0.shift.y; parameters[10] = &leftLeg0.shift.z; parameters[11] = &leftLeg0.shift.w;
	parameters[12] = &leftLeg0.partOfBody.x; parameters[13] = &leftLeg0.partOfBody.y; parameters[14] = &leftLeg0.partOfBody.z; parameters[15] = &leftLeg0.partOfBody.w;
	parameters[16] = &leftLeg0.radius;

	parameters[17] = &leftLeg1.shift.x; parameters[18] = &leftLeg1.shift.y; parameters[19] = &leftLeg1.shift.z; parameters[20] = &leftLeg1.shift.w;
	parameters[21] = &leftLeg1.partOfBody.x; parameters[22] = &leftLeg1.partOfBody.y; parameters[23] = &leftLeg1.partOfBody.z; parameters[24] = &leftLeg1.partOfBody.w;
	parameters[25] = &leftLeg1.radius;

	parameters[26] = &leftLeg2.shift.x; parameters[27] = &leftLeg2.shift.y; parameters[28] = &leftLeg2.shift.z; parameters[29] = &leftLeg2.shift.w;
	parameters[30] = &leftLeg2.partOfBody.x; parameters[31] = &leftLeg2.partOfBody.y; parameters[32] = &leftLeg2.partOfBody.z; parameters[33] = &leftLeg2.partOfBody.w;
	parameters[34] = &leftLeg2.radius;

	parameters[35] = &rightLeg0.shift.x; parameters[36] = &rightLeg0.shift.y; parameters[37] = &rightLeg0.shift.z; parameters[38] = &rightLeg0.shift.w;
	parameters[39] = &rightLeg0.partOfBody.x; parameters[40] = &rightLeg0.partOfBody.y; parameters[41] = &rightLeg0.partOfBody.z; parameters[42] = &rightLeg0.partOfBody.w;
	parameters[43] = &rightLeg0.radius;

	parameters[44] = &rightLeg1.shift.x; parameters[45] = &rightLeg1.shift.y; parameters[46] = &rightLeg1.shift.z; parameters[47] = &rightLeg1.shift.w;
	parameters[48] = &rightLeg1.partOfBody.x; parameters[49] = &rightLeg1.partOfBody.y; parameters[50] = &rightLeg1.partOfBody.z; parameters[51] = &rightLeg1.partOfBody.w;
	parameters[52] = &rightLeg1.radius;

	parameters[53] = &rightLeg2.shift.x; parameters[54] = &rightLeg2.shift.y; parameters[55] = &rightLeg2.shift.z; parameters[56] = &rightLeg2.shift.w;
	parameters[57] = &rightLeg2.partOfBody.x; parameters[58] = &rightLeg2.partOfBody.y; parameters[59] = &rightLeg2.partOfBody.z; parameters[60] = &rightLeg2.partOfBody.w;
	parameters[61] = &rightLeg2.radius;

	parameters[62] = &leftArm0.shift.x; parameters[63] = &leftArm0.shift.y; parameters[64] = &leftArm0.shift.z; parameters[65] = &leftArm0.shift.w;
	parameters[66] = &leftArm0.partOfBody.x; parameters[67] = &leftArm0.partOfBody.y; parameters[68] = &leftArm0.partOfBody.z; parameters[69] = &leftArm0.partOfBody.w;
	parameters[70] = &leftArm0.radius;

	parameters[71] = &leftArm1.shift.x; parameters[72] = &leftArm1.shift.y; parameters[73] = &leftArm1.shift.z; parameters[74] = &leftArm1.shift.w;
	parameters[75] = &leftArm1.partOfBody.x; parameters[76] = &leftArm1.partOfBody.y; parameters[77] = &leftArm1.partOfBody.z; parameters[78] = &leftArm1.partOfBody.w;
	parameters[79] = &leftArm1.radius;

	parameters[80] = &leftArm2.shift.x; parameters[81] = &leftArm2.shift.y; parameters[82] = &leftArm2.shift.z; parameters[83] = &leftArm2.shift.w;
	parameters[84] = &leftArm2.partOfBody.x; parameters[85] = &leftArm2.partOfBody.y; parameters[86] = &leftArm2.partOfBody.z; parameters[87] = &leftArm2.partOfBody.w;
	parameters[88] = &leftArm2.radius;

	parameters[89] = &rightArm0.shift.x; parameters[90] = &rightArm0.shift.y; parameters[91] = &rightArm0.shift.z; parameters[92] = &rightArm0.shift.w;
	parameters[93] = &rightArm0.partOfBody.x; parameters[94] = &rightArm0.partOfBody.y; parameters[95] = &rightArm0.partOfBody.z; parameters[96] = &rightArm0.partOfBody.w;
	parameters[97] = &rightArm0.radius;

	parameters[98] = &rightArm1.shift.x; parameters[99] = &rightArm1.shift.y; parameters[100] = &rightArm1.shift.z; parameters[101] = &rightArm1.shift.w;
	parameters[102] = &rightArm1.partOfBody.x; parameters[103] = &rightArm1.partOfBody.y; parameters[104] = &rightArm1.partOfBody.z; parameters[105] = &rightArm1.partOfBody.w;
	parameters[106] = &rightArm1.radius;

	parameters[107] = &rightArm2.shift.x; parameters[108] = &rightArm2.shift.y; parameters[109] = &rightArm2.shift.z; parameters[110] = &rightArm2.shift.w;
	parameters[111] = &rightArm2.partOfBody.x; parameters[112] = &rightArm2.partOfBody.y; parameters[113] = &rightArm2.partOfBody.z; parameters[114] = &rightArm2.partOfBody.w;
	parameters[115] = &rightArm2.radius;

	parameters[116] = &head.shift.x; parameters[117] = &head.shift.y; parameters[118] = &head.shift.z; parameters[119] = &head.shift.w;
	parameters[120] = &head.partOfBody.x; parameters[121] = &head.partOfBody.y; parameters[122] = &head.partOfBody.z; parameters[123] = &head.partOfBody.w;
	parameters[124] = &head.radius;
}

Skeleton::~Skeleton()
{
	for (int i = 0; i < caps.size(); i++)
		delete caps[i];
	delete[] parameters;
}

float SphereVolume(float r) {
	return (4.0f/3.0f) * XM_PI *r*r*r;
}

float CylinderVolume(float r, float h) {
	return h * XM_PI *r*r;
}

float Skeleton::GetVolume() {
	float volume = 0.0f;

	volume += SphereVolume(torso.radius) + CylinderVolume(torso.radius, torso.partOfBody.w);

	volume += SphereVolume(leftLeg0.radius) + CylinderVolume(leftLeg0.radius, leftLeg0.partOfBody.w);
	volume += SphereVolume(leftLeg1.radius) + CylinderVolume(leftLeg1.radius, leftLeg1.partOfBody.w);
	volume += SphereVolume(leftLeg2.radius) + CylinderVolume(leftLeg2.radius, leftLeg2.partOfBody.w);

	volume += SphereVolume(rightLeg0.radius) + CylinderVolume(rightLeg0.radius, rightLeg0.partOfBody.w);
	volume += SphereVolume(rightLeg1.radius) + CylinderVolume(rightLeg1.radius, rightLeg1.partOfBody.w);
	volume += SphereVolume(rightLeg2.radius) + CylinderVolume(rightLeg2.radius, rightLeg2.partOfBody.w);

	volume += SphereVolume(leftArm0.radius) + CylinderVolume(leftArm0.radius, leftArm0.partOfBody.w);
	volume += SphereVolume(leftArm1.radius) + CylinderVolume(leftArm1.radius, leftArm1.partOfBody.w);
	volume += SphereVolume(leftArm2.radius) + CylinderVolume(leftArm2.radius, leftArm2.partOfBody.w);

	volume += SphereVolume(rightArm0.radius) + CylinderVolume(rightArm0.radius, rightArm0.partOfBody.w);
	volume += SphereVolume(rightArm1.radius) + CylinderVolume(rightArm1.radius, rightArm1.partOfBody.w);
	volume += SphereVolume(rightArm2.radius) + CylinderVolume(rightArm2.radius, rightArm2.partOfBody.w);

	volume += SphereVolume(head.radius) + CylinderVolume(head.radius, head.partOfBody.w);

	return volume;
}

void Skeleton::CapsulesInObjSys(DirectX::XMFLOAT4 ** capsules)
{
	(*capsules)[0].x = shift.x;
	(*capsules)[0].y = shift.y;
	(*capsules)[0].z = shift.z;
	(*capsules)[0].w = torso.radius;

	float length = sqrt(torso.partOfBody.x * torso.partOfBody.x + torso.partOfBody.y * torso.partOfBody.y + torso.partOfBody.z * torso.partOfBody.z);
	(*capsules)[1].x = (*capsules)[0].x + torso.partOfBody.x * torso.partOfBody.w / length;
	(*capsules)[1].y = (*capsules)[0].y + torso.partOfBody.y * torso.partOfBody.w / length;
	(*capsules)[1].z = (*capsules)[0].z + torso.partOfBody.z * torso.partOfBody.w / length;
	(*capsules)[1].w = torso.radius;

	//left leg
	length = sqrt(leftLeg0.shift.x * leftLeg0.shift.x + leftLeg0.shift.y * leftLeg0.shift.y + leftLeg0.shift.z * leftLeg0.shift.z);
	(*capsules)[2].x = (*capsules)[0].x + leftLeg0.shift.x * leftLeg0.shift.w / length;
	(*capsules)[2].y = (*capsules)[0].y + leftLeg0.shift.y * leftLeg0.shift.w / length;
	(*capsules)[2].z = (*capsules)[0].z + leftLeg0.shift.z * leftLeg0.shift.w / length;
	(*capsules)[2].w = leftLeg0.radius;

	length = sqrt(leftLeg0.partOfBody.x * leftLeg0.partOfBody.x + leftLeg0.partOfBody.y * leftLeg0.partOfBody.y + leftLeg0.partOfBody.z * leftLeg0.partOfBody.z);
	(*capsules)[3].x = (*capsules)[2].x + leftLeg0.partOfBody.x * leftLeg0.partOfBody.w / length;
	(*capsules)[3].y = (*capsules)[2].y + leftLeg0.partOfBody.y * leftLeg0.partOfBody.w / length;
	(*capsules)[3].z = (*capsules)[2].z + leftLeg0.partOfBody.z * leftLeg0.partOfBody.w / length;
	(*capsules)[3].w = leftLeg0.radius;

	length = sqrt(leftLeg1.shift.x * leftLeg1.shift.x + leftLeg1.shift.y * leftLeg1.shift.y + leftLeg1.shift.z * leftLeg1.shift.z);
	(*capsules)[4].x = (*capsules)[3].x + leftLeg1.shift.x * leftLeg1.shift.w / length;
	(*capsules)[4].y = (*capsules)[3].y + leftLeg1.shift.y * leftLeg1.shift.w / length;
	(*capsules)[4].z = (*capsules)[3].z + leftLeg1.shift.z * leftLeg1.shift.w / length;
	(*capsules)[4].w = leftLeg1.radius;										
	
	length = sqrt(leftLeg1.partOfBody.x * leftLeg1.partOfBody.x + leftLeg1.partOfBody.y * leftLeg1.partOfBody.y + leftLeg1.partOfBody.z * leftLeg1.partOfBody.z);
	(*capsules)[5].x = (*capsules)[4].x + leftLeg1.partOfBody.x * leftLeg1.partOfBody.w / length;
	(*capsules)[5].y = (*capsules)[4].y + leftLeg1.partOfBody.y * leftLeg1.partOfBody.w / length;
	(*capsules)[5].z = (*capsules)[4].z + leftLeg1.partOfBody.z * leftLeg1.partOfBody.w / length;
	(*capsules)[5].w = leftLeg1.radius;

	length = sqrt(leftLeg2.shift.x * leftLeg2.shift.x + leftLeg2.shift.y * leftLeg2.shift.y + leftLeg2.shift.z * leftLeg2.shift.z);
	(*capsules)[6].x = (*capsules)[5].x + leftLeg2.shift.x * leftLeg2.shift.w / length;
	(*capsules)[6].y = (*capsules)[5].y + leftLeg2.shift.y * leftLeg2.shift.w / length;
	(*capsules)[6].z = (*capsules)[5].z + leftLeg2.shift.z * leftLeg2.shift.w / length;
	(*capsules)[6].w = leftLeg2.radius;

	length = sqrt(leftLeg2.partOfBody.x * leftLeg2.partOfBody.x + leftLeg2.partOfBody.y * leftLeg2.partOfBody.y + leftLeg2.partOfBody.z * leftLeg2.partOfBody.z);
	(*capsules)[7].x = (*capsules)[6].x + leftLeg2.partOfBody.x * leftLeg2.partOfBody.w / length;
	(*capsules)[7].y = (*capsules)[6].y + leftLeg2.partOfBody.y * leftLeg2.partOfBody.w / length;
	(*capsules)[7].z = (*capsules)[6].z + leftLeg2.partOfBody.z * leftLeg2.partOfBody.w / length;
	(*capsules)[7].w = leftLeg2.radius;

	//right leg
	length = sqrt(rightLeg0.shift.x * rightLeg0.shift.x + rightLeg0.shift.y * rightLeg0.shift.y + rightLeg0.shift.z * rightLeg0.shift.z);
	(*capsules)[8].x = (*capsules)[0].x + rightLeg0.shift.x * rightLeg0.shift.w / length;
	(*capsules)[8].y = (*capsules)[0].y + rightLeg0.shift.y * rightLeg0.shift.w / length;
	(*capsules)[8].z = (*capsules)[0].z + rightLeg0.shift.z * rightLeg0.shift.w / length;
	(*capsules)[8].w = rightLeg0.radius;

	length = sqrt(rightLeg0.partOfBody.x * rightLeg0.partOfBody.x + rightLeg0.partOfBody.y * rightLeg0.partOfBody.y + rightLeg0.partOfBody.z * rightLeg0.partOfBody.z);
	(*capsules)[9].x = (*capsules)[8].x + rightLeg0.partOfBody.x * rightLeg0.partOfBody.w / length;
	(*capsules)[9].y = (*capsules)[8].y + rightLeg0.partOfBody.y * rightLeg0.partOfBody.w / length;
	(*capsules)[9].z = (*capsules)[8].z + rightLeg0.partOfBody.z * rightLeg0.partOfBody.w / length;
	(*capsules)[9].w = rightLeg0.radius;

	length = sqrt(rightLeg1.shift.x * rightLeg1.shift.x + rightLeg1.shift.y * rightLeg1.shift.y + rightLeg1.shift.z * rightLeg1.shift.z);
	(*capsules)[10].x = (*capsules)[9].x + rightLeg1.shift.x * rightLeg1.shift.w / length;
	(*capsules)[10].y = (*capsules)[9].y + rightLeg1.shift.y * rightLeg1.shift.w / length;
	(*capsules)[10].z = (*capsules)[9].z + rightLeg1.shift.z * rightLeg1.shift.w / length;
	(*capsules)[10].w = rightLeg1.radius;

	length = sqrt(rightLeg1.partOfBody.x * rightLeg1.partOfBody.x + rightLeg1.partOfBody.y * rightLeg1.partOfBody.y + rightLeg1.partOfBody.z * rightLeg1.partOfBody.z);
	(*capsules)[11].x = (*capsules)[10].x + rightLeg1.partOfBody.x * rightLeg1.partOfBody.w / length;
	(*capsules)[11].y = (*capsules)[10].y + rightLeg1.partOfBody.y * rightLeg1.partOfBody.w / length;
	(*capsules)[11].z = (*capsules)[10].z + rightLeg1.partOfBody.z * rightLeg1.partOfBody.w / length;
	(*capsules)[11].w = rightLeg1.radius;

	length = sqrt(rightLeg2.shift.x * rightLeg2.shift.x + rightLeg2.shift.y * rightLeg2.shift.y + rightLeg2.shift.z * rightLeg2.shift.z);
	(*capsules)[12].x = (*capsules)[11].x + rightLeg2.shift.x * rightLeg2.shift.w / length;
	(*capsules)[12].y = (*capsules)[11].y + rightLeg2.shift.y * rightLeg2.shift.w / length;
	(*capsules)[12].z = (*capsules)[11].z + rightLeg2.shift.z * rightLeg2.shift.w / length;
	(*capsules)[12].w = rightLeg2.radius;

	length = sqrt(rightLeg2.partOfBody.x * rightLeg2.partOfBody.x + rightLeg2.partOfBody.y * rightLeg2.partOfBody.y + rightLeg2.partOfBody.z * rightLeg2.partOfBody.z);
	(*capsules)[13].x = (*capsules)[12].x + rightLeg2.partOfBody.x * rightLeg2.partOfBody.w / length;
	(*capsules)[13].y = (*capsules)[12].y + rightLeg2.partOfBody.y * rightLeg2.partOfBody.w / length;
	(*capsules)[13].z = (*capsules)[12].z + rightLeg2.partOfBody.z * rightLeg2.partOfBody.w / length;
	(*capsules)[13].w = rightLeg2.radius;

	//left Arm
	length = sqrt(leftArm0.shift.x * leftArm0.shift.x + leftArm0.shift.y * leftArm0.shift.y + leftArm0.shift.z * leftArm0.shift.z);
	(*capsules)[14].x = (*capsules)[1].x + leftArm0.shift.x * leftArm0.shift.w / length;
	(*capsules)[14].y = (*capsules)[1].y + leftArm0.shift.y * leftArm0.shift.w / length;
	(*capsules)[14].z = (*capsules)[1].z + leftArm0.shift.z * leftArm0.shift.w / length;
	(*capsules)[14].w = leftArm0.radius;

	length = sqrt(leftArm0.partOfBody.x * leftArm0.partOfBody.x + leftArm0.partOfBody.y * leftArm0.partOfBody.y + leftArm0.partOfBody.z * leftArm0.partOfBody.z);
	(*capsules)[15].x = (*capsules)[14].x + leftArm0.partOfBody.x * leftArm0.partOfBody.w / length;
	(*capsules)[15].y = (*capsules)[14].y + leftArm0.partOfBody.y * leftArm0.partOfBody.w / length;
	(*capsules)[15].z = (*capsules)[14].z + leftArm0.partOfBody.z * leftArm0.partOfBody.w / length;
	(*capsules)[15].w = leftArm0.radius;

	length = sqrt(leftArm1.shift.x * leftArm1.shift.x + leftArm1.shift.y * leftArm1.shift.y + leftArm1.shift.z * leftArm1.shift.z);
	(*capsules)[16].x = (*capsules)[15].x + leftArm1.shift.x * leftArm1.shift.w / length;
	(*capsules)[16].y = (*capsules)[15].y + leftArm1.shift.y * leftArm1.shift.w / length;
	(*capsules)[16].z = (*capsules)[15].z + leftArm1.shift.z * leftArm1.shift.w / length;
	(*capsules)[16].w = leftArm1.radius;

	length = sqrt(leftArm1.partOfBody.x * leftArm1.partOfBody.x + leftArm1.partOfBody.y * leftArm1.partOfBody.y + leftArm1.partOfBody.z * leftArm1.partOfBody.z);
	(*capsules)[17].x = (*capsules)[16].x + leftArm1.partOfBody.x * leftArm1.partOfBody.w / length;
	(*capsules)[17].y = (*capsules)[16].y + leftArm1.partOfBody.y * leftArm1.partOfBody.w / length;
	(*capsules)[17].z = (*capsules)[16].z + leftArm1.partOfBody.z * leftArm1.partOfBody.w / length;
	(*capsules)[17].w = leftArm1.radius;

	length = sqrt(leftArm2.shift.x * leftArm2.shift.x + leftArm2.shift.y * leftArm2.shift.y + leftArm2.shift.z * leftArm2.shift.z);
	(*capsules)[18].x = (*capsules)[17].x + leftArm2.shift.x * leftArm2.shift.w / length;
	(*capsules)[18].y = (*capsules)[17].y + leftArm2.shift.y * leftArm2.shift.w / length;
	(*capsules)[18].z = (*capsules)[17].z + leftArm2.shift.z * leftArm2.shift.w / length;
	(*capsules)[18].w = leftArm2.radius;

	length = sqrt(leftArm2.partOfBody.x * leftArm2.partOfBody.x + leftArm2.partOfBody.y * leftArm2.partOfBody.y + leftArm2.partOfBody.z * leftArm2.partOfBody.z);
	(*capsules)[19].x = (*capsules)[18].x + leftArm2.partOfBody.x * leftArm2.partOfBody.w / length;
	(*capsules)[19].y = (*capsules)[18].y + leftArm2.partOfBody.y * leftArm2.partOfBody.w / length;
	(*capsules)[19].z = (*capsules)[18].z + leftArm2.partOfBody.z * leftArm2.partOfBody.w / length;
	(*capsules)[19].w = leftArm2.radius;

	//right Arm
	length = sqrt(rightArm0.shift.x * rightArm0.shift.x + rightArm0.shift.y * rightArm0.shift.y + rightArm0.shift.z * rightArm0.shift.z);
	(*capsules)[20].x = (*capsules)[1].x + rightArm0.shift.x * rightArm0.shift.w / length;
	(*capsules)[20].y = (*capsules)[1].y + rightArm0.shift.y * rightArm0.shift.w / length;
	(*capsules)[20].z = (*capsules)[1].z + rightArm0.shift.z * rightArm0.shift.w / length;
	(*capsules)[20].w = rightArm0.radius;

	length = sqrt(rightArm0.partOfBody.x * rightArm0.partOfBody.x + rightArm0.partOfBody.y * rightArm0.partOfBody.y + rightArm0.partOfBody.z * rightArm0.partOfBody.z);
	(*capsules)[21].x = (*capsules)[20].x + rightArm0.partOfBody.x * rightArm0.partOfBody.w / length;
	(*capsules)[21].y = (*capsules)[20].y + rightArm0.partOfBody.y * rightArm0.partOfBody.w / length;
	(*capsules)[21].z = (*capsules)[20].z + rightArm0.partOfBody.z * rightArm0.partOfBody.w / length;
	(*capsules)[21].w = rightArm0.radius;

	length = sqrt(rightArm1.shift.x * rightArm1.shift.x + rightArm1.shift.y * rightArm1.shift.y + rightArm1.shift.z * rightArm1.shift.z);
	(*capsules)[22].x = (*capsules)[21].x + rightArm1.shift.x * rightArm1.shift.w / length;
	(*capsules)[22].y = (*capsules)[21].y + rightArm1.shift.y * rightArm1.shift.w / length;
	(*capsules)[22].z = (*capsules)[21].z + rightArm1.shift.z * rightArm1.shift.w / length;
	(*capsules)[22].w = rightArm1.radius;

	length = sqrt(rightArm1.partOfBody.x * rightArm1.partOfBody.x + rightArm1.partOfBody.y * rightArm1.partOfBody.y + rightArm1.partOfBody.z * rightArm1.partOfBody.z);
	(*capsules)[23].x = (*capsules)[22].x + rightArm1.partOfBody.x * rightArm1.partOfBody.w / length;
	(*capsules)[23].y = (*capsules)[22].y + rightArm1.partOfBody.y * rightArm1.partOfBody.w / length;
	(*capsules)[23].z = (*capsules)[22].z + rightArm1.partOfBody.z * rightArm1.partOfBody.w / length;
	(*capsules)[23].w = rightArm1.radius;

	length = sqrt(rightArm1.shift.x * rightArm1.shift.x + rightArm1.shift.y * rightArm1.shift.y + rightArm1.shift.z * rightArm1.shift.z);
	(*capsules)[24].x = (*capsules)[23].x + rightArm1.shift.x * rightArm1.shift.w / length;
	(*capsules)[24].y = (*capsules)[23].y + rightArm1.shift.y * rightArm1.shift.w / length;
	(*capsules)[24].z = (*capsules)[23].z + rightArm1.shift.z * rightArm1.shift.w / length;
	(*capsules)[24].w = rightArm1.radius;

	length = sqrt(rightArm1.partOfBody.x * rightArm1.partOfBody.x + rightArm1.partOfBody.y * rightArm1.partOfBody.y + rightArm1.partOfBody.z * rightArm1.partOfBody.z);
	(*capsules)[25].x = (*capsules)[24].x + rightArm1.partOfBody.x * rightArm1.partOfBody.w / length;
	(*capsules)[25].y = (*capsules)[24].y + rightArm1.partOfBody.y * rightArm1.partOfBody.w / length;
	(*capsules)[25].z = (*capsules)[24].z + rightArm1.partOfBody.z * rightArm1.partOfBody.w / length;
	(*capsules)[25].w = rightArm1.radius;

	length = sqrt(rightArm2.shift.x * rightArm2.shift.x + rightArm2.shift.y * rightArm2.shift.y + rightArm2.shift.z * rightArm2.shift.z);
	(*capsules)[24].x = (*capsules)[23].x + rightArm2.shift.x * rightArm2.shift.w / length;
	(*capsules)[24].y = (*capsules)[23].y + rightArm2.shift.y * rightArm2.shift.w / length;
	(*capsules)[24].z = (*capsules)[23].z + rightArm2.shift.z * rightArm2.shift.w / length;
	(*capsules)[24].w = rightArm2.radius;

	length = sqrt(rightArm2.partOfBody.x * rightArm2.partOfBody.x + rightArm2.partOfBody.y * rightArm2.partOfBody.y + rightArm2.partOfBody.z * rightArm2.partOfBody.z);
	(*capsules)[25].x = (*capsules)[24].x + rightArm2.partOfBody.x * rightArm2.partOfBody.w / length;
	(*capsules)[25].y = (*capsules)[24].y + rightArm2.partOfBody.y * rightArm2.partOfBody.w / length;
	(*capsules)[25].z = (*capsules)[24].z + rightArm2.partOfBody.z * rightArm2.partOfBody.w / length;
	(*capsules)[25].w = rightArm2.radius;

	//head
	length = sqrt(head.shift.x * head.shift.x + head.shift.y * head.shift.y + head.shift.z * head.shift.z);
	(*capsules)[26].x = (*capsules)[1].x + head.shift.x * head.shift.w / length;
	(*capsules)[26].y = (*capsules)[1].y + head.shift.y * head.shift.w / length;
	(*capsules)[26].z = (*capsules)[1].z + head.shift.z * head.shift.w / length;
	(*capsules)[26].w = head.radius;

	length = sqrt(head.partOfBody.x * head.partOfBody.x + head.partOfBody.y * head.partOfBody.y + head.partOfBody.z * head.partOfBody.z);
	(*capsules)[27].x = (*capsules)[26].x + head.partOfBody.x * head.partOfBody.w / length;
	(*capsules)[27].y = (*capsules)[26].y + head.partOfBody.y * head.partOfBody.w / length;
	(*capsules)[27].z = (*capsules)[26].z + head.partOfBody.z * head.partOfBody.w / length;
	(*capsules)[27].w = head.radius;
	return;
	}