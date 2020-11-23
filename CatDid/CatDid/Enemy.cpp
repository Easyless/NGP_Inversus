#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "GameObject.h"
#include "InversusClasses.h"
#include "InversusFramework.h"
//#define DIVDRAW

Enemy::Enemy(InversusFramework* framework)
	:GameObject(framework,"Enemy")
{
	this->transform.Size = Vec2DF{ 25,25 };
	this->transform.SetCheckMap("Bullet", true);
	this->transform.SetCheckMap("Player", true);
	this->transform.SetCheckMap("Explosion", true);
}

void Enemy::Update(float deltaTime)
{
	if(!this->framework->GetPauseSate())
	{ 
		if (state == GenState::Gen)
		{
			CharginCount -= 30 * deltaTime;
			if (CharginCount < 0)
			{
				this->state = GenState::On;
			}
		}
		else if (state == GenState::On)
		{
			//this->transform.Translate(
			//	(this->framework->container->player.transform.Position - this->transform.Position).Nomal() * deltaTime * moveSpeed
			//);
			auto idx = this->framework->container->BlockMap.GetPositionIndex(this->transform.Position);
			this->framework->container->BlockMap.GetBlock(idx.first, idx.second).Active();

		}
		else if (state == GenState::Dead)
		{
			this->CharginCount -= deltaTime;
			if (CharginCount < 0)
			{
				//this->framework->controller->PostAddScore();
				this->Deactive();
				this->framework->container->ActiveExplosion(this->transform.Position, this->color);
			}
		}
	}
}
void Enemy::Draw(PaintInfo info)
{
	if (this->state == GenState::Gen)
	{
		HPEN hPen = CreatePen(PS_SOLID, 5, color);
		auto oldPen = (HPEN)SelectObject(info.hdc, hPen);
		auto oldBrush = (HBRUSH)SelectObject(info.hdc, GetStockObject(NULL_BRUSH));
		auto rt = RectF(this->transform.Position * info.AntiAliasing, (1 + CharginCount) * this->transform.Size.x * info.AntiAliasing, (1 + CharginCount) * this->transform.Size.y * info.AntiAliasing) + info.margin;
		SetBkMode(info.hdc, TRANSPARENT);
		RoundRect(info.hdc, rt.left, rt.top, rt.right, rt.bottom, 20, 20);
		SetBkMode(info.hdc, OPAQUE);
		SelectObject(info.hdc, oldPen);
		DeleteObject(hPen);
		SelectObject(info.hdc, oldBrush);
	}
	else if (state == GenState::On)
	{
#ifdef DIVDRAW


		{
			auto idx = this->framework->container->BlockMap.GetPositionIndex(this->transform.Position);
			int centerX = idx.first;
			int centerY = idx.second;

			int idxArr[9][2] =
			{
				{centerX - 1,centerY - 1},{centerX,centerY - 1},{centerX + 1,centerY - 1},
				{centerX - 1,centerY},{centerX,centerY},{centerX + 1,centerY},
				{centerX - 1,centerY + 1},{centerX,centerY + 1},{centerX + 1,centerY + 1}
			};
			for (size_t i = 0; i < 9; i++)
			{
				if (idxArr[i][0] >= 0 && idxArr[i][0] < this->framework->container->BlockMap.GetSize().first
					&& idxArr[i][1] >= 0 && idxArr[i][1] < this->framework->container->BlockMap.GetSize().second)
				{

					auto& targetObj = this->framework->container->BlockMap.GetBlock(idxArr[i][0], idxArr[i][1]);
					if(targetObj.DrawHatched == false)
					{
						HPEN hPen = CreatePen(PS_SOLID, 3, color);
						auto oldPen = (HPEN)SelectObject(info.hdc, hPen);
						HBRUSH hBrush = CreateHatchBrush(HS_FDIAGONAL, color);
						auto oldBrush = (HBRUSH)SelectObject(info.hdc, hBrush);
						SetBkMode(info.hdc, TRANSPARENT);
						auto blockRt = RectF(targetObj.transform.Position * info.AntiAliasing, targetObj.transform.Size.x * info.AntiAliasing, targetObj.transform.Size.y * info.AntiAliasing) + info.margin;
						//Rectangle(info.hdc, blockRt.left, blockRt.top, blockRt.right, blockRt.bottom);
						SetBkMode(info.hdc, OPAQUE);
						SelectObject(info.hdc, oldPen);
						SelectObject(info.hdc, oldBrush);
						DeleteObject(hPen);
						DeleteObject(hBrush);
						targetObj.DrawHatched = true;
					}
				}
			}
		}
#else
		{
			auto idx = this->framework->container->BlockMap.GetPositionIndex(this->transform.Position);
			int centerX = idx.first;
			int centerY = idx.second;
			auto& targetObj = this->framework->container->BlockMap.GetBlock(centerX, centerY);
			HPEN hPen = CreatePen(PS_SOLID, 3, color);
			auto oldPen = (HPEN)SelectObject(info.hdc, hPen);
			HBRUSH hBrush = CreateHatchBrush(HS_FDIAGONAL, color);
			auto oldBrush = (HBRUSH)SelectObject(info.hdc, hBrush);
			SetBkMode(info.hdc, TRANSPARENT);
			auto blockRt = RectF(targetObj.transform.Position * info.AntiAliasing, targetObj.transform.Size.x * 3 * info.AntiAliasing, targetObj.transform.Size.y * 3 * info.AntiAliasing) + info.margin;
			Rectangle(info.hdc, blockRt.left, blockRt.top, blockRt.right, blockRt.bottom);
			SetBkMode(info.hdc, OPAQUE);
			SelectObject(info.hdc, oldPen);
			SelectObject(info.hdc, oldBrush);
			DeleteObject(hPen);
			DeleteObject(hBrush);
			targetObj.DrawHatched = true;
		}
#endif // DIVDRAW

		{
			HPEN hPen = CreatePen(PS_SOLID, 7, color);
			auto oldPen = (HPEN)SelectObject(info.hdc, hPen);
			HBRUSH hBrush = CreateSolidBrush(color);
			auto oldBrush = (HBRUSH)SelectObject(info.hdc, hBrush);
			auto rt = RectF(this->transform.Position * info.AntiAliasing, this->transform.Size.x * info.AntiAliasing, this->transform.Size.y * info.AntiAliasing) + info.margin;
			RoundRect(info.hdc, rt.left, rt.top, rt.right, rt.bottom, 20, 20);
			SelectObject(info.hdc, oldPen);
			SelectObject(info.hdc, oldBrush);
			DeleteObject(hPen);
			DeleteObject(hBrush);
		}
	}
}
bool Enemy::isCollision(GameObject& other)
{
	if(this->state == GenState::On)
	{
		if (other.tag == "Explosion")
		{
			this->state = GenState::Dead;
			this->CharginCount = 0.15f;
		}
		else if( other.tag == "Bullet")
		{
			//this->framework->controller->PostAddScore(this->isSpecial);
			this->Deactive();
			other.Deactive();
			this->framework->container->ActiveExplosion(this->transform.Position, this->color);
		}
		if (other.tag == "Player")
		{
			other.Deactive();
		}
	}
	return false;
}
bool Enemy::GetActiveState() const
{
	return this->state == GenState::On && this->isActive;
}
void Enemy::Reset()
{
	GameObject::Deactive();
	this->state = GenState::Off;
}
void Enemy::Active() 
{
	GameObject::Active();
}
void Enemy::Deactive() 
{
	if (rand() % 3 == 0)
	{
		this->framework->container->DropBullet(this->transform.Position, 1 + rand() % 3);
	}
	GameObject::Deactive();
	this->state = GenState::Off;
}
void Enemy::TurnActive(Vec2DF Position,bool isSpecial)
{
	this->isSpecial = isSpecial;
	this->Active();
	this->transform.Position = Position;
	state = GenState::Gen;
	CharginCount = 10.0f;
	if (!isSpecial)
	{
		this->color = RGB(56, 182, 214);
		this->moveSpeed = 30.f;
	}
	else 
	{
		this->color = RGB(214, 56, 56);
		this->moveSpeed = 80.f;
	}
}