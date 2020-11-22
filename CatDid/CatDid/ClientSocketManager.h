#pragma once
#include "LowLevelData.h"
#include <vector>
#include <queue>
#include <string>
#include <functional>

#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#include <winsock2.h>
#include <utility>

extern DWORD WINAPI ClientMessageThread( void* param );
extern void err_quit( const char* msg );
extern void err_msg_quit( const char* msg );
extern void err_display( const char* msg );

template<class Ty>
struct NetworkGameVariable
{
	CRITICAL_SECTION cs;
	bool isChange = false;
	Ty data;

	inline void InitializeCriticalSection()
	{
		::InitializeCriticalSection( &cs );
	}

	inline void DeleteCriticalSection()
	{
		::DeleteCriticalSection( &cs );
	}

	inline void Use()
	{
		EnterCriticalSection( &cs );
	}

	inline void End()
	{
		LeaveCriticalSection( &cs );
	}
};


using BulletDatas = std::vector<BulletData>;
using MobDatas = std::vector<MobData>;
using EventDatas = std::vector<EventParameter>;
struct NetworkVariables
{
	//recv
	NetworkGameVariable<WaitRoomData> roomData;
	NetworkGameVariable<GameSceneData> sceneData;
	NetworkGameVariable<BulletDatas> bulletData;
	NetworkGameVariable<MobDatas> mobData;
	NetworkGameVariable<EventDatas> expData;
	NetworkGameVariable<EventDatas> spawnData;
	NetworkGameVariable<bool> recvAllReady;
	NetworkGameVariable<bool> recvGameStart;
	NetworkGameVariable<bool> recvGameEnd;

	void Initialize()
	{
		roomData.InitializeCriticalSection();
		sceneData.InitializeCriticalSection();
		bulletData.InitializeCriticalSection();
		mobData.InitializeCriticalSection();
		expData.InitializeCriticalSection();
		spawnData.InitializeCriticalSection();
		recvAllReady.InitializeCriticalSection();
		recvGameStart.InitializeCriticalSection();
		recvGameEnd.InitializeCriticalSection();
	}

	void Delete()
	{
		roomData.DeleteCriticalSection();
		sceneData.DeleteCriticalSection();
		bulletData.DeleteCriticalSection();
		mobData.DeleteCriticalSection();
		expData.DeleteCriticalSection();
		spawnData.DeleteCriticalSection();
		recvAllReady.DeleteCriticalSection();
		recvGameStart.DeleteCriticalSection();
		recvGameEnd.DeleteCriticalSection();
	}
};

class ClientSocketManager
{
	friend DWORD WINAPI ClientMessageThread( void* param );
	std::string ipAddress;

	WSADATA wsa;
	SOCKET clientSocket;

	SOCKADDR_IN serverAddr;
	HANDLE msgThread;

	NetworkVariables variables;

	ClientSocketManager() = default;
public:
	static ClientSocketManager* GetInstance();

	void InitalizeWinSock();
	void CreateSocket();
	void SetConnection( const char* ipaddr );
	void StartRecvThread(); 

	void Send( NetGameMessageType type);
	void Send( NetGameMessageType type, int count, void* parameter );

	void SendReqReadyMsg();
	void SendCancelReadyMsg();
	void SendPlayerInput( const PlayerInput& input );

	void RecvProc();
	static int Recvn( SOCKET socket, char* buf, int len, int flags );

	void OnRecvWaitData();
	void OnRecvAllReady();
	void OnRecvGameStart();
	void OnRecvSceneData();
	void OnRecvBulletData();
	void OnRecvMobData();
	void OnRecvGameEnd();
	void OnRecvEventExplosion();
	void OnRecvEventSpawn();

	void CloseConnection();
	void Destroy();

	std::function<void( const WaitRoomData& )> recvWaitDataFunction = nullptr;
	std::function<void()> recvAllReadyFunction = nullptr;
	std::function<void()> recvGameStartFunction = nullptr;

	std::function<void( const GameSceneData& )> recvSceneDataFunction = nullptr;
	std::function<void( const std::vector<BulletData>& )> recvBulletDataFunction = nullptr;
	std::function<void( const std::vector<MobData>& )> recvMobDataFunction = nullptr;

	std::function<void()> recvGameEndFunction = nullptr;
	std::function<void( const std::vector<EventParameter>& )> recvEventExplosionFunction = nullptr;
	std::function<void( const std::vector<EventParameter>& )> recvEventSpawnFunction = nullptr;


};