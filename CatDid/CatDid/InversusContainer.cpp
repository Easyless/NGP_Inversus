#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "InversusClasses.h"
#include "InversusFramework.h"
#include "UIObject.h"

InversusContainer::InversusContainer()
	:BlockMap(BLOCK_COUNT_X, BLOCK_COUNT_Y, Vec2DF{ 40,40 } )
{
	for ( size_t i = 0; i < 4; i++ )
	{
		this->player.emplace_back( i );
	}
	this->UIObject.push_back(std::move(std::make_unique<UILifeObject>()));
	this->UIObject.push_back(std::move(std::make_unique<UIScoreObject>()));
}
void InversusContainer::Start()
{
}
//void InversusContainer::AddBullet(Vec2DF position, Vec2DF moveVec, bool isSpecial)
//{
//	bullets.emplace_back(position, moveVec);
//}

void InversusContainer::AddExplosion(Vec2DF position, COLORREF color, bool isCollision)
{
	this->explosionEffect.emplace_back( position, color );
}

void InversusContainer::RefreshBulletFromData( const BulletDatas& datas )
{
	this->bullets.resize( datas.size() );
	for ( size_t i = 0; i < this->bullets.size(); i++ )
	{
		this->bullets[i].RefreshFromData( datas[i] );
	}
}

void InversusContainer::RefreshEnemyFromData( const MobDatas& datas )
{
	this->enemys.resize( datas.size() );
	for ( size_t i = 0; i < this->enemys.size(); i++ )
	{
		this->enemys[i].RefreshFromData( datas[i] );
	}
}

void InversusContainer::RefreshPlayersFromData( const GameSceneData& data )
{
	for ( size_t i = 0; i < 4; i++ )
	{
		this->player[i].RefreshFromData( data );
	}
}

void InversusContainer::RefreshMapFromData( const GameSceneData& data )
{
	this->BlockMap.RefreshFromData( data );
}

void InversusContainer::CollectGarbage()
{
	//for ( auto i = this->bullets.begin(); i != this->bullets.end(); )
	//{
	//	if ( i->IsDestroy() )
	//	{
	//		i = this->bullets.erase( i );
	//	}
	//	else 
	//	{
	//		++i;
	//	}
	//}

	//for ( auto i = this->explosionEffect.begin(); i != this->explosionEffect.end(); )
	//{
	//	if ( i->IsDestroy() )
	//	{
	//		i = this->explosionEffect.erase( i );
	//	}
	//	else
	//	{
	//		++i;
	//	}
	//}
}

Vec2DF InversusContainer::GetGameDisplaySize() const
{
	auto blockCount = this->BlockMap.GetSize();
	//blockCount.first -= 2;
	//blockCount.second -= 2;
	auto blockSize = this->BlockMap.GetBlockSize();
	return Vec2DF{ blockSize.x*blockCount.first,blockSize.y * blockCount.second };

}
Vec2DF InversusContainer::GetMargin() const
{
	//auto blockSize = this->BlockMap.GetBlockSize();
	auto blockSize = Vec2DF{0,0};
	//int expNum = static_cast<int>(this->framework->controller->explosionNum);
	//blockSize.x += sin(expNum + (rand()%4) / 1.5f) * expNum;
	//blockSize.y += sin(expNum + (rand()%4) / 1.5f) * expNum;
	return blockSize;
}
void InversusContainer::Reset()
{
	this->enemys.clear();
	this->bullets.clear();
	this->explosionEffect.clear();
	this->bullets.clear();
}
