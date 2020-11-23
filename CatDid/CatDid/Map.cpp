#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "GameObject.h"
#include "InversusClasses.h"
#include "InversusFramework.h"
#include "Difficulty.h"
// ¸Ê
Map::Map(InversusFramework* framework , size_t sizeX, size_t sizeY, Vec2DF blockSize)
	: GameObject(framework,"Map") , sizeX(sizeX), sizeY(sizeY) , blockSize(blockSize)
{
	map.reserve(sizeX * sizeY);
	for (size_t i = 0; i < sizeX * sizeY; i++)
	{
		map.emplace_back(framework);
	}

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
	for (auto& block : map)
	{
		block.Draw(info);
	}
}


Block::Block(InversusFramework* framework)
	:GameObject(framework,"Block")
{
	this->transform.Size = Vec2DF{ 25,25 };
	this->transform.SetCheckMap("Bullet", true);
	this->transform.SetCheckMap("Explosion", true);
}

// ºí·°
void Block::Update(float deltaTime)
{
}

void Block::Draw(PaintInfo info)
{
	this->DrawHatched = false;
	auto penColor = Block::GetPenColor(this->state);
	auto brColor = Block::GetBrushColor(this->state);
	HPEN hPen = CreatePen(PS_SOLID, 2, penColor);
	auto oldPen = (HPEN)SelectObject(info.hdc, hPen);
	HBRUSH hBr = CreateSolidBrush(brColor);
	auto oldBr = (HBRUSH)SelectObject(info.hdc, hBr);
	auto rt = RectF(this->transform.Position * info.AntiAliasing, this->transform.Size.x * info.AntiAliasing, this->transform.Size.y * info.AntiAliasing) + info.margin;
	Rectangle(info.hdc, rt.left, rt.top, rt.right, rt.bottom);
	SelectObject(info.hdc, oldPen);
	SelectObject(info.hdc, oldBr);
	DeleteObject(hBr);
	DeleteObject(hPen);
}
bool Block::isCollision(GameObject& other)
{
	if (BlockState::Off != state)
	{
		if (this->state == BlockState::BreakableOn)
		{
			if (other.tag == "Player")
			{
				return true && !((Player&)other).GetInvulnerable();
			}
			else if (other.tag == "Explosion" || other.tag == "Bullet")
			{
				this->Deactive();
			}
		}
		else if (this->state == BlockState::UnbreakableOn)
		{
			if (other.tag == "Player")
			{
				return true;
			}
		}
	}
	return false;
}
void Block::Active() 
{
	GameObject::Active(); 
	if (this->state == BlockState::Off)
	{
		this->state = BlockState::BreakableOn; 
		if (this->framework->container != nullptr && this->framework->container->player.state == GenState::On && this->framework->container->player.transform.CheckCollision(this->transform))
		{
			this->framework->container->player.Deactive();
		}
	}
}
void Block::Deactive() 
{
	GameObject::Deactive(); 
	this->state = BlockState::Off; 
}
std::pair<size_t, size_t> Map::GetSize() const 
{
	return std::make_pair(this->sizeX, this->sizeY); 
}

Vec2DF Map::GetBlockSize() const 
{
	return blockSize; 
}

void Map::Reset()
{
	for ( int y = 0; y < sizeY; y++ )
	{
		for ( int x = 0; x < sizeX; x++ )
		{
			auto& target = map[y * sizeX + x];
			target.Deactive();
			target.Active();
			target.transform.Size = blockSize;
			target.transform.Position = Vec2DF( target.transform.Size.x * x + target.transform.Size.x / 2, target.transform.Size.y * y + target.transform.Size.y / 2 );
		}
	}
}

void Map::Reset(Difficulty dif)
{
	switch (dif)
	{
	case Difficulty::Normal:
	{
		for (int y = 0; y < sizeY; y++)
		{
			for (int x = 0; x < sizeX; x++)
			{
				auto& target = map[y * sizeX + x];
				target.Deactive();
				switch (diffMap[0][y][x])
				{
				case -1: target.Active(); target.state = BlockState::UnbreakableOn; break;
				case 00: break;
				case 01: target.Active(); break;
				}
				target.transform.Size = blockSize;
				target.transform.Position = Vec2DF(target.transform.Size.x * x + target.transform.Size.x / 2, target.transform.Size.y * y + target.transform.Size.y / 2);
				if (y == 0 || y == sizeY - 1 || x == 0 || x == sizeX - 1)
				{
					target.Active();
					target.state = BlockState::UnbreakableOn;
				}
			}
		}
	}
	break;
	case Difficulty::Hard:
	{
				for (int y = 0; y < sizeY; y++)
		{
			for (int x = 0; x < sizeX; x++)
			{
				auto& target = map[y * sizeX + x];
				target.Deactive();
				switch (diffMap[1][y][x])
				{
				case -1: target.Active(); target.state = BlockState::UnbreakableOn; break;
				case 00: break;
				case 01: target.Active(); break;
				}
				target.transform.Size = blockSize;
				target.transform.Position = Vec2DF(target.transform.Size.x * x + target.transform.Size.x / 2, target.transform.Size.y * y + target.transform.Size.y / 2);
				if (y == 0 || y == sizeY - 1 || x == 0 || x == sizeX - 1)
				{
					target.Active();
					target.state = BlockState::UnbreakableOn;
				}
			}
		}
	}
	break;	
	case Difficulty::Extreme:
	{
		for (int y = 0; y < sizeY; y++)
		{
			for (int x = 0; x < sizeX; x++)
			{
				auto& target = map[y * sizeX + x];
				target.Deactive();
				switch (diffMap[2][y][x])
				{
				case -1: target.Active(); target.state = BlockState::UnbreakableOn; break;
				case 00: break;
				case 01: target.Active(); break;
				}
				target.transform.Size = blockSize;
				target.transform.Position = Vec2DF(target.transform.Size.x * x + target.transform.Size.x / 2, target.transform.Size.y * y + target.transform.Size.y / 2);
				if (y == 0 || y == sizeY - 1 || x == 0 || x == sizeX - 1)
				{
					target.Active();
					target.state = BlockState::UnbreakableOn;
				}
			}
		}
	}
	break;
	}
}

void Map::RefreshFromData( const GameSceneData& data )
{
	for ( size_t y = 0; y < BLOCK_COUNT_Y; y++ )
	{
		for ( size_t x = 0; x < BLOCK_COUNT_X; x++ )
		{
			this->GetBlock( x, y ).state = data.mapData.blockState[y][x] ? BlockState::BreakableOn : BlockState::Off;
		}
	}

}
