#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "GameObject.h"
#include "InversusClasses.h"
#include "InversusFramework.h"
#include "DropBullet.h"

DropBullet::DropBullet(InversusFramework* framework)
	:GameObject(framework,"DropBullet")
{
	this->transform.SetCheckMap("Player", true);
}

void DropBullet::Active()
{
	GameObject::Active();
}

void DropBullet::Deactive()
{
	GameObject::Deactive();
}

void DropBullet::Update(float deltaTime)
{
	if (!this->framework->GetPauseSate())
	{
		if (this->GetActiveState())
		{
			curRotate += this->rotateSpeed * deltaTime;
			if (curRotate > 360)
			{
				curRotate -= 360;
			}
			//this->framework->CheckCollision(*this);
		}
	}
}

void DropBullet::Draw(PaintInfo info)
{
	if (this->GetActiveState())
	{
		for (int i = 0; i < this->SpecialBulletCount; i++)
		{
			COLORREF color = RGB(56, 182, 214);
			HPEN hBulletPen = CreatePen(PS_SOLID, 1, color);
			auto oldBulletPen = (HPEN)SelectObject(info.hdc, hBulletPen);
			HBRUSH hBulletBrush = CreateSolidBrush(color);
			auto oldBulletBrush = (HBRUSH)SelectObject(info.hdc, hBulletBrush);
			auto bulletPos = (this->transform.Position + Vec2DF{ 0,10 }).RotatedPoint(this->transform.Position, i *( 360/SpecialBulletCount) + this->curRotate);
			auto bulletRt = RectF(bulletPos * info.AntiAliasing, this->transform.Size.x * 0.2 * info.AntiAliasing, this->transform.Size.y * 0.2 * info.AntiAliasing) + info.margin;
			Ellipse(info.hdc, bulletRt.left, bulletRt.top, bulletRt.right, bulletRt.bottom);
			SelectObject(info.hdc, oldBulletBrush);
			SelectObject(info.hdc, oldBulletPen);
			DeleteObject(hBulletBrush);
			DeleteObject(hBulletPen);
		}
	}
}

bool DropBullet::isCollision(GameObject& other)
{
	if (this->GetActiveState())
	{
		if (other.tag == "Player")
		{
			static_cast<Player&>(other).AddSpecialBullet(this->SpecialBulletCount);
			this->Deactive();
		}
	}
	return false;
}

void DropBullet::Reset()
{
	this->Deactive();
}

void DropBullet::ActiveDrop(Vec2DF position, int count)
{
	auto idx = this->framework->container->BlockMap.GetPositionIndex(position);
	this->transform.Position = this->framework->container->BlockMap.GetBlock(idx.first, idx.second).transform.Position;
	this->transform.Size = this->framework->container->BlockMap.GetBlock(idx.first, idx.second).transform.Size;
	this->curRotate = 0;
	SpecialBulletCount = count;
	this->Active();
}
