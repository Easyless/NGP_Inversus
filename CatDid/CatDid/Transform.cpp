#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "GameObject.h"
#include "Transform.h"
#include "InversusFramework.h"

constexpr int TranslationCount = 10;

bool Transform::GetCheckMap(const Tag& tag)
{
	if (this->checkMap.find(tag) == this->checkMap.end())
	{
		return false;
	}
	else 
	{
		return this->checkMap.find(tag)->second;
	}
}

void Transform::Translate(Vec2D<Ty> vec)
{
	for (size_t i = 0; i < 4; i++)
	{
		this->Position += vec/4;
		//if (this->framework->CheckCollision(this->obj)) { this->Position -= vec / 4; break; }
	}
}

void Transform::SetCheckMap(const Tag& tag, bool checkCollision)
{
	this->checkMap[tag] = checkCollision;
}

bool Transform::CheckCollision(Transform& other)
{
	auto thisRect = RectF(this->Position, this->Size.x, this->Size.y);
	auto otherRect = RectF(other.Position, other.Size.x, other.Size.y);
	return thisRect.isCollision(otherRect);
}

