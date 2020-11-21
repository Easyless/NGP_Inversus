#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <list>
#include "LowLevelData.h"
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winmm.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지

#define SERVERPORT 15073
#define FPS 1

struct Param {
	SOCKET client_sock;
	int threadnum;
};

CRITICAL_SECTION cs;

// 이벤트로 스레드 동기화
// 대기방에서는 업데이트 x?

// 전역 데이터
// 연결 최대 4개
bool isConnect[MAX_PLAYER_LENGTH] = { false, false, false, false };
bool isPlay = false;
bool isStart = false;
bool isSend = false;

SOCKET connectedSocket[4] = { NULL, NULL, NULL, NULL };
WaitRoomData waitRoomData;
GameSceneData gameSceneData;
std::list<BulletData> bulletDatas;
std::list<MobData> mobDatas;
EventParameter eventParameter;
PlayerInput playerInput[MAX_PLAYER_LENGTH];
int connectedCount = 0;


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

	SendtoAll(sendmessage);

	int readyCount = 0;

	while (true) {
		std::cout << "cycle" << std::endl;
		if (!isPlay || (isPlay && isSend)) {
			// 데이터 송신 - 메세지 타입 및 사이즈
			retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);
			if (retval == SOCKET_ERROR) err_display("send(), message");

			std::cout << "sendmessage type: " << sendmessage.type << std::endl;

			// 데이터 송신 - 메세지에 해당하는 데이터 송신
			switch (sendmessage.type) {
			case MSG_WAIT_ROOM_DATA:
				retval = send(client_sock, (char*)&waitRoomData, sizeof(WaitRoomData), 0);
				if (retval == SOCKET_ERROR) err_display("send(), message");
				std::cout << "send roomdata" << std::endl;
				break;
			case MSG_GAME_START:
				break;
			case MSG_SCENE_DATA:
				retval = send(client_sock, (char*)&gameSceneData, sizeof(GameSceneData), 0);
				if (retval == SOCKET_ERROR) err_display("send(), message");
				std::cout << "send roomdata" << std::endl;
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

			UINT datasize = GetMessageParameterSize(receivemessage.type);

			//EnterCriticalSection(&cs);
			// 데이터 수신 - 메세지 정보에 따른 후속 정보
			switch (receivemessage.type)
			{
				// 대기 방 수신 가능 메시지
				// 메시지 수신 시 다음 메시지 정보 전송(대기 방 갱신)
			case MSG_REQ_READY:
				retval = recvn(client_sock, (char*)&receivemessage, datasize, 0);
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
					SendtoAll(sendmessage);
				}
				else {
					sendmessage.type = MSG_WAIT_ROOM_DATA;
				}
				sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type);
				SendtoAll(sendmessage);
				break;

			case MSG_CANCLE_READY:
				retval = recvn(client_sock, (char*)&receivemessage, datasize, 0);
				if (retval == SOCKET_ERROR || 0) {
					err_display("recvn, ready message");
					isConnect[threadnum] = false;
					connectedCount--;
					waitRoomData.playerWaitStates[threadnum] = WAIT_NOT_CONNECTED;
					sendmessage.type = MSG_WAIT_ROOM_DATA;
					sendmessage.parameterSize = GetMessageParameterSize(sendmessage.type);
					SendtoAll(sendmessage);
					std::cout << "end communication thread" << std::endl;
					return 0;
				}
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
				retval = recvn(client_sock, (char*)&playerInput[threadnum], datasize, 0);
				if (retval == SOCKET_ERROR) err_quit("recvn, player input message");

				// 받은 인풋 상태 저장, 업데이트에 영향
				if (playerInput[threadnum].shootInput != PlayerShootType::None) { // 총알 생성
					BulletData* b = new BulletData;
					b->ownerPlayer = threadnum;
					b->positionX = gameSceneData.playerState[threadnum].positionX;
					b->positionY = gameSceneData.playerState[threadnum].positionY;
					b->shootDirection = playerInput[threadnum].shootInput;
					bulletDatas.push_back(*b);
				}

				std::cout << "player upinput: " << playerInput[threadnum].isPressedMoveUp << std::endl;
				std::cout << "player downinput: " << playerInput[threadnum].isPressedMoveDown << std::endl;
				std::cout << "player leftinput: " << playerInput[threadnum].isPressedMoveLeft << std::endl;
				std::cout << "player rightinput: " << playerInput[threadnum].isPressedMoveRight << std::endl;
				break;

			default:
				break;
			}
		}

		isSend = false; // 업데이트 대기

		//LeaveCriticalSection(&cs);
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
	DWORD Delta = 0;

	while (true) {
		if (isPlay) {
			if (isStart) { // 시작 2초 후부터 업데이트 
				Sleep(2000);
				isStart = false;
			}

			//Deltatime
			currTime = timeGetTime();
			Delta = (currTime - lastTime) * 0.001f;

			if (Delta >= 1 / FPS) {
				//플레이어 관련

				// move
				for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
				{
					if (isConnect[i]) {
						if (playerInput[i].isPressedMoveUp) { gameSceneData.playerState[i].positionY -= 1; }
						if (playerInput[i].isPressedMoveDown) { gameSceneData.playerState[i].positionY += 1; }
						if (playerInput[i].isPressedMoveLeft) { gameSceneData.playerState[i].positionX -= 1; }
						if (playerInput[i].isPressedMoveRight) { gameSceneData.playerState[i].positionX += 1; }
					}
				}

				for (auto b : bulletDatas)
				{
					switch (b.shootDirection)
					{
					case PlayerShootType::ShootUp:
						b.positionY -= BULLETSPEED;
						break;
					case PlayerShootType::ShootDown:
						b.positionY += BULLETSPEED;
						break;
					case PlayerShootType::ShootLeft:
						b.positionX -= BULLETSPEED;
						break;
					case PlayerShootType::ShootRight:
						b.positionX += BULLETSPEED;
						break;
					default:
						break;
					}
				}


				// collision 

				// 몹, 플레이어 충돌 - 플레이어 라이프 감소
				// 몹, 총알 충돌 - 몹 삭제 처리

				// create

				// create mob

				lastTime = currTime;
				isSend = true; // 업데이트 후 메시지 전송
				std::cout << "update" << std::endl;
			}
		}

		if (connectedCount == 0) {
			isPlay = false;
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
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

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
		// 클라이언트 소켓으로 CommunicationThread 생성
		if (success) {
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
