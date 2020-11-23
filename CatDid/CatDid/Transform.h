#pragma once
#include <Windows.h>

#include "IFrameworkObject.h"
#include "Vec2D.h"
#include "Rect.h"

struct Transform
{
	Vec2DF Position = Vec2DF{ 0, 0 };
	Vec2DF Size = Vec2DF{ 0,0 };
	Vec2DF Scale = Vec2DF{ 1,1 };

	void Translate( const Vec2DF& vec );
};