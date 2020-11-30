#include "stdafx.h"
#include "UtillFunction.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "InversusFramework.h"
#include "Player.h"
#include "InversusClasses.h"


Player::Player( UINT index )
{
	this->isRender = false;
	this->playerIndex = index;
	this->transform.Size = Vec2DF{ 25,25 };
}

void Player::RefreshFromData( const GameSceneData& data )
{
	this->isRender = !data.playerState[playerIndex].isDead;
	if ( this->BulletCount != data.playerState[playerIndex].remainBullet )
	{
		this->regenTime = 0.0f;
	}
	this->BulletCount = data.playerState[playerIndex].remainBullet;
	this->transform.Position.x = data.playerState[playerIndex].positionX;
	this->transform.Position.y = data.playerState[playerIndex].positionY;
}

void Player::Update( float deltaTime )
{
	this->bulletRotate += rotateSpeed * deltaTime;
	this->regenTime += deltaTime;
}

void Player::Draw( PaintInfo info )
{
	if ( this->isRender )
	{
		HPEN hPen;
		HBRUSH hBrush;
		COLORREF playerColor[4] = { RGB( 169, 14, 21 ) ,RGB( 0, 183, 0 ) ,RGB( 0, 75, 151 ) ,RGB( 0, 0, 0 ) };
		hPen = CreatePen( PS_SOLID, 7, playerColor[this->playerIndex] );
		hBrush = CreateSolidBrush( playerColor[this->playerIndex] );
		auto oldPen = (HPEN)SelectObject( info.hdc, hPen );
		auto oldBrush = (HBRUSH)SelectObject( info.hdc, hBrush );
		auto rt = RectF( this->transform.Position * info.AntiAliasing, this->transform.Size.x * info.AntiAliasing, this->transform.Size.y * info.AntiAliasing ) + info.margin;
		RoundRect( info.hdc, rt.left, rt.top, rt.right, rt.bottom, 20, 20 );
		Ellipse( info.hdc, rt.left, rt.top, rt.right, rt.bottom );

		size_t i = 0;
		size_t currentCount = min( BulletCount, 6 );
		for ( i = 0; i < min( BulletCount, 6 ); i++ )
		{
			COLORREF color = RGB( 255, 255, 255 );
			if ( i == 0 && CharginCount > maxCharge )
			{
				color = RGB( 255, 0, 0 );
			}
			HPEN hBulletPen = CreatePen( PS_SOLID, 1, color );
			auto oldBulletPen = (HPEN)SelectObject( info.hdc, hBulletPen );
			HBRUSH hBulletBrush = CreateSolidBrush( color );
			auto oldBulletBrush = (HBRUSH)SelectObject( info.hdc, hBulletBrush );
			auto bulletPos = (this->transform.Position + Vec2DF{ 0,10 }).RotatedPoint( this->transform.Position, i * 60 + bulletRotate );
			//float scale = min( 1.0f, (i == currentCount - 1) ? this->regenTime : 1.0f );
			float scale = 1.0f;
			auto bulletRt = RectF( bulletPos * info.AntiAliasing, this->transform.Size.x * 0.2 * info.AntiAliasing * scale, this->transform.Size.y * 0.2 * info.AntiAliasing * scale ) + info.margin;
			Ellipse( info.hdc, bulletRt.left, bulletRt.top, bulletRt.right, bulletRt.bottom );
			SelectObject( info.hdc, oldBulletBrush );
			SelectObject( info.hdc, oldBulletPen );
			DeleteObject( hBulletBrush );
			DeleteObject( hBulletPen );
		}
		if ( i < 6 )
		{
			//COLORREF color = RGB( 255 * this->regenTime / bulletRegenDelay, 255 * this->regenTime / bulletRegenDelay, 255 * this->regenTime / bulletRegenDelay );
			COLORREF color = RGB( 255, 255, 255 );
			HPEN hBulletPen = CreatePen( PS_SOLID, 1, color );
			auto oldBulletPen = (HPEN)SelectObject( info.hdc, hBulletPen );
			HBRUSH hBulletBrush = CreateSolidBrush( color );
			auto oldBulletBrush = (HBRUSH)SelectObject( info.hdc, hBulletBrush );
			auto bulletPos = (this->transform.Position + Vec2DF{ 0,10 }).RotatedPoint( this->transform.Position, (i) * 60 + bulletRotate );
			float scale = min(this->regenTime, 1.0f );
			auto bulletRt = RectF( bulletPos * info.AntiAliasing, this->transform.Size.x * 0.2 * info.AntiAliasing * scale, this->transform.Size.y * 0.2 * info.AntiAliasing * scale ) + info.margin;
			Ellipse( info.hdc, bulletRt.left, bulletRt.top, bulletRt.right, bulletRt.bottom );
			SelectObject( info.hdc, oldBulletBrush );
			SelectObject( info.hdc, oldBulletPen );
			DeleteObject( hBulletBrush );
			DeleteObject( hBulletPen );
		}

		SelectObject( info.hdc, oldPen );
		SelectObject( info.hdc, oldBrush );
		DeleteObject( hBrush );
		DeleteObject( hPen );
	}
}
