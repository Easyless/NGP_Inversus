#pragma once
#include <list>
#include <memory>
#include "Map.h"
#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "Explosion.h"
#include "SpawnEffect.h"

struct InversusContainer
{
	InversusContainer();
	Map BlockMap;
	std::vector<Player> player;
	std::vector<Enemy> enemys;
	std::vector<Bullet> bullets;
	//std::list<Bullet> bullets;
	std::list<Explosion> explosionEffect;
	std::list<SpawnEffect> spawnEffect;
	std::list<std::unique_ptr<RefreshObject<int>>> UIObject;
	void Start();
	//void AddBullet(Vec2DF position, Vec2DF moveVec, bool isSpecial = false);
	void AddSpawnEffect( Vec2DF position, COLORREF color);
	void AddExplosion( Vec2DF position, COLORREF color, bool isCollision = true );
	void RefreshBulletFromData( const BulletDatas& datas );
	void RefreshEnemyFromData( const MobDatas& datas );
	void RefreshPlayersFromData( const GameSceneData& data );
	void RefreshMapFromData( const GameSceneData& data );
	void CollectGarbage();
	Vec2DF GetGameDisplaySize() const;
	Vec2DF GetMargin() const;
	void Reset();

};

enum class GameState
{
	On, Off , Over
};