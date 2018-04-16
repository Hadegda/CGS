#ifndef _PERSON_PATTERN_H_
#define _PERSON_PATTERN_H_

#include <vector>
#include <DirectXMath.h>

struct PartOfBody {
	DirectX::XMFLOAT4 shift; //shift from root body part (shift direction {x, y, z}, shift magnitude {w})
	DirectX::XMFLOAT4 partOfBody; //direction {x, y, z},  magnitude {w}
	float radius;
	DirectX::XMFLOAT3 color;
};

class PersonPattern {
public:
	PersonPattern();
	~PersonPattern() { ; };

	void Scale(float scale);
	void DropForward();
	void DropBack();
	void DropLeft();
	void DropRight();
	void InvertY();
	void Centered(DirectX::XMFLOAT3 center);

	int GetCapsCount() { return capsCount; };
	void GetParameters(std::vector<float>* params);

private:
	int capsCount;
	int parametersCount;

	DirectX::XMFLOAT3 shift;
	PartOfBody torso;
	PartOfBody leftLeg0;
	PartOfBody leftLeg1;
	PartOfBody leftLeg2;
	PartOfBody rightLeg0;
	PartOfBody rightLeg1;
	PartOfBody rightLeg2;
	PartOfBody leftArm0;
	PartOfBody leftArm1;
	PartOfBody leftArm2;
	PartOfBody rightArm0;
	PartOfBody rightArm1;
	PartOfBody rightArm2;
	PartOfBody head;

	float** parameters;
};

#endif