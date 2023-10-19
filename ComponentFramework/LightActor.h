#pragma once

#include "Vector.h"
#include <vector>
using namespace MATH;

class LightActor {
private:
	Vec4 position;
	Vec4 color;
public:
	LightActor() = default;
	LightActor(Vec4 location_, Vec4 color_);
	~LightActor();

	Vec4 GetPosition() const { return position; }
	Vec4 GetColor() const { return color; }
};

typedef struct {
	std::vector<Vec4> position;
	std::vector<Vec4> color;
} LightData;