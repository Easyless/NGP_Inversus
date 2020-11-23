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

void InversusNetworkController::RefreshMapData()
{
	this->framework->container->BlockMap.RefreshFromData(this->sceneData);
}

void InversusNetworkController::GetPlayerInput()
{
	input.isPressedMoveUp = GetAsyncKeyState( 'W' ) ? true : false;
	input.isPressedMoveLeft = GetAsyncKeyState( 'A' ) ? true : false;
	input.isPressedMoveRight = GetAsyncKeyState( 'D' ) ? true : false;
	input.isPressedMoveDown = GetAsyncKeyState( 'S' ) ? true : false;

	//if ( GetAsyncKeyState( VK_UP ) )
	//{
	//}
	//else if ( GetAsyncKeyState( VK_LEFT ) )
	//{
	//}
	//else if ( GetAsyncKeyState( VK_DOWN ) )
	//{
	//}
	//else if ( GetAsyncKeyState( VK_RIGHT ) )
	//{
	//}
}

void InversusNetworkController::SendPlayerInput()
{
	this->socket->SendPlayerInput( input );
}
