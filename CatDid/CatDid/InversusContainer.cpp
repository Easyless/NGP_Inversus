#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "GameObject.h"
#include "InversusClasses.h"
#include "InversusFramework.h"
#include "UIObject.h"

InversusContainer::InversusContainer(InversusFramework* framework)
	:framework( framework ), BlockMap( framework, 21, 21, Vec2DF{ 40,40 } )
{
	for ( size_t i = 0; i < 4; i++ )
	{
		this->player.emplace_back( framework );
	}
	this->UIObject.push_back(std::move(std::make_unique<UILifeObject>(this->framework)));
	this->UIObject.push_back(std::move(std::make_unique<UIScoreObject>(this->framework)));
}
void InversusContainer::Start()
{
}
void InversusContainer::AddBullet(Vec2DF position, Vec2DF moveVec, bool isSpecial)
{
	bool haveToCreate = true;
	for (auto& bullet : bullets)
	{
		if (bullet.GetActiveState() == false)
		{
			bullet.TurnActive(position,moveVec, isSpecial);
			haveToCreate = false;
			break;
		}
	}
	if (haveToCreate)
	{
		bullets.emplace_back(this->framework);
		bullets.back().TurnActive(position, moveVec, isSpecial);
	}
}
void InversusContainer::AddEnemy(Vec2DF Position, bool isSpecial)
{
	bool haveToCreate = true;
	for (auto& enemy : enemys)
	{
		if (enemy.state == GenState::Off)
		{
			enemy.TurnActive(Position, isSpecial);
			haveToCreate = false;
			break;
		}
	}
	if (haveToCreate)
	{
		enemys.emplace_back(this->framework);
		enemys.back().TurnActive(Position, isSpecial);
	}
}

void InversusContainer::ActiveExplosion(Vec2DF position, COLORREF color, bool isCollision)
{
	bool haveToCreate = true;
	for (auto& effect : explosionEffect)
	{
		if (effect.state == GenState::Off)
		{
			effect.TurnActive(position,color, isCollision);
			haveToCreate = false;
			break;
		}
	}
	if (haveToCreate)
	{
		explosionEffect.emplace_back(this->framework);
		explosionEffect.back().TurnActive(position,color, isCollision);
	}
}

void InversusContainer::DropBullet(Vec2DF position, int count)
{
	bool haveToCreate = true;
	for (auto& drop : dropBullets)
	{
		if (!drop.GetActiveState())
		{
			drop.ActiveDrop(position, count);
			haveToCreate = false;
			break;
		}
	}
	if (haveToCreate)
	{
		dropBullets.emplace_back(this->framework);
		dropBullets.back().ActiveDrop(position, count);
	}
}

void InversusContainer::RefreshEnemyFromData( const MobDatas& datas )
{
}

Vec2DF InversusContainer::GetGameDisplaySize() const
{
	auto blockCount = this->BlockMap.GetSize();
	blockCount.first -= 2;
	blockCount.second -= 2;
	auto blockSize = this->BlockMap.GetBlockSize();
	return Vec2DF{ blockSize.x*blockCount.first,blockSize.y * blockCount.second };

}
Vec2DF InversusContainer::GetMargin() const
{
	auto blockSize = this->BlockMap.GetBlockSize();
	//int expNum = static_cast<int>(this->framework->controller->explosionNum);
	//blockSize.x += sin(expNum + (rand()%4) / 1.5f) * expNum;
	//blockSize.y += sin(expNum + (rand()%4) / 1.5f) * expNum;
	return blockSize;
}
void InversusContainer::Reset(Difficulty diff)
{
	this->BlockMap.Reset(diff);
	for ( auto& p : player )
	{
		p.Reset();
	}
	for (auto& effect : explosionEffect) { effect.Reset(); }
	for (auto& bullet : bullets) { bullet.Reset(); }
	for (auto& enemy : enemys) { enemy.Reset(); }
	for (auto& drop : dropBullets) { drop.Reset(); }
}
