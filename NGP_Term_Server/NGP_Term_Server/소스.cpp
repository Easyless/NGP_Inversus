#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
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
			default:
				break;
			}
		}
	}
}

bool Collision(float px1, float px2, float py1, float py2, float s1, float s2) {
	if ((px1 - s1) > (px2 + s2)) {
		return false;
	}
	if ((py1 - s1) > (py2 + s2)) {
		return false;
	}
	if ((px2 - s2) > (px1 + s1)) {
		return false;
	}
	if ((py2 - s2) > (py1 + s1)) {
		return false;
	}

	return true;
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

			//std::cout << "sendmessage type: " << sendmessage.type << std::endl;

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
				//std::cout << "send scene data" << std::endl;
				sendmessage.type = MSG_BULLET_DATA;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type) * bulletDatas.size();
				retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);
				retval = send(client_sock, (char*)bulletDatas.data(), sizeof(BulletData) * bulletDatas.size(), 0);
				sendmessage.type = MSG_MOB_DATA;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type) * mobDatas.size();
				retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);
				retval = send(client_sock, (char*)mobDatas.data(), sizeof(MobData) * mobDatas.size(), 0);
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

			//std::cout << "recvmessage type: " << receivemessage.type << std::endl;

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
				else if (playerInput[threadnum].shootInput != PlayerShootType::None && !isCharging[threadnum] &&
					gameSceneData.playerState[threadnum].remainBullet > 0) {
					isCharging[threadnum] = true;
					shootDir[threadnum] = playerInput[threadnum].shootInput;
				}
				/*std::cout << "player upinput: " << std::boolalpha << playerInput[threadnum].isPressedMoveUp << std::endl;
				std::cout << "player downinput: " << std::boolalpha << playerInput[threadnum].isPressedMoveDown << std::endl;
				std::cout << "player leftinput: " << std::boolalpha << playerInput[threadnum].isPressedMoveLeft << std::endl;
				std::cout << "player rightinput: " << std::boolalpha << playerInput[threadnum].isPressedMoveRight << std::endl;*/
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
	float addBulletTimer[4] = { 0, };
	float mobTimer = 0;
	std::vector<float> mobActiveTimer;
	std::vector<int> mobtarget;

	float moveVal = 0;
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
						moveVal = PLAYER_MOVE_SPEED_PER_SECOND * delta;
						if (playerInput[i].isPressedMoveUp && y > moveVal) {
							y -= moveVal;
						}
						if (playerInput[i].isPressedMoveDown && y < MAP_SIZE_Y - moveVal) {
							y += moveVal;
						}
						// �� �̵� ���� ����
						if (!gameSceneData.mapData.blockState[(int)(y / BLOCK_SIZE_Y)][(int)(x / BLOCK_SIZE_X)]) {
							gameSceneData.playerState[i].positionY = y;
						}

						if (playerInput[i].isPressedMoveLeft && x > moveVal) {
							x -= moveVal;
						}
						if (playerInput[i].isPressedMoveRight && x < MAP_SIZE_X - moveVal) {
							x += moveVal;
						}
						if (!gameSceneData.mapData.blockState[(int)(y / BLOCK_SIZE_Y)][(int)(x / BLOCK_SIZE_X)]) {
							gameSceneData.playerState[i].positionX = x;
						}

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
				if (!bulletDatas.empty()) {
					for (size_t i = 0; i < bulletDatas.size(); i++)
					{
						//	�浹 - �� : �� ���� �� �Ѿ� ����, ���� �̺�Ʈ �޽���

						for (size_t j = 0; j < mobDatas.size(); j++)
						{
							if (Collision(mobDatas[j].positionX, bulletDatas[i].positionX,
								mobDatas[j].positionY, bulletDatas[i].positionY, BULLET_SIZE_X, PLAYER_SIZE_X)) {
								mobDatas.erase(mobDatas.begin() + j);
								break;
							}
						}

						x = bulletDatas[i].positionX / BLOCK_SIZE_X;
						y = bulletDatas[i].positionY / BLOCK_SIZE_Y;
						if (gameSceneData.mapData.blockState[(int)y][(int)x]) {
							gameSceneData.mapData.blockState[(int)y][(int)x] = false;
							//bulletDatas.erase(bulletDatas.begin() + i); // �� ����
						}

						// �Ѿ� �̵�
						x = bulletDatas[i].positionX;
						y = bulletDatas[i].positionY;
						moveVal = BULLET_MOVE_SPEED_PER_SECOND * delta;
						switch (bulletDatas[i].shootDirection)
						{
						case PlayerShootType::ShootUp:
							y -= moveVal;
							break;
						case PlayerShootType::ShootDown:
							y += moveVal;
							break;
						case PlayerShootType::ShootLeft:
							x -= moveVal;
							break;
						case PlayerShootType::ShootRight:
							x += moveVal;
							break;
						default:
							break;
						}

						bulletDatas[i].positionX = x;
						bulletDatas[i].positionY = y;

						if (bulletDatas[i].positionX < 0 || bulletDatas[i].positionX > MAP_SIZE_X ||
							bulletDatas[i].positionY < 0 || bulletDatas[i].positionY > MAP_SIZE_Y) {
							bulletDatas.erase(bulletDatas.begin() + i);
							break;
						}
					}

				}



				//  �� ����
				mobTimer += delta;
				if (mobTimer > 3) {
					mobTimer = 0;
					MobData* m = new MobData;
					m->isSpecialMob = false;
					m->positionX = rand() % MAP_SIZE_X;
					m->positionY = rand() % MAP_SIZE_Y;
					mobDatas.push_back(*m);
					mobActiveTimer.push_back(0);
					mobtarget.push_back(0);
				}

				//	��
				for (size_t i = 0; i < mobDatas.size(); i++)
				{
					for (size_t j = 0; j < MAX_PLAYER_LENGTH; j++)
					{
						// �� �÷��̾� ������ �Ÿ� ���

						// �Ÿ��� �� ����� ���
						//mobtarget[i] = j;
					}

					mobActiveTimer[i] += delta;
					if (mobActiveTimer[i] > 1) {
						// �̵�
						x = mobDatas[i].positionX;
						y = mobDatas[i].positionY;
						moveVal = (PLAYER_MOVE_SPEED_PER_SECOND / 2) * delta;

						if (x > moveVal) {
							x -= moveVal;
							mobDatas[i].positionX = x;
						}

						// �� ��ġ �� ����
						x = mobDatas[i].positionX / BLOCK_SIZE_X;
						y = mobDatas[i].positionY / BLOCK_SIZE_Y;
						gameSceneData.mapData.blockState[(int)y][(int)x] = true;
					}

					//	�浹 - �÷��̾� : �÷��̾� ���, ��� ī��Ʈ ����
					//	- �Ѿ� : �� ���� �� ���� �̺�Ʈ
				}


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
