#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include "LowLevelData.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winmm.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����

#define SERVERPORT 15073
#define FPS 30.f
#define BULLET_SPEED 2.f
#define PLAYER_SPEED 2.f

struct Param {
	SOCKET client_sock;
	int threadnum;
};

CRITICAL_SECTION cs;

// �̺�Ʈ�� ������ ����ȭ
// ���濡���� ������Ʈ x?

// ���� ������
// ���� �ִ� 4��
bool isConnect[MAX_PLAYER_LENGTH] = { false, false, false, false };
bool isPlay = false;
bool isStart = false;


bool isSend[MAX_PLAYER_LENGTH] = { false, };
bool isCharging[MAX_PLAYER_LENGTH] = { false, };
PlayerShootType shootDir[MAX_PLAYER_LENGTH] = { None, };
float chargingTime[MAX_PLAYER_LENGTH] = { 0, };

SOCKET connectedSocket[4] = { NULL, NULL, NULL, NULL };
WaitRoomData waitRoomData;
GameSceneData gameSceneData;
std::vector<BulletData> bulletDatas;
std::vector<MobData> mobDatas;
EventParameter eventParameter;
PlayerInput playerInput[MAX_PLAYER_LENGTH];
int connectedCount = 0;

void InitSceneData() {
	gameSceneData.leftLifeCount = 3;
	gameSceneData.mapData = { false, };
	for (size_t i = 0; i < 4; i++)
	{
		gameSceneData.playerState[i].positionX = i * 50;
		gameSceneData.playerState[i].positionY = i * 30;
		gameSceneData.playerState[i].isDead = true;
	}
}

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}
	return (len - left);
}


void SendtoAll(NetGameMessage sendmessage) {
	for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
	{
		if (isConnect[i]) {
			send(connectedSocket[i], (char*)&sendmessage, sizeof(NetGameMessage), 0);
			switch (sendmessage.type) {
			case MSG_WAIT_ROOM_DATA:
				send(connectedSocket[i], (char*)&waitRoomData, sizeof(WaitRoomData), 0);
				std::cout << "sendtoall waitroomdata" << std::endl;
				break;
			case MSG_BULLET_DATA:
				send(connectedSocket[i], (char*)bulletDatas.data(), sizeof(BulletData) * bulletDatas.size(), 0);
				std::cout << "sendtoall bulletdatas" << std::endl;
				break;
			default:
				break;
			}
		}
	}
}


DWORD WINAPI CommunicationThreadFunc(LPVOID arg) {
	int retval;

	Param* p = (Param*)arg;
	SOCKET client_sock = p->client_sock;
	int threadnum = p->threadnum;

	SOCKADDR_IN clientaddr;
	int size;
	int addrlen;

	NetGameMessage receivemessage;
	NetGameMessage sendmessage = { MSG_WAIT_ROOM_DATA, GetMessageParameterSize(MSG_WAIT_ROOM_DATA) };

	UINT datasize = 0;
	SendtoAll(sendmessage);

	int readyCount = 0;

	while (true) {
		if (!isPlay || (isPlay && isSend)) {
			isSend[threadnum] = false; // ������Ʈ ���
			// ������ �۽� - �޼��� Ÿ�� �� ������
			if (isPlay) {
				sendmessage.type = MSG_SCENE_DATA;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type);
			}

			retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);

			std::cout << "sendmessage type: " << sendmessage.type << std::endl;

			// ������ �۽� - �޼����� �ش��ϴ� ������ �۽�
			switch (sendmessage.type) {
			case MSG_WAIT_ROOM_DATA:
				retval = send(client_sock, (char*)&waitRoomData, sizeof(WaitRoomData), 0);
				std::cout << "send room data" << std::endl;
				break;
			case MSG_GAME_START:
				break;
			case MSG_SCENE_DATA:
				retval = send(client_sock, (char*)&gameSceneData, sizeof(GameSceneData), 0);
				std::cout << "send scene data" << std::endl;
				sendmessage.type = MSG_BULLET_DATA;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type) * bulletDatas.size();
				retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);
				retval = send(client_sock, (char*)bulletDatas.data(), sizeof(BulletData) * bulletDatas.size(), 0);
				//SendtoAll(sendmessage);
				break;
			case MSG_BULLET_DATA:
				break;
			case MSG_MOB_DATA:
				break;
			default:
				break;
			}

			// ������ ���� - �޼��� Ÿ�� �� ������
			retval = recvn(client_sock, (char*)&receivemessage, sizeof(NetGameMessage), 0);
			if (retval == SOCKET_ERROR) {
				//Disconnect(client_sock, threadnum, sendmessage)
				err_display("recvn, ready message");
				isConnect[threadnum] = false;
				connectedCount--;
				waitRoomData.playerWaitStates[threadnum] = WAIT_NOT_CONNECTED;
				sendmessage.type = MSG_WAIT_ROOM_DATA;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type);
				SendtoAll(sendmessage);
				closesocket(client_sock);
				std::cout << "end communication thread" << std::endl;
				return 0;
			}

			std::cout << "recvmessage type: " << receivemessage.type << std::endl;

			datasize = GetMessageParameterSize(receivemessage.type);

			// ������ ���� - �޼��� ������ ���� �ļ� ����
			switch (receivemessage.type)
			{
				// ��� �� ���� ���� �޽���
				// �޽��� ���� �� ���� �޽��� ���� ����(��� �� ����)
			case MSG_REQ_READY:
				retval = recvn(client_sock, (char*)&receivemessage, datasize, 0);

				waitRoomData.playerWaitStates[threadnum] = WAIT_READY;
				std::cout << "recv ready msg" << std::endl;
				readyCount = 0;
				for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
				{
					if (waitRoomData.playerWaitStates[i] == WAIT_READY) {
						readyCount++;
					}
				}
				if (readyCount == connectedCount) {
					sendmessage.type = MSG_GAME_ALL_READY;
					isPlay = true;
					isStart = true;
				}
				else {
					sendmessage.type = MSG_WAIT_ROOM_DATA;
				}
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type);
				SendtoAll(sendmessage);
				break;

			case MSG_CANCLE_READY:
				retval = recvn(client_sock, (char*)&receivemessage, datasize, 0);
				// ���� ��� ó��
				std::cout << "recv ready cancle msg" << std::endl;
				readyCount--;
				waitRoomData.playerWaitStates[threadnum] = WAIT_CONNECTED_NORMAL;
				sendmessage.type = MSG_WAIT_ROOM_DATA;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type);
				SendtoAll(sendmessage);
				break;

				// ���� �� ���� ���� �޽���
			case NetGameMessageType::MSG_PLAYER_INPUT:
				retval = recvn(client_sock, (char*)&playerInput[threadnum], datasize, 0);
				// ���� ��ǲ ���� ����, ������Ʈ�� ����
				if (playerInput[threadnum].shootInput == PlayerShootType::None && isCharging[threadnum] &&
					gameSceneData.playerState[threadnum].remainBullet > 0) { // �Ѿ� ����
					// �����ð� if
					BulletData* b = new BulletData;
					b->ownerPlayer = threadnum;
					b->positionX = gameSceneData.playerState[threadnum].positionX;
					b->positionY = gameSceneData.playerState[threadnum].positionY;
					b->shootDirection = shootDir[threadnum];
					bulletDatas.push_back(*b);
					isCharging[threadnum] = false;
					gameSceneData.playerState[threadnum].remainBullet -= 1;
				}
				else if (playerInput[threadnum].shootInput != PlayerShootType::None && !isCharging[threadnum]) {
					isCharging[threadnum] = true;
					shootDir[threadnum] = playerInput[threadnum].shootInput;
				}
				std::cout << "player upinput: " << std::boolalpha << playerInput[threadnum].isPressedMoveUp << std::endl;
				std::cout << "player downinput: " << std::boolalpha << playerInput[threadnum].isPressedMoveDown << std::endl;
				std::cout << "player leftinput: " << std::boolalpha << playerInput[threadnum].isPressedMoveLeft << std::endl;
				std::cout << "player rightinput: " << std::boolalpha << playerInput[threadnum].isPressedMoveRight << std::endl;
				break;

			default:
				break;

			}
			if (retval == SOCKET_ERROR) {
				err_display("recvn, ready message");
				isConnect[threadnum] = false;
				connectedCount--;
				waitRoomData.playerWaitStates[threadnum] = WAIT_NOT_CONNECTED;
				sendmessage.type = MSG_WAIT_ROOM_DATA;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type);
				SendtoAll(sendmessage);
				closesocket(client_sock);
				std::cout << "end communication thread" << std::endl;
				return 0;
			}
		}

	}


	isConnect[threadnum] = false;
	connectedCount--;
	waitRoomData.playerWaitStates[threadnum] = WAIT_NOT_CONNECTED;
	sendmessage.type = MSG_WAIT_ROOM_DATA;
	sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type);
	SendtoAll(sendmessage);

	closesocket(client_sock);
	std::cout << "end communication thread" << std::endl;
	return 0;
}

DWORD WINAPI UpdateThreadFunc(LPVOID arg) {
	DWORD lastTime = timeGetTime();
	DWORD currTime;

	float delta = 0;
	float x, y;
	float mobTimer = 0;
	float addBulletTimer[4] = { 0, };

	while (true) {
		if (isPlay) {
			if (isStart) { // ���� 2�� �ĺ��� ������Ʈ 
				InitSceneData();
				Sleep(2000);
				NetGameMessage sendmessage;
				sendmessage.type = MSG_GAME_START;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type);
				SendtoAll(sendmessage);
				isStart = false;
				for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
				{
					if (isConnect[i]) {
						gameSceneData.playerState[i].isDead = false;
					}
				}
			}

			//Deltatime
			currTime = timeGetTime();
			delta = (currTime - lastTime) * 0.001f;

			if (delta >= 1.f / FPS) {

				// �÷��̾�
				// move
				for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
				{
					if (isConnect[i]) {
						x = gameSceneData.playerState[i].positionX;
						y = gameSceneData.playerState[i].positionY;

						if (playerInput[i].isPressedMoveUp && y > PLAYER_SPEED) {
							y -= PLAYER_SPEED;
						}
						if (playerInput[i].isPressedMoveDown && y < MAP_SIZE_Y - PLAYER_SPEED) {
							y += PLAYER_SPEED;
						}
						if (playerInput[i].isPressedMoveLeft && x > PLAYER_SPEED) {
							x -= PLAYER_SPEED;
						}
						if (playerInput[i].isPressedMoveRight && x < MAP_SIZE_X - PLAYER_SPEED) {
							x += PLAYER_SPEED;
						}

						gameSceneData.playerState[i].positionX = x;
						gameSceneData.playerState[i].positionY = y;

						if (isCharging[i]) {
							chargingTime[i] += delta;
						}
						else {
							chargingTime[i] = 0;
						}

						if (gameSceneData.playerState[i].remainBullet < 6) {
							addBulletTimer[i] += delta;
							if (addBulletTimer[i] > 1) {
								gameSceneData.playerState[i].remainBullet += 1;
								addBulletTimer[i] = 0;
								std::cout << "add bullet" << std::endl;
							}
						}

					}

				
				}

				//	�Ѿ�
				//	�޽���
				//	�浹 - �� : �� ���� �� �Ѿ� ����
				//	- �� : �� ���� ���� �� �Ѿ� ����

				if (!bulletDatas.empty()) {

					for (size_t i = 0; i < bulletDatas.size(); i++)
					{
						// �Ѿ� ���� ��ǲ����
						// ���� ����� �Ѿ� ����
						x = bulletDatas.at(i).positionX;
						y = bulletDatas.at(i).positionY;
						switch (bulletDatas.at(i).shootDirection)
						{
						case PlayerShootType::ShootUp:
							y -= BULLET_SPEED;
							break;
						case PlayerShootType::ShootDown:
							y += BULLET_SPEED;
							break;
						case PlayerShootType::ShootLeft:
							x -= BULLET_SPEED;
							break;
						case PlayerShootType::ShootRight:
							x += BULLET_SPEED;
							break;
						default:
							break;
						}
						bulletDatas.at(i).positionX = x;
						bulletDatas.at(i).positionY = y;


						if (bulletDatas.at(i).positionX < 0 || bulletDatas.at(i).positionX > MAP_SIZE_X ||
							bulletDatas.at(i).positionY < 0 || bulletDatas.at(i).positionY > MAP_SIZE_Y) {
							bulletDatas.erase(bulletDatas.begin() + i);
							break;
						}

					}
					for (auto& b : bulletDatas)
					{
						// �Ѿ� ���� ��ǲ����
						// ���� ����� �Ѿ� ����
						x = b.positionX;
						y = b.positionY;
						switch (b.shootDirection)
						{
						case PlayerShootType::ShootUp:
							y -= BULLET_SPEED;
							break;
						case PlayerShootType::ShootDown:
							y += BULLET_SPEED;
							break;
						case PlayerShootType::ShootLeft:
							x -= BULLET_SPEED;
							break;
						case PlayerShootType::ShootRight:
							x += BULLET_SPEED;
							break;
						default:
							break;
						}
						b.positionX = x;
						b.positionY = y;

					}
				}


				//BLOCK_SIZE_X
				//BLOCK_SIZE_Y
				//BULLET_SIZE_X
				//BULLET_SIZE_Y
				//PLAYER_SIZE_X
				//PLAYER_SIZE_Y

				//	��
				//	�޽���
				//	�� ��ġ�� �� ����
				//	�浹 - �÷��̾� : �÷��̾� ���, ��� ī��Ʈ ����
				//	- �Ѿ� : �� ���� �� ���� �̺�Ʈ

				//	�̵� - �� �÷��̾� ������ �Ÿ� ��� �� ����� �÷��̾� ��ġ��



				//// mob
				//for (auto m : mobDatas) {
				//	// ������ / ������ ��ġ �� true�� ����
				//	x = m.positionX / BLOCK_SIZE_X;
				//	y = m.positionY / BLOCK_SIZE_Y;

				//	// �̵� - ����� �÷��̾�� �̵�

				//	// �浹 
				//	// ��, �÷��̾� �浹 - �÷��̾� ������ ����
				//	// ��, �Ѿ� �浹 - �� ���� ó��

				//}

				lastTime = currTime;
				for (size_t i = 0; i < 4; i++)
				{
					isSend[i] = true; // ������Ʈ �� �޽��� ����
				}

			}
		}

		if (connectedCount == 0) {
			isPlay = false;
			bulletDatas.clear();
			mobDatas.clear();
		}
	}
	return 0;
}



int main(int argc, char* argv[]) {

	int retval;
	InitializeCriticalSection(&cs);

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");


	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	BOOL optval = TRUE; // Nagle �˰��� ����
	setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY,
		(char*)&optval, sizeof(optval));

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	// ������ �ڵ�
	HANDLE hThread;

	// �����忡 �ѱ� ����
	Param arg;


	// UpdateThread ����
	hThread = CreateThread(NULL, 0, UpdateThreadFunc,
		(LPVOID)&arg, 0, NULL);

	int threadnum = -1;
	bool success = false;
	while (1) {

		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		success = false;
		for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
		{
			if (!isConnect[i]) {
				threadnum = i;
				isConnect[i] = true;
				waitRoomData.playerWaitStates[i] = WAIT_CONNECTED_NORMAL;
				connectedCount++;
				success = true;
				break;
			}
		}
		for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
		{
			std::cout << isConnect[i] << " ";
		}

		std::cout << std::endl;
		// Ŭ���̾�Ʈ �������� CommunicationThread ����
		if (success && !isPlay) {
			arg = { client_sock, threadnum };
			connectedSocket[threadnum] = client_sock;
			hThread = CreateThread(NULL, 0, CommunicationThreadFunc,
				(LPVOID)&arg, 0, NULL);
			if (hThread == NULL) { closesocket(client_sock); }
			else {
				CloseHandle(hThread);
			}
		}
	}

	closesocket(listen_sock);

	DeleteCriticalSection(&cs);

	WSACleanup();
	return 0;

}
