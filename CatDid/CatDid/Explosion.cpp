#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "NetworkDummyObject.h"
#include "Explosion.h"

void _ExplosionEffect::Draw( PaintInfo info, float thisTime, Vec2DF mainPosition, COLORREF color )
{
	if ( startTime <= thisTime && thisTime <= startTime + duration )
	{
		HPEN hPen = CreatePen( PS_SOLID, 5, color );
		auto oldPen = (HPEN)SelectObject( info.hdc, hPen );
		HBRUSH hBrush = CreateSolidBrush( color );
		auto oldBrush = (HBRUSH)SelectObject( info.hdc, hBrush );
		float scale = -1 * (thisTime - startTime) * (thisTime - startTime - duration) / pow( duration, 2 ) * 4 * maxScale;
		auto rt = RectF( (mainPosition + this->position) * info.AntiAliasing, scale * info.AntiAliasing, scale * info.AntiAliasing ) + info.margin;
		Ellipse( info.hdc, rt.left, rt.top, rt.right, rt.bottom );
		SelectObject( info.hdc, oldPen );
		DeleteObject( hPen );
		SelectObject( info.hdc, oldBrush );
		DeleteObject( hBrush );
	}
}

Explosion::Explosion( Vec2DF position, COLORREF color )
{
	this->color = color;
	this->time = 0;
	subEffect.clear();
	size_t particleSize = 8 + rand() % 8;
	subEffect.resize( particleSize );
	mainPosition = position;
	for ( auto& sub : subEffect )
	{
		sub.position = Vec2DF{
			((rand() % 2 == 0) ? -1 : 1) * 15.0f * (rand() % 4),
			((rand() % 2 == 0) ? -1 : 1) * 15.0f * (rand() % 4)
		};
		sub.startTime = 0.05f * (rand() % 6);
		sub.maxScale = 60.0 + ((rand() % 20) - 10);
		endTime = (endTime < sub.startTime + sub.duration) ? sub.startTime + sub.duration : endTime;
	}
}
void Explosion::Update( float deltaTime )
{
	if ( !this->isDestroy )
	{
		time += deltaTime;
		if ( time > endTime + 1 )
		{
			this->Destroy();
		}
	}
}
void Explosion::Draw( PaintInfo info )
{
	if ( isRender )
	{
		for ( auto& sub : subEffect )
		{
			sub.Draw( info, time, mainPosition, color );
		}
	}
}
