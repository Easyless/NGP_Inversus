#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
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

// �̺�Ʈ�� ������ ����ȭ
// ���濡���� ������Ʈ x?

// ���� �ִ� 4��
bool isConnect[MAX_PLAYER_LENGTH] = { false, false, false, false };

// ���� ������
WaitRoomData waitRoomData;
GameSceneData gameSceneData;
BulletData bulletData;
MobData m;
EventParameter e;
PlayerInput p[MAX_PLAYER_LENGTH];
int connectedCount = 0;

// nextMessage ��� �����忡�� �ѹ��� ���� �� �ٲٱ� - ����ȭ
unsigned char nextMessage = MSG_MESSAGE_NULL;

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

	// �޽��� ����
	// MSG_MESSAGE_NULL - �޽��� ���� �ʿ� ���� ��(ex ��� �� ��ûx or  �ε� ��)

	// ��� �濡�� ����
	// MSG_WAIT_DATA, ��û Ȥ�� Ŭ���̾�Ʈ ���� ����� �� �ٸ� Ŭ�� ����
	// MSG_GAME_ALL_READY - ������Ʈ ��� ���� �غ� �� ����

	// ���� ������ ����
	// MSG_GAME_START - ���� ���� �� ���� �޽��� �ε� ������ ����
	// MSG_SCENE_DATA
	// MSG_BULLET_DATA
	// MSG_MOB_DATA
	
	while (true) {
		// Ŭ���̾�Ʈ ���� ���
		addrlen = sizeof(clientaddr);
		getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

		// ������ �۽� - �޼��� Ÿ�� �� ������
		retval = send(client_sock, (char*)&sendmessage, sizeof(NetGameMessage), 0);
		if (retval == SOCKET_ERROR) err_quit("send(), message");

		// ������ �۽� - �޼����� �ش��ϴ� ������ �۽�
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

		// ������ ���� - �޼��� Ÿ�� �� ������
		retval = recvn(client_sock, (char*)&receivemessage, sizeof(NetGameMessage), 0);
		if (retval == SOCKET_ERROR) err_quit("recvn(), message");

		UINT datasize = GetMessageParameterSize(receivemessage.type);

		EnterCriticalSection(&cs);
		// ������ ���� - �޼��� ������ ���� �ļ� ����
		switch (receivemessage.type)
		{
			// ��� �� ���� ���� �޽���
			// �޽��� ���� �� ���� �޽��� ���� ����(��� �� ����)
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
			// ���� ��� ó��
			waitRoomData.playerWaitStates[threadnum] = WAIT_CONNECTED_NORMAL;
			nextMessage = MSG_WAIT_ROOM_DATA;
			break;

		//	// ���� �� ���� ���� �޽���
		//case NetGameMessageType::MSG_PLAYER_INPUT:
		//	retval = recvn(client_sock, (char*)&p[threadnum], datasize, 0);
		//	if (retval == SOCKET_ERROR) err_quit("recvn, player input message");
		//	// ���� ��ǲ ���� ����, ������Ʈ�� ����
		//	break;

		default:
			// ����: NULL, ���� �÷���: MSG_SCENE_DATA
			nextMessage = MSG_MESSAGE_NULL; // ���� �޽��� NULL
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
		// �÷��̾� ����
		//for (size_t i = 0; i < MAX_PLAYER_LENGTH; i++)
		//{
		//	if (isConnect[i]) {
		//		if (p[i].isPressedMoveUp) {gameSceneData.playerState[i].positionY -= 1;}
		//		if (p[i].isPressedMoveDown) {gameSceneData.playerState[i].positionY += 1;}
		//		if (p[i].isPressedMoveLeft) { gameSceneData.playerState[i].positionX -= 1; }
		//		if (p[i].isPressedMoveRight) { gameSceneData.playerState[i].positionX += 1; }
		//	}
		//}

		// �� ����

		// �Ѿ� ����
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

		// Ŭ���̾�Ʈ �������� CommunicationThread ����
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
