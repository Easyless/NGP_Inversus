#pragma once
#pragma once
#include <list>
#include <memory>

#include "ClientSocketManager.h"
#include "Map.h"
#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "Explosion.h"


struct InversusNetworkController
{
	InversusNetworkController( InversusFramework* framework ) :framework( framework ) {}
	InversusFramework* framework = nullptr;
	ClientSocketManager* socket = nullptr;
	GameSceneData sceneData;
	MobDatas mobData;
	BulletDatas bulletData;
	PlayerInput input;
	EventDatas explosionDatas;
	EventDatas spawnDatas;
	bool startGame = false;
	void InitlizeWithSocket( ClientSocketManager* socket );
	void Reset();
	void Start();
	void Update( float deltaTime );
	void RecvData();
	void RefreshPlayerData();
	void RefreshBulletData();
	void RefreshMapData();
	void RefreshMobData();
	void UpdateExplosionData();
	//void UpdateSpawnData();
	void GetPlayerInput();
	void SendPlayerInput();
};