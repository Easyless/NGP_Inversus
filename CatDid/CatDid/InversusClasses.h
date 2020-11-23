#pragma once
#include <list>
#include <memory>
#include "GameObject.h"

#include "Map.h"
#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "Explosion.h"
#include "DropBullet.h"


struct InversusContainer
{
	InversusContainer(InversusFramework* framework);

	InversusFramework* framework = nullptr;

	Map BlockMap;
	std::vector<Player> player;
	std::list<Bullet> bullets;
	std::list<Enemy> enemys;
	std::list<Explosion> explosionEffect;
	std::list<DropBullet> dropBullets;
	std::list<std::unique_ptr<GameObject>> UIObject;
	void Start();
	void AddBullet(Vec2DF position, Vec2DF moveVec, bool isSpecial = false);
	void AddEnemy(Vec2DF Position,bool isSpecial = false);
	void ActiveExplosion(Vec2DF position, COLORREF color, bool isCollision = true);
	void DropBullet(Vec2DF position, int count);
	Vec2DF GetGameDisplaySize() const;
	Vec2DF GetMargin() const;
	void Reset(Difficulty diff);

};

enum class GameState
{
	On, Off , Over
};

struct InversusController
{
	Difficulty curDiff = Difficulty::Normal;
	InversusController(InversusFramework* framework) :framework(framework){}
	InversusFramework* framework = nullptr;
	static constexpr float playerRegenTime = 2;
	static constexpr float ScoreMax = 9999999999;
	float maxGenTime = 4;
	float minGenTime = 0.125;
	float maxComboTime = 4;
	float minComboTime = 1.5;
	GameState state = GameState::Off;
	int Score = 0;
	int Combo = 0;
	int life = 3;
	float curComboTime = 0;
	float curMaxComboTime = maxComboTime;
	float curEnemyGenTime = 0;
	float curEnemyGenCount = 0;
	float curPlayerGenCount = 0;
	float explosionNum = 0;
	bool isCombing = false;
	bool regening = false;
	void Reset(Difficulty diff);
	void Start();
	void Update(float deltaTime);
	void AddEnemy();
	void PostPlayerDead();
	void PostGameOver();
	void PostExplosion();
	void PostAddLife();
	void PostAddScore(bool isSpecial = false);
	void BreakCombo();
	void Pause(bool isRestart = false);
};