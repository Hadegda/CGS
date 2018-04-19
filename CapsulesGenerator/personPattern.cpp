#include "personPattern.h"

using namespace DirectX;

PersonPattern::PersonPattern()
{
	capsCount = 14;
	parametersCount = 125;

	shift = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	torso = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, 1.0f, 0.24f }, 0.16f };
	leftLeg0 = { XMFLOAT4{ 0.0f, 1.0f, 0.0f, 0.1f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.22f }, 0.07f };
	leftLeg1 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.22f }, 0.06f };
	leftLeg2 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 1.0f, 0.0f, 0.0f, 0.05f }, 0.06f };
	rightLeg0 = { XMFLOAT4{ 0.0f, -1.0f, 0.0f, 0.1f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.22f }, 0.07f };
	rightLeg1 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.22f }, 0.06f };
	rightLeg2 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 1.0f, 0.0f, 0.0f, 0.05f }, 0.06f };
	leftArm0 = { XMFLOAT4{ 0.0f, 1.0f, 0.4f, 0.14f }, XMFLOAT4{ 0.0f, 1.0f, 0.0f, 0.15f }, 0.05f };
	leftArm1 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.2f },0.05f };
	leftArm2 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.2f }, 0.05f };
	rightArm0 = { XMFLOAT4{ 0.0f, -1.0f, 0.4f, 0.14f }, XMFLOAT4{ 0.0f, -1.0f, 0.0f, 0.15f }, 0.05f };
	rightArm1 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.2f }, 0.05f };
	rightArm2 = { XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, -1.0f, 0.2f }, 0.05f };
	head = { XMFLOAT4{ 0.0f, 0.0f, 1.0f, 0.16f }, XMFLOAT4{ 0.0f, 0.0f, 1.0f, 0.02f }, 0.08f };

	parameters = new float*[parametersCount];
	for(int i = 0; i < parametersCount; i++)
		parameters[i] = new float;
	*(parameters[0]) = shift.x; *(parameters[1]) = shift.y; *(parameters[2]) = shift.z;
	*(parameters[3]) = torso.partOfBody.x; *(parameters[4]) = torso.partOfBody.y; *(parameters[5]) = torso.partOfBody.z; *(parameters[6]) = torso.partOfBody.w;
	*(parameters[7]) = torso.radius;

	*(parameters[8]) = leftLeg0.shift.x; *(parameters[9]) = leftLeg0.shift.y; *(parameters[10]) = leftLeg0.shift.z; *(parameters[11]) = leftLeg0.shift.w;
	*(parameters[12]) = leftLeg0.partOfBody.x; *(parameters[13]) = leftLeg0.partOfBody.y; *(parameters[14]) = leftLeg0.partOfBody.z; *(parameters[15]) = leftLeg0.partOfBody.w;
	*(parameters[16]) = leftLeg0.radius;

	*(parameters[17]) = leftLeg1.shift.x; *(parameters[18]) = leftLeg1.shift.y; *(parameters[19]) = leftLeg1.shift.z; *(parameters[20]) = leftLeg1.shift.w;
	*(parameters[21]) = leftLeg1.partOfBody.x; *(parameters[22]) = leftLeg1.partOfBody.y; *(parameters[23]) = leftLeg1.partOfBody.z; *(parameters[24]) = leftLeg1.partOfBody.w;
	*(parameters[25]) = leftLeg1.radius;

	*(parameters[26]) = leftLeg2.shift.x; *(parameters[27]) = leftLeg2.shift.y; *(parameters[28]) = leftLeg2.shift.z; *(parameters[29]) = leftLeg2.shift.w;
	*(parameters[30]) = leftLeg2.partOfBody.x; *(parameters[31]) = leftLeg2.partOfBody.y; *(parameters[32]) = leftLeg2.partOfBody.z; *(parameters[33]) = leftLeg2.partOfBody.w;
	*(parameters[34]) = leftLeg2.radius;

	*(parameters[35]) = rightLeg0.shift.x; *(parameters[36]) = rightLeg0.shift.y; *(parameters[37]) = rightLeg0.shift.z; *(parameters[38]) = rightLeg0.shift.w;
	*(parameters[39]) = rightLeg0.partOfBody.x; *(parameters[40]) = rightLeg0.partOfBody.y; *(parameters[41]) = rightLeg0.partOfBody.z; *(parameters[42]) = rightLeg0.partOfBody.w;
	*(parameters[43]) = rightLeg0.radius;

	*(parameters[44]) = rightLeg1.shift.x; *(parameters[45]) = rightLeg1.shift.y; *(parameters[46]) = rightLeg1.shift.z; *(parameters[47]) = rightLeg1.shift.w;
	*(parameters[48]) = rightLeg1.partOfBody.x; *(parameters[49]) = rightLeg1.partOfBody.y; *(parameters[50]) = rightLeg1.partOfBody.z; *(parameters[51]) = rightLeg1.partOfBody.w;
	*(parameters[52]) = rightLeg1.radius;

	*(parameters[53]) = rightLeg2.shift.x; *(parameters[54]) = rightLeg2.shift.y; *(parameters[55]) = rightLeg2.shift.z; *(parameters[56]) = rightLeg2.shift.w;
	*(parameters[57]) = rightLeg2.partOfBody.x; *(parameters[58]) = rightLeg2.partOfBody.y; *(parameters[59]) = rightLeg2.partOfBody.z; *(parameters[60]) = rightLeg2.partOfBody.w;
	*(parameters[61]) = rightLeg2.radius;

	*(parameters[62]) = leftArm0.shift.x; *(parameters[63]) = leftArm0.shift.y; *(parameters[64]) = leftArm0.shift.z; *(parameters[65]) = leftArm0.shift.w;
	*(parameters[66]) = leftArm0.partOfBody.x; *(parameters[67]) = leftArm0.partOfBody.y; *(parameters[68]) = leftArm0.partOfBody.z; *(parameters[69]) = leftArm0.partOfBody.w;
	*(parameters[70]) = leftArm0.radius;

	*(parameters[71]) = leftArm1.shift.x; *(parameters[72]) = leftArm1.shift.y; *(parameters[73]) = leftArm1.shift.z; *(parameters[74]) = leftArm1.shift.w;
	*(parameters[75]) = leftArm1.partOfBody.x; *(parameters[76]) = leftArm1.partOfBody.y; *(parameters[77]) = leftArm1.partOfBody.z; *(parameters[78]) = leftArm1.partOfBody.w;
	*(parameters[79]) = leftArm1.radius;

	*(parameters[80]) = leftArm2.shift.x; *(parameters[81]) = leftArm2.shift.y; *(parameters[82]) = leftArm2.shift.z; *(parameters[83]) = leftArm2.shift.w;
	*(parameters[84]) = leftArm2.partOfBody.x; *(parameters[85]) = leftArm2.partOfBody.y; *(parameters[86]) = leftArm2.partOfBody.z; *(parameters[87]) = leftArm2.partOfBody.w;
	*(parameters[88]) = leftArm2.radius;

	*(parameters[89]) = rightArm0.shift.x; *(parameters[90]) = rightArm0.shift.y; *(parameters[91]) = rightArm0.shift.z; *(parameters[92]) = rightArm0.shift.w;
	*(parameters[93]) = rightArm0.partOfBody.x; *(parameters[94]) = rightArm0.partOfBody.y; *(parameters[95]) = rightArm0.partOfBody.z; *(parameters[96]) = rightArm0.partOfBody.w;
	*(parameters[97]) = rightArm0.radius;

	*(parameters[98]) = rightArm1.shift.x; *(parameters[99]) = rightArm1.shift.y; *(parameters[100]) = rightArm1.shift.z; *(parameters[101]) = rightArm1.shift.w;
	*(parameters[102]) = rightArm1.partOfBody.x; *(parameters[103]) = rightArm1.partOfBody.y; *(parameters[104]) = rightArm1.partOfBody.z; *(parameters[105]) = rightArm1.partOfBody.w;
	*(parameters[106]) = rightArm1.radius;

	*(parameters[107]) = rightArm2.shift.x; *(parameters[108]) = rightArm2.shift.y; *(parameters[109]) = rightArm2.shift.z; *(parameters[110]) = rightArm2.shift.w;
	*(parameters[111]) = rightArm2.partOfBody.x; *(parameters[112]) = rightArm2.partOfBody.y; *(parameters[113]) = rightArm2.partOfBody.z; *(parameters[114]) = rightArm2.partOfBody.w;
	*(parameters[115]) = rightArm2.radius;

	*(parameters[116]) = head.shift.x; *(parameters[117]) = head.shift.y; *(parameters[118]) = head.shift.z; *(parameters[119]) = head.shift.w;
	*(parameters[120]) = head.partOfBody.x; *(parameters[121]) = head.partOfBody.y; *(parameters[122]) = head.partOfBody.z; *(parameters[123]) = head.partOfBody.w;
	*(parameters[124]) = head.radius;
}

void PersonPattern::Scale(float scale) {
	int i;
	for (i = 6; i < parametersCount; i += 9)
		*(parameters[i]) *= scale;
	for (i = 7; i < parametersCount; i += 9)
		*(parameters[i]) *= scale;

	for (i = 11; i < parametersCount; i += 9)
		*(parameters[i]) *= scale;
}

void PersonPattern::DropForward() {
	float tmp;
	int i;
	for (i = 5; i < parametersCount; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 2]);
		*(parameters[i - 2]) = tmp;
	}
	for (i = 10; i < parametersCount; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 2]);
		*(parameters[i - 2]) = tmp;
	}
}

void PersonPattern::DropBack() {
	float tmp;
	int i;
	for (i = 5; i < parametersCount; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 2]);
		*(parameters[i - 2]) = -tmp;
	}
	for (i = 10; i < parametersCount; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 2]);
		*(parameters[i - 2]) = -tmp;
	}
}

void PersonPattern::DropLeft() {
	float tmp;
	int i;
	for (i = 5; i < parametersCount; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 1]);
		*(parameters[i - 1]) = -tmp;
	}
	for (i = 10; i < parametersCount; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 1]);
		*(parameters[i - 1]) = -tmp;
	}
}

void PersonPattern::DropRight() {
	float tmp;
	int i;
	for (i = 5; i < parametersCount; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 1]);
		*(parameters[i - 1]) = tmp;
	}
	for (i = 10; i < parametersCount; i += 9) {
		tmp = *(parameters[i]);
		*(parameters[i]) = *(parameters[i - 1]);
		*(parameters[i - 1]) = tmp;
	}
}

void PersonPattern::InvertY() {
	int i;
	for (i = 5; i < parametersCount; i += 9) {
		*(parameters[i]) = -*(parameters[i]);
	}
	for (i = 10; i < parametersCount; i += 9) {
		*(parameters[i]) = -*(parameters[i]);
	}
}

void PersonPattern::Centered(XMFLOAT3 center)
{
	*(parameters[0]) = center.x;
	*(parameters[1]) = center.y;
	*(parameters[2]) = center.z;
}

void PersonPattern::GetParameters(std::vector<float>* params) {
	params->clear();
	params->resize(parametersCount);
	for (int i = 0; i < params->size(); i++)
		(*params)[i] = *parameters[i];
}