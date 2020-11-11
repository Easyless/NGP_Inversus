#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "GameObject.h"
#include "InversusClasses.h"
#include "InversusFramework.h"

Explosion::Explosion(InversusFramework* framework)
	:GameObject(framework, "Explosion")
{
	this->transform.SetCheckMap("Enemy", true);
	this->transform.SetCheckMap("Block", true);

}


void Explosion::Update(float deltaTime)
{
	if (!this->framework->GetPauseSate())
	{
		if (this->state == GenState::On)
		{
			if (effecting == false)
			{

				effecting = true;
			}
			time += deltaTime;
			if (time > endTime + 1)
			{
				this->state == GenState::Off;
			}
		}
	}
}

void Explosion::Draw(PaintInfo info)
{
	if (this->state == GenState::On)
	{

		for (auto& sub : subEffect)
		{
			sub.Draw(info, time, mainPosition,color);
		}
	}
}

bool Explosion::isCollision(GameObject& other)
{
	return false;
}

void Explosion::Active()
{
	GameObject::Active();
}

void Explosion::Deactive()
{
	GameObject::Deactive();
}

void Explosion::TurnActive(Vec2DF position, COLORREF color, bool CheckCollision)
{
	this->Active();
	effecting = false;
	this->framework->controller->PostExplosion();

	this->state = GenState::On;
	this->color = color;
	this->time = 0;
	this->checkCollision = CheckCollision;
	subEffect.clear();
	size_t particleSize = 8 + rand() % 8;
	subEffect.resize(particleSize);
	mainPosition = position;
	for (auto& sub : subEffect)
	{
		sub.position = Vec2DF{
			((rand() % 2 == 0) ? -1 : 1) * 15.0f * (rand() % 4),
			((rand() % 2 == 0) ? -1 : 1) * 15.0f * (rand() % 4) 
		};
		sub.startTime = 0.05f * (rand() % 6);
		sub.maxScale = 60.0 + ((rand() % 20) - 10);
		endTime = (endTime < sub.startTime + sub.duration) ? sub.startTime + sub.duration : endTime;
	}
	//물리충돌 파트
	if (this->checkCollision)
	{
		auto index = this->framework->container->BlockMap.GetPositionIndex(position);
		auto& centerBlock = this->framework->container->BlockMap.GetBlock(index.first, index.second);
		this->transform.Position = centerBlock.transform.Position;
		this->transform.Size = centerBlock.transform.Size * 3;
	}
	else 
	{
		this->transform.Size = Vec2DF{ 0,0 };
	}

	this->framework->CheckCollision(*this);


}

void _ExplosionEffect::Draw(PaintInfo info,float thisTime, Vec2DF mainPosition , COLORREF color)
{
	if (startTime <= thisTime && thisTime <= startTime + duration)
	{
		HPEN hPen = CreatePen(PS_SOLID, 5, color);
		auto oldPen = (HPEN)SelectObject(info.hdc, hPen);
		HBRUSH hBrush = CreateSolidBrush(color);
		auto oldBrush = (HBRUSH)SelectObject(info.hdc, hBrush);
		float scale = -1 * (thisTime-startTime) * (thisTime - startTime - duration) / pow(duration,2) * 4 * maxScale;
		auto rt = RectF((mainPosition + this->position) * info.AntiAliasing, scale * info.AntiAliasing, scale * info.AntiAliasing) + info.margin;
		Ellipse(info.hdc, rt.left, rt.top, rt.right, rt.bottom);
		SelectObject(info.hdc, oldPen);
		DeleteObject(hPen);
		SelectObject(info.hdc, oldBrush);
		DeleteObject(hBrush);
	}
}