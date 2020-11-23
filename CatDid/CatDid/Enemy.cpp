#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "InversusClasses.h"
#include "InversusFramework.h"
//#define DIVDRAW


Enemy::Enemy()
{
	this->transform.Size = Vec2DF{ 25,25 };
}

void Enemy::RefreshFromData( const MobData& data )
{
	this->isSpecial = data.isSpecialMob;
	this->transform.Position.x = data.positionX;
	this->transform.Position.y = data.positionY;
}

void Enemy::Update(float deltaTime)
{
}
void Enemy::Draw(PaintInfo info)
{
	if (isRender)
	{
		COLORREF color = isSpecial ? RGB( 214, 56, 56 ) : RGB( 56, 182, 214 );
#ifdef DIVDRAW


		{
			auto idx = InversusFramework::GetInstance()->container->BlockMap.GetPositionIndex( this->transform.Position );
			int centerX = idx.first;
			int centerY = idx.second;

			int idxArr[9][2] =
			{
				{centerX - 1,centerY - 1},{centerX,centerY - 1},{centerX + 1,centerY - 1},
				{centerX - 1,centerY},{centerX,centerY},{centerX + 1,centerY},
				{centerX - 1,centerY + 1},{centerX,centerY + 1},{centerX + 1,centerY + 1}
			};
			for ( size_t i = 0; i < 9; i++ )
			{
				if ( idxArr[i][0] >= 0 && idxArr[i][0] < InversusFramework::GetInstance()->container->BlockMap.GetSize().first
					&& idxArr[i][1] >= 0 && idxArr[i][1] < InversusFramework::GetInstance()->container->BlockMap.GetSize().second )
				{

					auto& targetObj = InversusFramework::GetInstance()->container->BlockMap.GetBlock( idxArr[i][0], idxArr[i][1] );
					if ( targetObj.DrawHatched == false )
					{
						HPEN hPen = CreatePen( PS_SOLID, 3, color );
						auto oldPen = (HPEN)SelectObject( info.hdc, hPen );
						HBRUSH hBrush = CreateHatchBrush( HS_FDIAGONAL, color );
						auto oldBrush = (HBRUSH)SelectObject( info.hdc, hBrush );
						SetBkMode( info.hdc, TRANSPARENT );
						auto blockRt = RectF( targetObj.transform.Position * info.AntiAliasing, targetObj.transform.Size.x * info.AntiAliasing, targetObj.transform.Size.y * info.AntiAliasing ) + info.margin;
						//Rectangle(info.hdc, blockRt.left, blockRt.top, blockRt.right, blockRt.bottom);
						SetBkMode( info.hdc, OPAQUE );
						SelectObject( info.hdc, oldPen );
						SelectObject( info.hdc, oldBrush );
						DeleteObject( hPen );
						DeleteObject( hBrush );
						targetObj.DrawHatched = true;
					}
				}
			}
		}
#else
		{
			auto idx = InversusFramework::GetInstance()->container->BlockMap.GetPositionIndex( this->transform.Position );
			int centerX = idx.first;
			int centerY = idx.second;
			auto targetPosition = InversusFramework::GetInstance()->container->BlockMap.GetBlockPosition( centerX, centerY );
			auto targetSize = InversusFramework::GetInstance()->container->BlockMap.GetBlockSize();
			HPEN hPen = CreatePen(PS_SOLID, 3, color);
			auto oldPen = (HPEN)SelectObject(info.hdc, hPen);
			HBRUSH hBrush = CreateHatchBrush(HS_FDIAGONAL, color);
			auto oldBrush = (HBRUSH)SelectObject(info.hdc, hBrush);
			SetBkMode(info.hdc, TRANSPARENT);
			auto blockRt = RectF( targetPosition * info.AntiAliasing, targetSize.x * 3 * info.AntiAliasing, targetSize.y * 3 * info.AntiAliasing) + info.margin;
			Rectangle(info.hdc, blockRt.left, blockRt.top, blockRt.right, blockRt.bottom);
			SetBkMode(info.hdc, OPAQUE);
			SelectObject(info.hdc, oldPen);
			SelectObject(info.hdc, oldBrush);
			DeleteObject(hPen);
			DeleteObject(hBrush);
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