#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "GameObject.h"
#include "InversusClasses.h"
#include "InversusFramework.h"

Bullet::Bullet(InversusFramework* framework)
	:GameObject(framework, "Bullet")
{
	this->transform.Size = Vec2DF{ 8,8 };
	this->transform.SetCheckMap("Block", true);
	this->transform.SetCheckMap("Enemy", true);
}

void Bullet::Update(float deltaTime)
{
	if (!this->framework->GetPauseSate())
	{
		if (isActive)
		{
			auto curMoveSpeed = (this->isSpecial) ? SpecialMoveSpeed : MoveSpeed;
			this->transform.Translate(MoveVector * curMoveSpeed * deltaTime);
			if (this->transform.Position.x > 3000 || this->transform.Position.x < -400 || this->transform.Position.y > 3000 || this->transform.Position.y < -400)
			{
				Deactive();
			}
		}
	}
}

void Bullet::Draw(PaintInfo info)
{
	if (isActive)
	{
		auto curRColor = (this->isSpecial) ? SpecialBulletRColor : BulletColor;
		auto curGColor = (this->isSpecial) ? SpecialBulletGColor : BulletColor;
		auto curBColor = (this->isSpecial) ? SpecialBulletBColor : BulletColor;
		auto curColor = (this->isSpecial) ? SpecialBulletColor : BulletColor;
		HPEN hPen = CreatePen(PS_SOLID, 7, curColor);
		auto oldPen = (HPEN)SelectObject(info.hdc, hPen);
		HBRUSH hBrush = CreateSolidBrush(curColor);
		auto oldBrush = (HBRUSH)SelectObject(info.hdc, hBrush);
		auto rt = RectF(this->transform.Position * info.AntiAliasing, this->transform.Size.x * info.AntiAliasing, this->transform.Size.y * info.AntiAliasing);
		if (MoveVector == Vec2DF::Up())
		{
			rt.bottom = (StartPosition.y * info.AntiAliasing - rt.top < GradientMaxLength * info.AntiAliasing) ? StartPosition.y * info.AntiAliasing : rt.bottom = rt.top + GradientMaxLength * info.AntiAliasing;
		}
		else if (MoveVector == Vec2DF::Left())
		{
			rt.right = (StartPosition.x * info.AntiAliasing - rt.left< GradientMaxLength * info.AntiAliasing) ? StartPosition.x * info.AntiAliasing : rt.right = rt.left + GradientMaxLength * info.AntiAliasing;
		}
		else if (MoveVector == Vec2DF::Down())
		{
			rt.top = (rt.bottom - StartPosition.y * info.AntiAliasing < GradientMaxLength * info.AntiAliasing) ? StartPosition.y * info.AntiAliasing : rt.top = rt.bottom - GradientMaxLength * info.AntiAliasing;
		}
		else if (MoveVector == Vec2DF::Right())
		{
			rt.left = (rt.right - StartPosition.x * info.AntiAliasing < GradientMaxLength * info.AntiAliasing) ? StartPosition.x * info.AntiAliasing : rt.left = rt.right - GradientMaxLength * info.AntiAliasing;
		}
		rt += info.margin;
		TRIVERTEX vert[2];
		GRADIENT_RECT grrect;
		// 그라데이션의 시작좌표를 명시한다.
		vert[0].x = rt.left;
		vert[0].y = rt.top;

		// 그라데이션의 시작색상을 명시한다.
		vert[0].Red = ((MoveVector == Vec2DF::Up() || MoveVector == Vec2DF::Left()) ? curRColor : 0xFFFF);
		vert[0].Green = ((MoveVector == Vec2DF::Up() || MoveVector == Vec2DF::Left()) ? curGColor : 0xFFFF);
		vert[0].Blue = ((MoveVector == Vec2DF::Up() || MoveVector == Vec2DF::Left()) ? curBColor : 0xFFFF);
		vert[0].Alpha = 0xFFFF;

		// 그라데이션의 끝좌표를 명시한다.
		vert[1].x = rt.right;
		vert[1].y = rt.bottom;

		// 그라데이션의 끝색상를 명시한다.
		vert[1].Red = ((MoveVector == Vec2DF::Up() || MoveVector == Vec2DF::Left()) ? 0xFFFF : curRColor);
		vert[1].Green = ((MoveVector == Vec2DF::Up() || MoveVector == Vec2DF::Left()) ? 0xFFFF : curGColor);
		vert[1].Blue = ((MoveVector == Vec2DF::Up() || MoveVector == Vec2DF::Left()) ? 0xFFFF : curBColor);
		vert[1].Alpha = 0xFFFF;

		
		grrect.UpperLeft = 0;
		grrect.LowerRight = 1;

		GradientFill(info.hdc, vert, 2, &grrect, 1, ((MoveVector == Vec2DF::Right() || MoveVector == Vec2DF::Left()) ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V)
		);
		SelectObject(info.hdc, oldPen);
		SelectObject(info.hdc, oldBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);

	}
}
void Bullet::Reset()
{
	this->Deactive();
}
void Bullet::TurnActive(Vec2DF position, Vec2DF MoveVector, bool isSpecial)
{
	if (!isActive)
	{
		this->MoveVector = MoveVector;
		this->transform.Position = position;
		this->StartPosition = position;
		this->isSpecial = isSpecial;
		isActive = true;
	}
}
bool Bullet::isCollision(GameObject& other)
{
	return false;
}