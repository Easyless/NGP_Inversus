#include "stdafx.h"
#include "InversusNetworkController.h"
#include "InversusFramework.h"
#include "InversusClasses.h"
#include "InversusMenu.h"

void InversusNetworkController::InitlizeWithSocket( ClientSocketManager* socket )
{
	this->socket = socket;
	this->socket->recvGameStartFunction =
		[this]()
	{
		this->startGame = true;
	};

	this->socket->recvSceneDataFunction =
		[this]( const GameSceneData& sceneData )
	{
		this->sceneData = sceneData;
	};
	this->socket->recvMobDataFunction =
		[this]( const MobDatas& datas )
	{
		this->mobData = datas;
	};
	this->socket->recvBulletDataFunction =
		[this]( const BulletDatas& datas )
	{
		this->bulletData = datas;
	};
	this->socket->recvGameEndFunction =
		[this]()
	{
		this->socket->CloseConnection();
		this->framework->menu->Active();
	};
	this->socket->recvEventExplosionFunction =
		[this]( const EventDatas& datas )
	{
		this->explosionDatas = datas;
	};
	this->socket->recvEventSpawnFunction =
		[this]( const EventDatas& datas )
	{
		this->spawnDatas = datas;
	};
}

void InversusNetworkController::Reset()
{
}

void InversusNetworkController::Start()
{

}

void InversusNetworkController::Update( float deltaTime )
{
	this->RecvData();
	if ( this->startGame )
	{
		this->RefreshPlayerData();
		this->RefreshBulletData();
		this->RefreshMobData();
		this->RefreshMapData();
		this->UpdateExplosionData();
		this->UpdateSpawnData();
		this->GetPlayerInput();
		this->SendPlayerInput(deltaTime);
	}
}

void InversusNetworkController::RecvData()
{
	this->socket->OnRecvGameStart();
	this->socket->OnRecvSceneData();
	this->socket->OnRecvBulletData();
	this->socket->OnRecvMobData();
	this->socket->OnRecvGameEnd();
	this->socket->OnRecvEventExplosion();
	this->socket->OnRecvEventSpawn();
}

void InversusNetworkController::RefreshPlayerData()
{
	this->framework->container->RefreshPlayersFromData( this->sceneData );
}

static Vec2DF GetShootDirection( PlayerShootType type )
{
	switch ( type )
	{
	case None:
		return Vec2DF( 0, 0 );
	case ShootUp:
		return Vec2DF::Up();
	case ShootDown:
		return Vec2DF::Down();
	case ShootLeft:
		return Vec2DF::Left();
	case ShootRight:
		return Vec2DF::Right();
	}
}

void InversusNetworkController::RefreshBulletData()
{
	//for ( auto& b : this->bulletData )
	//{
	//	this->framework->container->AddBullet( Vec2DF( b.positionX, b.positionY ), GetShootDirection( b.shootDirection ) );
	//}
	this->framework->container->RefreshBulletFromData( this->bulletData );
}

void InversusNetworkController::RefreshMapData()
{
	this->framework->container->RefreshMapFromData( this->sceneData );
}

void InversusNetworkController::RefreshMobData()
{
	this->framework->container->RefreshEnemyFromData( this->mobData );
}

void InversusNetworkController::UpdateExplosionData()
{
	for ( auto& e : this->explosionDatas )
	{
		this->framework->container->AddExplosion(
			Vec2DF( e.positionX, e.positionY ),
			RGB( 0, 0, 0 ),
			false );
	}
	this->explosionDatas.clear();
}

inline COLORREF GetColor( EventOwnerType owner )
{
	switch ( owner )
	{
	case PLAYER0: return RGB( 169, 14, 21 );
	case PLAYER1: return RGB( 0, 183, 0 );
	case PLAYER2: return RGB( 0, 75, 151 );
	case PLAYER3: return RGB( 0, 0, 0 );
	case NormalMob: return RGB( 56, 182, 214 );
	case SpecialMob: return RGB( 214, 56, 56 );
	}
}

void InversusNetworkController::UpdateSpawnData()
{
	for ( auto& e : this->spawnDatas )
	{
		this->framework->container->AddSpawnEffect(
			Vec2DF( e.positionX, e.positionY ),
			GetColor(e.owner) );
	}
	this->spawnDatas.clear();
}

void InversusNetworkController::GetPlayerInput()
{
	input.isPressedMoveUp = GetAsyncKeyState( 'W' ) ? true : false;
	input.isPressedMoveLeft = GetAsyncKeyState( 'A' ) ? true : false;
	input.isPressedMoveRight = GetAsyncKeyState( 'D' ) ? true : false;
	input.isPressedMoveDown = GetAsyncKeyState( 'S' ) ? true : false;

	if ( input.shootInput == PlayerShootType::ShootUp && GetAsyncKeyState( VK_UP ) ||
		input.shootInput == PlayerShootType::ShootDown && GetAsyncKeyState( VK_DOWN ) ||
		input.shootInput == PlayerShootType::ShootLeft && GetAsyncKeyState( VK_LEFT ) ||
		input.shootInput == PlayerShootType::ShootRight && GetAsyncKeyState( VK_RIGHT )
		)
	{
		return;
	}
	else if ( GetAsyncKeyState( VK_UP ) )
	{
		input.shootInput = PlayerShootType::ShootUp;
	}
	else if ( GetAsyncKeyState( VK_LEFT ) )
	{
		input.shootInput = PlayerShootType::ShootLeft;
	}
	else if ( GetAsyncKeyState( VK_DOWN ) )
	{
		input.shootInput = PlayerShootType::ShootDown;
	}
	else if ( GetAsyncKeyState( VK_RIGHT ) )
	{
		input.shootInput = PlayerShootType::ShootRight;
	}
	else
	{
		input.shootInput = PlayerShootType::None;
	}


}

void InversusNetworkController::SendPlayerInput( float deltaTime )
{
	static float count = 0;
	count += deltaTime;
	if ( count >= 0.01666f )
	{
		count = 0;
		this->socket->SendPlayerInput( input );
	}
}
