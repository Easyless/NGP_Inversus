#include "stdafx.h"
#include "UtillFunction.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "InversusFramework.h"
#include "GameObject.h"
#include "Player.h"
#include "InversusClasses.h"


Player::Player(InversusFramework* framework)
	:GameObject(framework, "Player")
{
	this->transform.Size = Vec2DF{ 25,25 };
	this->transform.SetCheckMap("Block", true);
	this->transform.SetCheckMap("Explosion", true);
	this->transform.SetCheckMap("DropBullet", true);
	this->transform.SetCheckMap("Enemy", true);
}

void Player::RefreshFromData( const GameSceneData& data, UINT index )
{
	if ( !this->isActive  && !data.playerState[index].isDead )
	{
		this->Active();
	}
	this->state = !data.playerState[index].isDead ? GenState::On : GenState::Off;
	this->transform.Position.x = data.playerState[index].positionX;
	this->transform.Position.y = data.playerState[index].positionY;
	this->playerIndex = index;
}

void Player::Update(float deltaTime)
{
	//if (this->framework->GetPauseSate())
	//{
	//}
	//else
	//{
	//	if (this->state == GenState::Gen)
	//	{
	//		CharginCount -= 30 * deltaTime;
	//		if (CharginCount < 0)
	//		{
	//			this->state = GenState::On;
	//		}
	//	}
	//	else if (this->state == GenState::On)
	//	{
	//		if (SpecialDelay > 0)
	//		{
	//			SpecialDelay -= deltaTime;
	//			if (SpecialDelay < 0)
	//			{
	//				bool isSpecialBullet = true;
	//				if (SpecialCharging)
	//				{
	//					auto posLeft = this->transform.Position + Vec2DF{ curCharging.y,curCharging.x } *this->transform.Size.x * 1.5f;
	//					auto posRight = this->transform.Position + Vec2DF{ curCharging.y,curCharging.x } *-1 * this->transform.Size.x * 1.5f;
	//					this->framework->container->AddBullet(posLeft, curCharging, isSpecialBullet);
	//					this->framework->container->AddBullet(this->transform.Position, curCharging, isSpecialBullet);
	//					this->framework->container->AddBullet(posRight, curCharging, isSpecialBullet);
	//					SpecialCharging = isSpecialBullet;
	//				}
	//				else
	//				{
	//					this->framework->container->AddBullet(this->transform.Position, curCharging, isSpecialBullet);
	//				}
	//				SpecialCharging = false;
	//			}
	//		}
	//		regenTime += deltaTime;
	//		if (regenTime > bulletRegenDelay)
	//		{
	//			regenTime = 0;
	//			if (BulletCount < 6)
	//			{
	//				BulletCount++;
	//			}
	//		}
	//		float rotate = 0;
	//		if (curCharging == Vec2DF::Up())
	//		{
	//			rotate = 270;
	//		}
	//		else if (curCharging == Vec2DF::Left())
	//		{
	//			rotate = 180;
	//		}
	//		else if (curCharging == Vec2DF::Down())
	//		{
	//			rotate = 90;
	//		}
	//		else if (curCharging == Vec2DF::Right())
	//		{
	//			rotate = 0;
	//		}

	//		if (CharginCount < maxCharge || !Utill::approximation(bulletRotate, rotate))
	//		{
	//			this->bulletRotate += rotateSpeed * deltaTime;
	//		}
	//		else if (Utill::approximation(bulletRotate, rotate))
	//		{
	//			this->bulletRotate = rotate;
	//		}
	//		if (this->bulletRotate > 360) { this->bulletRotate -= 360; }
	//		if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
	//		{
	//			this->framework->controller->PostGameOver();
	//		}
	//		if (GetAsyncKeyState('W'))
	//		{
	//			this->transform.Translate(Vec2DF::Up() * speedNumber * deltaTime);
	//		}
	//		if (GetAsyncKeyState('A'))
	//		{
	//			this->transform.Translate(Vec2DF::Left() * speedNumber * deltaTime);
	//		}
	//		if (GetAsyncKeyState('S'))
	//		{
	//			this->transform.Translate(Vec2DF::Down() * speedNumber * deltaTime);
	//		}
	//		if (GetAsyncKeyState('D'))
	//		{
	//			this->transform.Translate(Vec2DF::Right() * speedNumber * deltaTime);
	//		}
	//		if (GetAsyncKeyState('P') & 0x0001)
	//		{
	//			//this->framework->controller->Pause(false);
	//		}
	//		//if (GetAsyncKeyState('K') & 0x0001)
	//		//{
	//		//	this->framework->controller->AddEnemy();
	//		//}
	//		//if (GetAsyncKeyState('I') & 0x0001)
	//		//{
	//		//	this->Deactive();
	//		//}
	//		//if (GetAsyncKeyState('P') & 0x0001)
	//		//{
	//		//	this->framework->container->DropBullet(this->transform.Position - Vec2DF{40,40}, 3);
	//		//}
	//		//if (GetAsyncKeyState('O') & 0x0001)
	//		//{
	//		//	  this->Regen();
	//		//}
	//		if (GetAsyncKeyState(VK_UP))
	//		{
	//			CharginCount += deltaTime;
	//			if (CharginCount < maxCharge || !Utill::approximation(bulletRotate, rotate)) { this->bulletRotate += chargingSpeed * deltaTime; }
	//			else if (Utill::approximation(bulletRotate, rotate))
	//			{
	//				this->bulletRotate = rotate;
	//			}
	//			curCharging = Vec2DF::Up();
	//		}
	//		else if (GetAsyncKeyState(VK_LEFT))
	//		{
	//			CharginCount += deltaTime;
	//			if (CharginCount < maxCharge || !Utill::approximation(bulletRotate, rotate))
	//			{
	//				this->bulletRotate += chargingSpeed * deltaTime;
	//			}
	//			else if (Utill::approximation(bulletRotate, rotate))
	//			{
	//				this->bulletRotate = rotate;
	//			}
	//			curCharging = Vec2DF::Left();
	//		}
	//		else if (GetAsyncKeyState(VK_DOWN))
	//		{
	//			CharginCount += deltaTime;
	//			if (CharginCount < maxCharge || !Utill::approximation(bulletRotate, rotate)) { this->bulletRotate += chargingSpeed * deltaTime; }
	//			else if (Utill::approximation(bulletRotate, rotate))
	//			{
	//				this->bulletRotate = rotate;
	//			}
	//			curCharging = Vec2DF::Down();
	//		}
	//		else if (GetAsyncKeyState(VK_RIGHT))
	//		{
	//			CharginCount += deltaTime;
	//			if (CharginCount < maxCharge || !Utill::approximation(bulletRotate, rotate)) { this->bulletRotate += chargingSpeed * deltaTime; }
	//			else if (Utill::approximation(bulletRotate, rotate))
	//			{
	//				this->bulletRotate = rotate;
	//			}
	//			curCharging = Vec2DF::Right();
	//		}
	//		else
	//		{
	//			if (CharginCount > 0.0f && (this->BulletCount > 0 || this->SpecialBulletCount > 0))
	//			{
	//				bool isSpecialBullet = SpecialBulletCount > 0;
	//				float margin = 100;
	//				if (CharginCount > maxCharge)
	//				{
	//					auto posLeft = this->transform.Position + Vec2DF{ curCharging.y,curCharging.x } *this->transform.Size.x * 1.5f;
	//					auto posRight = this->transform.Position + Vec2DF{ curCharging.y,curCharging.x } *-1 * this->transform.Size.x * 1.5f;
	//					this->framework->container->AddBullet(posLeft, curCharging, isSpecialBullet);
	//					this->framework->container->AddBullet(this->transform.Position, curCharging, isSpecialBullet);
	//					this->framework->container->AddBullet(posRight, curCharging, isSpecialBullet);
	//					SpecialCharging = isSpecialBullet;
	//				}
	//				else
	//				{
	//					this->framework->container->AddBullet(this->transform.Position, curCharging, isSpecialBullet);
	//				}
	//				if (isSpecialBullet)
	//				{
	//					SpecialBulletCount = Utill::clamp(SpecialBulletCount - 1, 0, 6);
	//					SpecialDelay = SpecialMaxDelay;
	//				}
	//				else
	//				{
	//					BulletCount--;
	//				}
	//			}
	//			CharginCount = 0.0f;
	//		}
	//	}
	//}

}

void Player::Draw(PaintInfo info)
{
	if (this->state == GenState::Gen)
	{
		COLORREF color = RGB(0, 0, 0);
		HPEN hPen = CreatePen(PS_SOLID, 5, color);
		auto oldPen = (HPEN)SelectObject(info.hdc, hPen);
		auto oldBrush = (HBRUSH)SelectObject(info.hdc, GetStockObject(NULL_BRUSH));
		auto rt = RectF(this->transform.Position * info.AntiAliasing, (1 + CharginCount) * this->transform.Size.x * info.AntiAliasing, (1 + CharginCount) * this->transform.Size.y * info.AntiAliasing) + info.margin;
		SetBkMode(info.hdc, TRANSPARENT);
		RoundRect(info.hdc, rt.left, rt.top, rt.right, rt.bottom, 10, 10);
		SetBkMode(info.hdc, OPAQUE);
		SelectObject(info.hdc, oldPen);
		DeleteObject(hPen);
		SelectObject(info.hdc, oldBrush);

	}
	else if (this->state == GenState::On)
	{
		HPEN hPen;
		HBRUSH hBrush;
		COLORREF playerColor[4] = { RGB( 169, 14, 21 ) ,RGB( 0, 183, 0 ) ,RGB( 0, 75, 151 ) ,RGB( 0, 0, 0 ) };
		hPen = CreatePen(PS_SOLID, 7, playerColor[this->playerIndex]);
		hBrush = CreateSolidBrush( playerColor[this->playerIndex] );
		auto oldPen = (HPEN)SelectObject(info.hdc, hPen);
		auto oldBrush = (HBRUSH)SelectObject(info.hdc, hBrush);
		auto rt = RectF(this->transform.Position * info.AntiAliasing, this->transform.Size.x * info.AntiAliasing, this->transform.Size.y * info.AntiAliasing) + info.margin;
		RoundRect(info.hdc, rt.left, rt.top, rt.right, rt.bottom, 20, 20);
		Ellipse(info.hdc, rt.left, rt.top, rt.right, rt.bottom);

		size_t i = 0;
		for (i = 0; i < min(BulletCount + SpecialBulletCount, 6); i++)
		{
			COLORREF color = RGB(255, 255, 255);
			if (i < SpecialBulletCount)
			{
				color = RGB(56, 182, 214);
			}
			if (i == 0 && CharginCount > maxCharge)
			{
				color = RGB(255, 0, 0);
			}
			HPEN hBulletPen = CreatePen(PS_SOLID, 1, color);
			auto oldBulletPen = (HPEN)SelectObject(info.hdc, hBulletPen);
			HBRUSH hBulletBrush = CreateSolidBrush(color);
			auto oldBulletBrush = (HBRUSH)SelectObject(info.hdc, hBulletBrush);
			auto bulletPos = (this->transform.Position + Vec2DF{ 0,10 }).RotatedPoint(this->transform.Position, i * 60 + bulletRotate);
			auto bulletRt = RectF(bulletPos * info.AntiAliasing, this->transform.Size.x * 0.2 * info.AntiAliasing, this->transform.Size.y * 0.2 * info.AntiAliasing) + info.margin;
			Ellipse(info.hdc, bulletRt.left, bulletRt.top, bulletRt.right, bulletRt.bottom);
			SelectObject(info.hdc, oldBulletBrush);
			SelectObject(info.hdc, oldBulletPen);
			DeleteObject(hBulletBrush);
			DeleteObject(hBulletPen);
		}
		if (i < 6)
		{
			COLORREF color = RGB(255 * this->regenTime / bulletRegenDelay, 255 * this->regenTime / bulletRegenDelay, 255 * this->regenTime / bulletRegenDelay);
			HPEN hBulletPen = CreatePen(PS_SOLID, 1, color);
			auto oldBulletPen = (HPEN)SelectObject(info.hdc, hBulletPen);
			HBRUSH hBulletBrush = CreateSolidBrush(color);
			auto oldBulletBrush = (HBRUSH)SelectObject(info.hdc, hBulletBrush);
			auto bulletPos = (this->transform.Position + Vec2DF{ 0,10 }).RotatedPoint(this->transform.Position, (i) * 60 + bulletRotate);
			auto bulletRt = RectF(bulletPos * info.AntiAliasing, this->transform.Size.x * 0.2 * info.AntiAliasing, this->transform.Size.y * 0.2 * info.AntiAliasing) + info.margin;
			Ellipse(info.hdc, bulletRt.left, bulletRt.top, bulletRt.right, bulletRt.bottom);
			SelectObject(info.hdc, oldBulletBrush);
			SelectObject(info.hdc, oldBulletPen);
			DeleteObject(hBulletBrush);
			DeleteObject(hBulletPen);
		}

		SelectObject(info.hdc, oldPen);
		SelectObject(info.hdc, oldBrush);
		DeleteObject(hBrush);
		DeleteObject(hPen);
	}
}
void Player::Deactive()
{
	if (!this->invulnerable)
	{
		GameObject::Deactive();
		this->state = GenState::Off;
		//this->framework->controller->PostPlayerDead();
		this->framework->container->ActiveExplosion(this->transform.Position, RGB(0, 0, 0), false);
	}
}
bool Player::GetActiveState() const
{
	return this->state == GenState::On && this->isActive;
}
void Player::Reset()
{
	CharginCount = 10.0f;
	SpecialBulletCount = 0;
	BulletCount = 0;
	life = 0;
	bulletRotate = 0;
	regenTime = 0;
	SpecialDelay = 0.0f;
	SpecialCharging = false;
	GameObject::Deactive();
	this->state = GenState::Off;
	firstGen = true;
}
void Player::Regen()
{
	this->state = GenState::Gen;
	this->Active();
	CharginCount = 10.0f;
	SpecialBulletCount = 0;
	BulletCount = 0;
	life = 0;
	bulletRotate = 0;
	regenTime = 0;
	SpecialDelay = 0.0f;
	SpecialCharging = false;
	if (!firstGen)
	{
		this->framework->container->ActiveExplosion(this->transform.Position, RGB(0, 0, 0));
	}
	else
	{
		this->transform.Position = (this->framework->GetDisplaySize() / 2) + Vec2DF{ 40.0f,40.0f };
	}
	firstGen = false;
}

void Player::AddSpecialBullet(int count)
{
	this->SpecialBulletCount += count;
}

void Player::OnInvulnerable()
{
	this->invulnerable = true;
}

void Player::OffInvulnerable()
{
	this->invulnerable = false;
}
bool Player::GetInvulnerable() const
{
	return this->invulnerable;
}

bool Player::SwtichInvulnerable()
{
	if (this->invulnerable)
	{
		this->invulnerable = false;
	}
	else
	{
		this->invulnerable = true;
	}
	return this->invulnerable;
}
