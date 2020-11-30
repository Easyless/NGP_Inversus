#include "stdafx.h"
#include "SpawnEffect.h"
#include "LowLevelData.h"

SpawnEffect::SpawnEffect( Vec2DF position, COLORREF color )
{
	this->transform.Position = position;
	this->color = color;
	this->ChargingCount = 10.15f;
	this->transform.Size = Vec2DF{ PLAYER_SIZE, PLAYER_SIZE };
}

void SpawnEffect::Update( float deltaTime )
{
	ChargingCount -= 30 * deltaTime;
	if ( ChargingCount < 0 )
	{
		this->Destroy();
	}
}

void SpawnEffect::Draw( PaintInfo info )
{
	if ( this->isRender )
	{
		HPEN hPen = CreatePen( PS_SOLID, 5, color );
		auto oldPen = (HPEN)SelectObject( info.hdc, hPen );
		auto oldBrush = (HBRUSH)SelectObject( info.hdc, GetStockObject( NULL_BRUSH ) );
		auto rt = RectF( this->transform.Position * info.AntiAliasing, (1 + ChargingCount) * this->transform.Size.x * info.AntiAliasing, (1 + ChargingCount) * this->transform.Size.y * info.AntiAliasing ) +    info.margin;
		SetBkMode( info.hdc, TRANSPARENT );
		RoundRect( info.hdc, rt.left, rt.top, rt.right, rt.bottom, 20, 20 );
		SetBkMode( info.hdc, OPAQUE );
		SelectObject( info.hdc, oldPen );
		DeleteObject( hPen );
		SelectObject( info.hdc, oldBrush );
	}
}
