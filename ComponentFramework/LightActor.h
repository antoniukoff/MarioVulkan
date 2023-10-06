#pragma once

#include "Vector.h"

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