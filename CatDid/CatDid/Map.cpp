#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "InversusClasses.h"
#include "InversusFramework.h"
// ¸Ê
Map::Map(size_t sizeX, size_t sizeY, Vec2DF blockSize)
	: sizeX(sizeX), sizeY(sizeY) , blockSize(blockSize)
{
	map.resize(sizeX * sizeY, BlockState::Off);
}
std::pair<size_t, size_t> Map::GetPositionIndex(Vec2DF Position) const
{
	return std::make_pair(Position.x / blockSize.x, Position.y / blockSize.y);
}

void Map::Update(float deltaTime)
{
}
void Map::Draw(PaintInfo info)
{
	for ( size_t y = 0; y < BLOCK_COUNT_Y; y++ )
	{
		for ( size_t x = 0; x < BLOCK_COUNT_X; x++ )
		{
			this->DrawBlock( info, GetBlockPosition( x, y ), blockSize, this->GetBlock( x, y ) );
		}
	}
}
Vec2DF Map::GetBlockPosition( int x, int y )
{
	return Vec2DF( blockSize.x * x + blockSize.x / 2, blockSize.y * y + blockSize.y / 2 );
}


std::pair<size_t, size_t> Map::GetSize() const 
{
	return std::make_pair(this->sizeX, this->sizeY); 
}


static constexpr COLORREF unbreakbleColor = RGB( 60, 60, 60 );
static constexpr COLORREF unbreakblePenColor = RGB( 60, 60, 60 );
static constexpr COLORREF breakbleColor = RGB( 0, 0, 0 );
static constexpr COLORREF breakblePenColor = RGB( 100, 100, 120 );
static constexpr COLORREF offColor = RGB( 255, 255, 255 );
static constexpr COLORREF offPenColor = RGB( 100, 100, 120 );
static COLORREF GetBrushColor( BlockState state )
{
	switch ( state )
	{
	case BlockState::BreakableOn: return breakbleColor;
	case BlockState::UnbreakableOn: return unbreakbleColor;
	case BlockState::Off: return offColor;
	default:
		break;
	}
}
static COLORREF GetPenColor( BlockState state )
{
	switch ( state )
	{
	case BlockState::BreakableOn: return breakblePenColor;
	case BlockState::UnbreakableOn: return unbreakblePenColor;
	case BlockState::Off: return offPenColor;
	default:
		break;
	}
}

void Map::DrawBlock( PaintInfo info, const Vec2DF& position, const Vec2DF& size, BlockState state )
{
	auto penColor = GetPenColor( state );
	auto brColor = GetBrushColor( state );
	HPEN hPen = CreatePen( PS_SOLID, 2, penColor );
	auto oldPen = (HPEN)SelectObject( info.hdc, hPen );
	HBRUSH hBr = CreateSolidBrush( brColor );
	auto oldBr = (HBRUSH)SelectObject( info.hdc, hBr );
	auto rt = RectF( position * info.AntiAliasing, blockSize.x * info.AntiAliasing, blockSize.y * info.AntiAliasing ) + info.margin;
	SetBkMode( info.hdc, 0 );
	Rectangle( info.hdc, rt.left, rt.top, rt.right, rt.bottom );
	SelectObject( info.hdc, oldPen );
	SelectObject( info.hdc, oldBr );
	DeleteObject( hBr );
	DeleteObject( hPen );
}

Vec2DF Map::GetBlockSize() const 
{
	return blockSize; 
}

void Map::RefreshFromData( const GameSceneData& data )
{
	for ( size_t y = 0; y < BLOCK_COUNT_Y; y++ )
	{
		for ( size_t x = 0; x < BLOCK_COUNT_X; x++ )
		{
			this->GetBlock( x, y ) = data.mapData.blockState[y][x] ? BlockState::BreakableOn : BlockState::Off;
		}
	}
}
