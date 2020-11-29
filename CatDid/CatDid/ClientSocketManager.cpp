#include "stdafx.h"
#include "ClientSocketManager.h"
#include <iostream>

DWORD WINAPI ClientMessageThread( void* param )
{
	ClientSocketManager* manager = (ClientSocketManager*)param;
	while ( true )
	{
		manager->RecvProc();
	}
}

ClientSocketManager* ClientSocketManager::GetInstance()
{
	static ClientSocketManager instance;
	return &instance;
}

void ClientSocketManager::InitalizeWinSock()
{
	if ( WSAStartup( MAKEWORD( 2, 2 ), &this->wsa ) != 0 )
	{
		err_msg_quit( "Init Fail" );
	}
	this->variables.Initialize();
}

void ClientSocketManager::CreateSocket()
{
	this->clientSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if ( this->clientSocket == INVALID_SOCKET )
	{
		err_quit( "socket()" );
	}
	BOOL optval = TRUE;
	setsockopt( this->clientSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof( optval ) );
}

void ClientSocketManager::SetConnection( const char* ipaddr )
{
	this->ipAddress = ipaddr;
	ZeroMemory( &this->serverAddr, sizeof( this->serverAddr ) );
	this->serverAddr.sin_family = AF_INET;
	this->serverAddr.sin_port = htons( SERVER_PORT );
	long addrLong = inet_addr( ipaddr );
	if ( addrLong == INADDR_NONE )
	{
		err_msg_quit( "IP ADDR ERROR" );
	}
	this->serverAddr.sin_addr.s_addr = inet_addr( ipaddr );

	int ret = connect( this->clientSocket, (SOCKADDR*)&this->serverAddr, sizeof( this->serverAddr ) );
	if ( ret == SOCKET_ERROR )
	{
		err_quit( "connect()" );
	}
}

void ClientSocketManager::StartRecvThread()
{
	this->msgThread = CreateThread( NULL, 0, ClientMessageThread, this, 0, NULL );
	if ( !this->msgThread )
	{
		err_quit( "CreateThread()" );
	}
}

void ClientSocketManager::Send( NetGameMessageType type )
{
	NetGameMessage message;
	message.type = type;
	message.parameterSize = GetMessageParameterSize( message.type );

	//send Message
	int ret = send( this->clientSocket, (char*)&message, sizeof( NetGameMessage ), 0 );
	if ( ret == SOCKET_ERROR ) err_display( "send()" );
}

void ClientSocketManager::Send( NetGameMessageType type, int count, void* parameter )
{
	NetGameMessage message;
	message.type = type;
	message.parameterSize = GetMessageParameterSize( message.type ) * count;

	//send Message
	int ret = send( this->clientSocket, (char*)&message, sizeof( NetGameMessage ), 0 );
	if ( ret == SOCKET_ERROR ) err_display( "send()" );
	if ( message.parameterSize != 0 && parameter != nullptr )
	{
		ret = send( this->clientSocket, (char*)parameter, message.parameterSize, 0 );
		if ( ret == SOCKET_ERROR ) err_display( "send()" );
	}
}

void ClientSocketManager::SendReqReadyMsg()
{
	this->Send( NetGameMessageType::MSG_REQ_READY );
}

void ClientSocketManager::SendCancelReadyMsg()
{
	this->Send( NetGameMessageType::MSG_CANCLE_READY );
}

void ClientSocketManager::SendPlayerInput( const PlayerInput& input )
{
	//std::cout << "Send Player Input : "
	//	<< std::boolalpha << input.isPressedMoveUp << ","
	//	<< std::boolalpha << input.isPressedMoveDown << ","
	//	<< std::boolalpha << input.isPressedMoveLeft << ","
	//	<< std::boolalpha << input.isPressedMoveRight
	//	<< std::endl;
	this->Send( NetGameMessageType::MSG_PLAYER_INPUT, 1, (void*)&input );
}

void ClientSocketManager::RecvProc()
{
	NetGameMessage messages;
	ZeroMemory( &messages, sizeof( NetGameMessage ) );
	int ret = ClientSocketManager::Recvn( this->clientSocket, (char*)&messages, sizeof( NetGameMessage ), 0 );
	if ( ret == SOCKET_ERROR ) err_quit( "recvn()" );
	std::cout << "Message Recv : " << GetMessageString( messages.type ) << " / paramSize : " << messages.parameterSize << std::endl;
	switch ( messages.type )
	{
	case MSG_MESSAGE_NULL:
	{
		//이상함 뭔가
	}
	break;
	case MSG_WAIT_ROOM_DATA:
	{
		this->variables.roomData.Use();
		this->variables.roomData.isChange = true;
		int ret = ClientSocketManager::Recvn( this->clientSocket, (char*)&this->variables.roomData.data, messages.parameterSize, 0 );
		if ( ret == SOCKET_ERROR ) err_quit( "recvn()" );
		std::cout << "Wait Room Data : " 
			<< (UINT)this->variables.roomData.data.playerWaitStates[0] << (UINT)this->variables.roomData.data.playerWaitStates[1]
			<< (UINT)this->variables.roomData.data.playerWaitStates[2] << (UINT)this->variables.roomData.data.playerWaitStates[3]
			<< std::endl;
		this->variables.roomData.End();
	}
	break;
	case MSG_GAME_ALL_READY:
	{
		this->variables.recvAllReady.Use();
		this->variables.recvAllReady.isChange = true;
		this->variables.recvAllReady.data = true;
		this->variables.recvAllReady.End();
	}
	break;
	case MSG_REQ_READY:
	{
		//서버용
	}
	break;
	case MSG_CANCLE_READY:
	{
		//서버용
	}
	break;
	case MSG_GAME_START:
	{
		this->variables.recvGameStart.Use();
		this->variables.recvGameStart.isChange = true;
		this->variables.recvGameStart.data = true;
		this->variables.recvGameStart.End();
	}
	break;
	case MSG_GAME_END:
	{
		this->variables.recvGameEnd.Use();
		this->variables.recvGameEnd.isChange = true;
		this->variables.recvGameEnd.data = true;
		this->variables.recvGameEnd.End();
	}
	break;
	case MSG_SCENE_DATA:
	{
		this->variables.sceneData.Use();
		this->variables.sceneData.isChange = true;
		int ret = ClientSocketManager::Recvn( this->clientSocket, (char*)&this->variables.sceneData.data, messages.parameterSize, 0 );
		std::cout << "Scene Data : "
			<< this->variables.sceneData.data.playerState[0].positionX << ", " << this->variables.sceneData.data.playerState[0].positionY
			<< std::endl;
		if ( ret == SOCKET_ERROR ) err_quit( "recvn()" );
		this->variables.sceneData.End();
	}
	break;
	case MSG_BULLET_DATA:
	{
		this->variables.bulletData.Use();
		this->variables.bulletData.isChange = true;
		UINT count = GetMessageParameterCount( messages );
		this->variables.bulletData.data.resize( count );
		int ret = ClientSocketManager::Recvn( this->clientSocket, (char*)this->variables.bulletData.data.data(), messages.parameterSize, 0 );
		if ( ret == SOCKET_ERROR ) err_quit( "recvn()" );
		this->variables.bulletData.End();
	}
	break;
	case MSG_MOB_DATA:
	{
		this->variables.mobData.Use();
		this->variables.mobData.isChange = true;
		UINT count = GetMessageParameterCount( messages );
		this->variables.mobData.data.resize( count );
		int ret = ClientSocketManager::Recvn( this->clientSocket, (char*)this->variables.mobData.data.data(), messages.parameterSize, 0 );
		if ( ret == SOCKET_ERROR ) err_quit( "recvn()" );
		this->variables.mobData.End();
	}
	break;
	case MSG_EVENT_EXPLOSION:
	{
		this->variables.expData.Use();
		this->variables.expData.isChange = true;
		UINT count = GetMessageParameterCount( messages );
		this->variables.expData.data.resize( count );
		int ret = ClientSocketManager::Recvn( this->clientSocket, (char*)this->variables.expData.data.data(), messages.parameterSize, 0 );
		if ( ret == SOCKET_ERROR ) err_quit( "recvn()" );
		this->variables.expData.End();
	}
	break;
	case MSG_EVENT_SPAWN:
	{
		this->variables.spawnData.Use();
		this->variables.spawnData.isChange = true;
		UINT count = GetMessageParameterCount( messages );
		this->variables.spawnData.data.resize( count );
		int ret = ClientSocketManager::Recvn( this->clientSocket, (char*)this->variables.spawnData.data.data(), messages.parameterSize, 0 );
		if ( ret == SOCKET_ERROR ) err_quit( "recvn()" );
		this->variables.spawnData.End();
	}break;
	case MSG_PLAYER_INPUT:
	{
		//서버용
	}
	break;
	}


}

int ClientSocketManager::Recvn( SOCKET socket, char* buf, int len, int flags )
{
	int received;
	char* ptr = buf;
	int left = len;

	while ( left > 0 )
	{
		received = recv( socket, ptr, left, flags );
		if ( received == SOCKET_ERROR ) return received;
		else if ( received == 0 ) break;

		left -= received;
		ptr += received;
	}
	return (len - left);
}

void ClientSocketManager::OnRecvWaitData()
{
	this->variables.roomData.Use();
	if ( this->variables.roomData.isChange == true )
	{
		this->variables.roomData.isChange = false;
		this->recvWaitDataFunction( this->variables.roomData.data );
	}
	this->variables.roomData.End();
}

void ClientSocketManager::OnRecvAllReady()
{
	this->variables.recvAllReady.Use();
	if ( this->variables.recvAllReady.isChange == true )
	{
		this->variables.recvAllReady.isChange = false;
		this->variables.recvAllReady.data = false;
		this->recvAllReadyFunction();
	}
	this->variables.recvAllReady.End();
}

void ClientSocketManager::OnRecvGameStart()
{
	this->variables.recvGameStart.Use();
	if ( this->variables.recvGameStart.isChange == true )
	{
		this->variables.recvGameStart.isChange = false;
		this->recvGameStartFunction();
	}
	this->variables.recvGameStart.End();
}

void ClientSocketManager::OnRecvSceneData()
{
	this->variables.sceneData.Use();
	if ( this->variables.sceneData.isChange == true )
	{
		this->variables.sceneData.isChange = false;
		this->recvSceneDataFunction( this->variables.sceneData.data );
	}
	this->variables.sceneData.End();
}

void ClientSocketManager::OnRecvBulletData()
{
	this->variables.bulletData.Use();
	if ( this->variables.bulletData.isChange == true )
	{
		this->variables.bulletData.isChange = false;
		this->recvBulletDataFunction( this->variables.bulletData.data );
	}
	this->variables.bulletData.End();
}

void ClientSocketManager::OnRecvMobData()
{
	this->variables.mobData.Use();
	if ( this->variables.mobData.isChange == true )
	{
		this->variables.mobData.isChange = false;
		this->recvMobDataFunction( this->variables.mobData.data );
	}
	this->variables.mobData.End();
}

void ClientSocketManager::OnRecvGameEnd()
{
	this->variables.recvGameEnd.Use();
	if ( this->variables.recvGameEnd.isChange == true )
	{
		this->variables.recvGameEnd.isChange = false;
		this->variables.recvGameEnd.data = false;
		this->recvGameEndFunction();
	}
	this->variables.recvGameEnd.End();
}

void ClientSocketManager::OnRecvEventExplosion()
{
	this->variables.expData.Use();
	if ( this->variables.expData.isChange == true )
	{
		this->variables.expData.isChange = false;
		this->recvEventExplosionFunction( this->variables.expData.data );
	}
	this->variables.expData.End();
}

void ClientSocketManager::OnRecvEventSpawn()
{
	this->variables.spawnData.Use();
	if ( this->variables.spawnData.isChange == true )
	{
		this->variables.spawnData.isChange = false;
		this->recvEventSpawnFunction( this->variables.spawnData.data );
	}
	this->variables.spawnData.End();
}

void ClientSocketManager::CloseConnection()
{
	closesocket( this->clientSocket );
}

void ClientSocketManager::Destroy()
{
	this->variables.Delete();
	WSACleanup();
}

void err_quit( const char* msg )
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );

	MessageBox( NULL, (LPCTSTR)lpMsgBuf, L"", MB_ICONERROR );
	LocalFree( lpMsgBuf );
	assert( false );
}

void err_msg_quit( const char* msg )
{
	std::string buf( msg );
	std::wstring buff( buf.begin(), buf.end() );
	MessageBox( NULL, buff.c_str(), L"", MB_ICONERROR );
	assert( false );
}


void err_display( const char* msg )
{
	err_quit( msg );
}
