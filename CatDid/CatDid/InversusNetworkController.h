#pragma once
#pragma once
#include <list>
#include <memory>
#include "GameObject.h"

#include "ClientSocketManager.h"
#include "Map.h"
#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "Explosion.h"
#include "DropBullet.h"


struct InversusNetworkController
{
	InversusNetworkController( InversusFramework* framework ) :framework( framework ) {}
	InversusFramework* framework = nullptr;
	ClientSocketManager* socket = nullptr;
	GameSceneData sceneData;
	MobDatas mobData;
	BulletDatas bulletData;
	PlayerInput input;
	bool startGame = false;
	void InitlizeWithSocket( ClientSocketManager* socket );
	void Reset();
	void Start();
	void Update( float deltaTime );
	void RecvData();
	void RefreshPlayerData();
	void RefreshMapData();
	void GetPlayerInput();
	void SendPlayerInput();
};