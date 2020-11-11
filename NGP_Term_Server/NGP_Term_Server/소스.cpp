#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "LowLevel_Data.h"

#define SERVERPORT 15073

struct Param {
	SOCKET client_sock;
	int threadnum;
};

CRITICAL_SECTION cs;

// 이벤트로 스레드 동기화
// 대기방에서는 업데이트 x?

// 연결 최대 4개
bool isConnect[MAX_PLAYER_LENGTH] = { false, false, false, false };

// 전역 데이터
WaitRoomData waitRoomData;
GameSceneData gameSceneData;
BulletData bulletData;
MobData m;
EventParameter e;
PlayerInput p[MAX_PLAYER_LENGTH];
int connectedCount = 0;

// nextMessage 모든 스레드에서 한번씩 보낸 뒤 바꾸기 - 동기화
unsigned char nextMessage = MSG_MESSAGE_NULL;

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



DWORD WINAPI CommunicationThreadFunc(LPVOID arg) {
	int retval;

	Param* p = (Param*)arg;
	SOCKET client_sock = p->client_sock;
	int threadnum = p->threadnum;

	SOCKADDR_IN clientaddr;
	int size;
	int addrlen;

	NetGameMessage receivemessage;
	NetGameMessage sendmessage;

	int readyCount;

	// 메시지 종류
	// MSG_MESSAGE_NULL - 메시지 전송 필요 없을 시(ex 대기 방 요청x or  로딩 시)

	// 대기 방에서 전송
	// MSG_WAIT_DATA, 요청 혹은 클라이언트 연결 종료될 시 다른 클라에 전송
	// MSG_GAME_ALL_READY - 업데이트 결과 전부 준비 시 전송

	// 게임 씬에서 전송
	// MSG_GAME_START - 최초 시작 시 전송 메시지 로딩 끝나고 전송
	// MSG_SCENE_DATA
	// MSG_BULLET_DATA
	// MSG_MOB_DATA
	
	while (true) {
		// 클라이언트 정보 얻기
		addrlen = sizeof(clientaddr);
		getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

		// 데이터 송신 - 메세지 타입 및 사이즈
		retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);
		if (retval == SOCKET_ERROR) err_quit("send(), message");

		// 데이터 송신 - 메세지에 해당하는 데이터 송신
		switch (nextMessage) {
		case MSG_WAIT_ROOM_DATA:
			sendmessage.type = MSG_WAIT_ROOM_DATA;
			retval = send(client_sock, (char*)&waitRoomData, sizeof(WaitRoomData), 0);
			if (retval == SOCKET_ERROR) err_quit("send(), message");
			break;
		case MSG_GAME_START:
			sendmessage.type = MSG_GAME_START;
			break;
		case MSG_SCENE_DATA:
			sendmessage.type = MSG_SCENE_DATA;
			break;
		case MSG_BULLET_DATA:
			sendmessage.type = MSG_BULLET_DATA;
			break;
		case MSG_MOB_DATA:
			sendmessage.type = MSG_MOB_DATA;
			break;
		default:
			sendmessage.type = MSG_MESSAGE_NULL;
			break;
		}

		// 데이터 수신 - 메세지 타입 및 사이즈
		retval = recvn(client_sock, (char*)&receivemessage, sizeof(NetGameMessage), 0);
		if (retval == SOCKET_ERROR) err_quit("recvn(), message");

		UINT datasize = GetMessageParameterSize(receivemessage.type);

		EnterCriticalSection(&cs);
		// 데이터 수신 - 메세지 정보에 따른 후속 정보
		switch (receivemessage.type)
		{
			// 대기 방 수신 가능 메시지
			// 메시지 수신 시 다음 메시지 정보 전송(대기 방 갱신)
		case MSG_REQ_READY:
			retval = recvn(client_sock, (char*)&receivemessage, datasize, 0);
			if (retval == SOCKET_ERROR) err_quit("recvn, ready message");
			waitRoomData.playerWaitStates[threadnum] = WIAT_READY;

			readyCount = 0;
			for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
			{
				if (waitRoomData.playerWaitStates[i] == WIAT_READY) {
					readyCount++;
				}
			}
			if (readyCount == connectedCount) {
				nextMessage = MSG_GAME_ALL_READY;
			}
			else {
				nextMessage = MSG_WAIT_ROOM_DATA;
			}
			break;

		case MSG_CANCLE_READY:
			retval = recvn(client_sock, (char*)&receivemessage, datasize, 0);
			if (retval == SOCKET_ERROR) err_quit("recvn, cancle ready message");
			// 레디 취소 처리
			waitRoomData.playerWaitStates[threadnum] = WAIT_CONNECTED_NORMAL;
			nextMessage = MSG_WAIT_ROOM_DATA;
			break;

		//	// 게임 씬 수신 가능 메시지
		//case NetGameMessageType::MSG_PLAYER_INPUT:
		//	retval = recvn(client_sock, (char*)&p[threadnum], datasize, 0);
		//	if (retval == SOCKET_ERROR) err_quit("recvn, player input message");
		//	// 받은 인풋 상태 저장, 업데이트에 영향
		//	break;

		default:
			// 대기방: NULL, 게임 플레이: MSG_SCENE_DATA
			nextMessage = MSG_MESSAGE_NULL; // 다음 메시지 NULL
			break;
		}
		LeaveCriticalSection(&cs);
		break;
	}

	closesocket(client_sock);

	isConnect[threadnum] = false;
	connectedCount--;

	return 0;
}

DWORD WINAPI UpdateThreadFunc(LPVOID arg) {
	while (true) {
		// collision move 
		// 플레이어 관련
		//for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
		//{
		//	if (isConnect[i]) {
		//		if (p[i].isPressedMoveUp) {gameSceneData.playerState[i].positionY -= 1;}
		//		if (p[i].isPressedMoveDown) {gameSceneData.playerState[i].positionY += 1;}
		//		if (p[i].isPressedMoveLeft) { gameSceneData.playerState[i].positionX -= 1; }
		//		if (p[i].isPressedMoveRight) { gameSceneData.playerState[i].positionX += 1; }
		//	}
		//}

		// 몹 관련

		// 총알 관련
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
				connectedCount++;
				success = true;
				break;
			}
		}

		// 클라이언트 소켓으로 CommunicationThread 생성
		if (success) {
			arg = { client_sock, threadnum };
			hThread = CreateThread(NULL, 0, CommunicationThreadFunc,
				(LPVOID)&p, 0, NULL);
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
