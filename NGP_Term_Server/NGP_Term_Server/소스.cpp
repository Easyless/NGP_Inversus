#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include "LowLevelData.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winmm.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지

#define SERVERPORT 15073
#define FPS 30.f
#define BULLET_SPEED 2.f
#define PLAYER_SPEED 2.f

struct Param {
	SOCKET client_sock;
	int threadnum;
};

struct RemainExplosion {
	float timer = 0;
	EventParameter explosioninfo;
};

CRITICAL_SECTION cs;

// 이벤트로 스레드 동기화
// 대기방에서는 업데이트 x?

// 전역 데이터
// 연결 최대 4개
bool isConnect[MAX_PLAYER_LENGTH] = { false, false, false, false };
bool isPlay = false;
bool isStart = false;

bool isSend[MAX_PLAYER_LENGTH] = { false, };
bool isCharging[MAX_PLAYER_LENGTH] = { false, };
PlayerShootType shootDir[MAX_PLAYER_LENGTH] = { None, };
float chargingTime[MAX_PLAYER_LENGTH] = { 0, };

SOCKET connectedSocket[MAX_PLAYER_LENGTH] = { NULL, NULL, NULL, NULL };
WaitRoomData waitRoomData;
GameSceneData gameSceneData;
std::vector<EventParameter> spawns;
std::vector<EventParameter> explosions;
std::vector<RemainExplosion> remainExplosions;
std::vector<BulletData> bulletDatas;
std::vector<MobData> mobDatas;
PlayerInput playerInput[MAX_PLAYER_LENGTH];
int connectedCount = 0;

void InitSceneData() {
	gameSceneData.leftLifeCount = 3;
	gameSceneData.mapData = { false, };
	for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
	{
		gameSceneData.playerState[i].positionX = i * 50;
		gameSceneData.playerState[i].positionY = i * 30;
		gameSceneData.playerState[i].isDead = true;
	}
}

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

// 사용자 정의 데이터 수신 함수
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
	EnterCriticalSection(&cs);
	for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
	{
		if (isConnect[i]) {
			send(connectedSocket[i], (char*)&sendmessage, sizeof(NetGameMessage), 0);
			switch (sendmessage.type) {
			case MSG_WAIT_ROOM_DATA:
				send(connectedSocket[i], (char*)&waitRoomData, sizeof(WaitRoomData), 0);
				std::cout << "send MSG_WAIT_ROOM_DATA" << std::endl;
				break;
			default:
				break;
			}
		}
	}
	LeaveCriticalSection(&cs);
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

float Distance(float px1, float px2, float py1, float py2) {
	return (px1 - px2) * (px1 - px2) + (py1 - py2) * (py1 - py2);
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
			isSend[threadnum] = false; // 업데이트 대기
			// 데이터 송신 - 메세지 타입 및 사이즈
			if (isPlay) {
				sendmessage.type = MSG_SCENE_DATA;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type);
			}

			retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);

			//std::cout << "sendmessage type: " << sendmessage.type << std::endl;

			// 데이터 송신 - 메세지에 해당하는 데이터 송신
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

				EnterCriticalSection(&cs);

				sendmessage.type = MSG_BULLET_DATA;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type) * bulletDatas.size();
				retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);
				retval = send(client_sock, (char*)bulletDatas.data(), sizeof(BulletData) * bulletDatas.size(), 0);

				sendmessage.type = MSG_MOB_DATA;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type) * mobDatas.size();
				retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);
				retval = send(client_sock, (char*)mobDatas.data(), sizeof(MobData) * mobDatas.size(), 0);

				if (!explosions.empty()) {
					sendmessage.type = MSG_EVENT_EXPLOSION;
					sendmessage.parameterSize = sizeof(EventParameter) * explosions.size();
					retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);
					retval = send(client_sock, (char*)explosions.data(), sizeof(EventParameter) * explosions.size(), 0);
					explosions.clear();
				}

				if (!spawns.empty()) {
					sendmessage.type = MSG_EVENT_SPAWN;
					sendmessage.parameterSize = sizeof(EventParameter) * spawns.size();
					retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);
					retval = send(client_sock, (char*)spawns.data(), sizeof(EventParameter) * spawns.size(), 0);
					spawns.clear();
				}

				LeaveCriticalSection(&cs);
				//SendtoAll(sendmessage);
				break;
			case MSG_BULLET_DATA:
				break;
			case MSG_MOB_DATA:
				break;
			default:
				break;
			}

			// 데이터 수신 - 메세지 타입 및 사이즈
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

			// 데이터 수신 - 메세지 정보에 따른 후속 정보
			switch (receivemessage.type)
			{
				// 대기 방 수신 가능 메시지
				// 메시지 수신 시 다음 메시지 정보 전송(대기 방 갱신)
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
				// 레디 취소 처리
				std::cout << "recv ready cancle msg" << std::endl;
				readyCount--;
				waitRoomData.playerWaitStates[threadnum] = WAIT_CONNECTED_NORMAL;
				sendmessage.type = MSG_WAIT_ROOM_DATA;
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type);
				SendtoAll(sendmessage);
				break;

				// 게임 씬 수신 가능 메시지
			case NetGameMessageType::MSG_PLAYER_INPUT:
				EnterCriticalSection(&cs);
				retval = recvn(client_sock, (char*)&playerInput[threadnum], datasize, 0);
				// 받은 인풋 상태 저장, 업데이트에 영향
				if (!gameSceneData.playerState[threadnum].isDead) {
					if (playerInput[threadnum].shootInput == PlayerShootType::None && isCharging[threadnum] &&
						gameSceneData.playerState[threadnum].remainBullet > 0) { // 총알 생성
						// 차지시간 if
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
				}
				LeaveCriticalSection(&cs);
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
	DWORD lastTime;
	DWORD currTime;

	float minDistance = -1;
	float x, y, tx, ty, val;
	float delta = 0;
	float addBulletTimer[MAX_PLAYER_LENGTH] = { 0, };
	float mobTimer = 0;
	std::vector<float> mobActiveTimer;
	float playerActiveTimer[4] = {0,};
	std::vector<int> mobtarget;

	float moveVal = 0;
	while (true) {
		if (isPlay) {
			if (isStart) { // 시작 2초 후부터 업데이트 
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
				lastTime = timeGetTime();
			}

			//Deltatime
			currTime = timeGetTime();
			delta = (currTime - lastTime) * 0.001f;
			
			if (delta >= 1.f / FPS) {
				// 플레이어
				// move
				EnterCriticalSection(&cs);
				for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
				{
					if (isConnect[i]) {
						if (gameSceneData.playerState[i].isDead) {
							playerActiveTimer[i] += delta;
							if (playerActiveTimer[i] > 3) {

								EventParameter* ep = new EventParameter;
								ep->positionX = gameSceneData.playerState[i].positionX;
								ep->positionY = gameSceneData.playerState[i].positionY;
								ep->owner = (EventOwnerType)(PLAYER0 + i);
								spawns.push_back(*ep);

								// 주변 적 폭발 처리
								for (size_t j = 0; j < mobDatas.size(); j++)
								{
									if (Collision(mobDatas[j].positionX, gameSceneData.playerState[i].positionX,
										mobDatas[j].positionY, gameSceneData.playerState[i].positionY,
										PLAYER_SIZE, PLAYER_SIZE * EXPLOSION_SIZE)) {
										
										EventParameter* ep = new EventParameter;
										ep->positionX = mobDatas[j].positionX;
										ep->positionY = mobDatas[j].positionY;
										if (mobDatas[j].isSpecialMob)
											ep->owner = SpecialMob;
										else
											ep->owner = NormalMob;
										explosions.push_back(*ep);

										RemainExplosion* re = new RemainExplosion;
										re->explosioninfo = *ep;
										remainExplosions.push_back(*re);

										mobDatas.erase(mobDatas.begin() + j);
										mobtarget.erase(mobtarget.begin() + j);
										mobActiveTimer.erase(mobActiveTimer.begin() + j);
										j = 0;
									}
								}
								x = gameSceneData.playerState[i].positionX;
								y = gameSceneData.playerState[i].positionY;

								for (int h = -1; h < 2; h++)
								{
									for (int v = -1; v < 2; v++)
									{
										if((int)(y / BLOCK_SIZE_Y) + h >= 0 && (int)(y / BLOCK_SIZE_Y) + h < MAP_SIZE_Y &&
											(int)(x / BLOCK_SIZE_X) + v >= 0 && (int)(x / BLOCK_SIZE_X) + v < MAP_SIZE_X)
										gameSceneData.mapData.blockState[(int)(y / BLOCK_SIZE_Y) + h][(int)(x / BLOCK_SIZE_X) + v] = false;
									}
								}
								gameSceneData.playerState[i].isDead = false;
								playerActiveTimer[i] = 0;
							}
						}
						else {
							x = gameSceneData.playerState[i].positionX;
							y = gameSceneData.playerState[i].positionY;
							moveVal = PLAYER_MOVE_SPEED_PER_SECOND * delta;
							if (playerInput[i].isPressedMoveUp && y > moveVal) {
								y -= moveVal;
							}
							if (playerInput[i].isPressedMoveDown && y < MAP_SIZE_Y - moveVal) {
								y += moveVal;
							}
							// 블럭 이동 가능 여부
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
								if (addBulletTimer[i] > BULLET_REGEN_SECOND) {
									gameSceneData.playerState[i].remainBullet += 1;
									addBulletTimer[i] = 0;
								}
							}
						}

					}
				}

				//	총알
				if (!bulletDatas.empty()) {
					for (size_t i = 0; i < bulletDatas.size(); i++)
					{
						//	충돌 - 몹 : 몹 폭발 및 총알 삭제, 폭발 이벤트 메시지
						for (size_t j = 0; j < mobDatas.size(); j++)
						{
							if (mobActiveTimer[j] > 1) {
								if (Collision(mobDatas[j].positionX, bulletDatas[i].positionX,
									mobDatas[j].positionY, bulletDatas[i].positionY, BULLET_SIZE, PLAYER_SIZE)) {
									
									EventParameter* ep = new EventParameter;
									ep->positionX = mobDatas[j].positionX;
									ep->positionY = mobDatas[j].positionY;
									if (mobDatas[j].isSpecialMob)
										ep->owner = SpecialMob;
									else
										ep->owner = NormalMob;
									explosions.push_back(*ep);
									// 폭발 위치 저장 및 메시지 전송
									RemainExplosion* re = new RemainExplosion;
									re->explosioninfo = *ep;
									remainExplosions.push_back(*re);

									x = mobDatas[j].positionX;
									y = mobDatas[j].positionY;
									for (int h = -1; h < 2; h++)
									{
										for (int v = -1; v < 2; v++)
										{
											if ((int)(y / BLOCK_SIZE_Y) + h >= 0 && (int)(y / BLOCK_SIZE_Y) + h < MAP_SIZE_Y &&
												(int)(x / BLOCK_SIZE_X) + v >= 0 && (int)(x / BLOCK_SIZE_X) + v < MAP_SIZE_X)
												gameSceneData.mapData.blockState[(int)(y / BLOCK_SIZE_Y) + h][(int)(x / BLOCK_SIZE_X) + v] = false;
										}
									}
									mobDatas.erase(mobDatas.begin() + j);
									mobtarget.erase(mobtarget.begin() + j);
									mobActiveTimer.erase(mobActiveTimer.begin() + j);
									break;
								}
							}
						}

						x = bulletDatas[i].positionX / BLOCK_SIZE_X;
						y = bulletDatas[i].positionY / BLOCK_SIZE_Y;
						if (gameSceneData.mapData.blockState[(int)y][(int)x]) {
							gameSceneData.mapData.blockState[(int)y][(int)x] = false;
						}

						// 총알 이동
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


				//  몹 생성
				mobTimer += delta;
				if (mobTimer > 3) { // 몹 리젠시간, 방식 정하기
					mobTimer = 0;
					MobData* m = new MobData;
					m->isSpecialMob = false;
					m->positionX = rand() % MAP_SIZE_X;
					m->positionY = rand() % MAP_SIZE_Y;
					mobDatas.push_back(*m);
					mobActiveTimer.push_back(0);
					mobtarget.push_back(0);

					EventParameter* ep = new EventParameter;
					ep->positionX =	m->positionX;
					ep->positionY =	m->positionY;
					if (m->isSpecialMob)
						ep->owner = SpecialMob;
					else
						ep->owner = NormalMob;
					spawns.push_back(*ep);
				}

				//	몹
				for (size_t i = 0; i < mobDatas.size(); i++)
				{
					minDistance = -1;
					for (size_t j = 0; j < MAX_PLAYER_LENGTH; j++)
					{
						if (!gameSceneData.playerState[j].isDead) {
							x = Distance(mobDatas[i].positionX, gameSceneData.playerState[j].positionX,
								mobDatas[i].positionY, gameSceneData.playerState[j].positionY);
							// 각 플레이어 까지의 거리 계산
							if (minDistance == -1 || x < minDistance) {
								minDistance = x;
								mobtarget[i] = j;
							}



							if (Collision(mobDatas[i].positionX, gameSceneData.playerState[j].positionX,
									mobDatas[i].positionY, gameSceneData.playerState[j].positionY, PLAYER_SIZE, PLAYER_SIZE)) {
								gameSceneData.leftLifeCount--;

								EventParameter* ep = new EventParameter;
								ep->positionX = gameSceneData.playerState[j].positionX;
								ep->positionY = gameSceneData.playerState[j].positionY;

								ep->owner = (EventOwnerType)(PLAYER0 + j);
								explosions.push_back(*ep);

								RemainExplosion* re = new RemainExplosion;
								re->explosioninfo = *ep;
								remainExplosions.push_back(*re);

								gameSceneData.playerState[j].isDead = true;
							}
						}
					}

					mobActiveTimer[i] += delta;
					if (mobActiveTimer[i] > 1) {
						// 이동
						x = mobDatas[i].positionX;
						y = mobDatas[i].positionY;
						moveVal = (PLAYER_MOVE_SPEED_PER_SECOND / 2) * delta;

						tx = x - gameSceneData.playerState[mobtarget[i]].positionX;
						ty = y - gameSceneData.playerState[mobtarget[i]].positionY;

						val = sqrt(pow(tx, 2) + pow(ty, 2));

						tx = tx / val;
						ty = ty / val;

						x -= tx * moveVal;
						y -= ty * moveVal;

						mobDatas[i].positionX = x;
						mobDatas[i].positionY = y;

						// 몹 위치 블럭 막기
						x = mobDatas[i].positionX / BLOCK_SIZE_X;
						y = mobDatas[i].positionY / BLOCK_SIZE_Y;
						gameSceneData.mapData.blockState[(int)y][(int)x] = true;
					}
				}
				
				// 연쇄폭발
				if (!remainExplosions.empty()) {
					for (size_t i = 0; i < remainExplosions.size(); i++)
					{
						remainExplosions[i].timer += delta;

						if (remainExplosions[i].timer > 1) {
							remainExplosions.erase(remainExplosions.begin() + i);
						}
						else {
							for (size_t j = 0; j < mobDatas.size(); j++)
							{
								if (Collision(mobDatas[j].positionX, remainExplosions[i].explosioninfo.positionX,
									mobDatas[j].positionY, remainExplosions[i].explosioninfo.positionY,
									PLAYER_SIZE, PLAYER_SIZE * EXPLOSION_SIZE)) {

									EventParameter* ep = new EventParameter;
									ep->positionX = mobDatas[j].positionX;
									ep->positionY = mobDatas[j].positionY;
									if (mobDatas[j].isSpecialMob)
										ep->owner = SpecialMob;
									else
										ep->owner = NormalMob;
									explosions.push_back(*ep);


									RemainExplosion* re = new RemainExplosion;
									re->explosioninfo = *ep;
									remainExplosions.push_back(*re);

									x = mobDatas[j].positionX;
									y = mobDatas[j].positionY;
									for (int h = -1; h < 2; h++)
									{
										for (int v = -1; v < 2; v++)
										{
											if ((int)(y / BLOCK_SIZE_Y) + h >= 0 && (int)(y / BLOCK_SIZE_Y) + h < MAP_SIZE_Y &&
												(int)(x / BLOCK_SIZE_X) + v >= 0 && (int)(x / BLOCK_SIZE_X) + v < MAP_SIZE_X)
												gameSceneData.mapData.blockState[(int)(y / BLOCK_SIZE_Y) + h][(int)(x / BLOCK_SIZE_X) + v] = false;
										}
									}

									mobDatas.erase(mobDatas.begin() + j);
									mobtarget.erase(mobtarget.begin() + j);
									mobActiveTimer.erase(mobActiveTimer.begin() + j);
									j = 0;


								}
							}
						}
					}
				}

				LeaveCriticalSection(&cs);

				lastTime = currTime;
				for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
				{
					if (!gameSceneData.playerState[i].isDead) {
						isSend[i] = true; // 업데이트 후 메시지 전송
					}
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

	// 윈속 초기화
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

	BOOL optval = TRUE; // Nagle 알고리즘 중지
	setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY,
		(char*)&optval, sizeof(optval));

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	// 스레드 핸들
	HANDLE hThread;

	// 스레드에 넘길 인자
	Param arg;


	// UpdateThread 생성
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
			if (!isConnect[i] && !isPlay) {
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
		// 클라이언트 소켓으로 CommunicationThread 생성
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
