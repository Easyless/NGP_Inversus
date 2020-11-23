#include "stdafx.h"
#include "InversusNetworkController.h"
#include "InversusFramework.h"
#include "InversusClasses.h"

void InversusNetworkController::InitlizeWithSocket( ClientSocketManager* socket )
{
	this->socket = socket;
	this->socket->recvGameStartFunction =
		[this]()
	{
		this->startGame = true;
		this->framework->container->BlockMap.Reset();
		for ( auto& p : this->framework->container->player )
		{
			p.Active();
		}
	};
	
	this->socket->recvSceneDataFunction =
		[this](const GameSceneData& sceneData)
	{
		this->sceneData = sceneData;
	};
	this->socket->recvMobDataFunction =
		[this]( const MobDatas& datas )
	{
	};
	this->socket->recvBulletDataFunction =
		[this]( const BulletDatas& datas )
	{
		this->bulletData = datas;
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
		this->RefreshMapData();
		this->GetPlayerInput();
		this->SendPlayerInput();
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
	for ( size_t i = 0; i < 4; i++ )
	{
		this->framework->container->player[i].RefreshFromData( this->sceneData, i );
	}
}

Vec2DF GetShootDirection( PlayerShootType type )
{
	switch ( type )
	{
	case None:
		return Vec2DF(0,0);
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
	for ( auto& b : this->bulletData )
	{
		this->framework->container->AddBullet( Vec2DF( b.positionX, b.positionY ), GetShootDirection( b.shootDirection ) );
	}
	this->bulletData.clear();
}

void InversusNetworkController::RefreshMapData()
{
	this->framework->container->BlockMap.RefreshFromData(this->sceneData);
}

void InversusNetworkController::RefreshMobData()
{
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

void InversusNetworkController::SendPlayerInput()
{
	this->socket->SendPlayerInput( input );
}
