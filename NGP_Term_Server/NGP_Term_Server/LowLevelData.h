#pragma once
#include <Windows.h>
#define MAX_PLAYER_LENGTH 4
#define MAX_LIFE_COUNT 3
#define PLAYER_MOVE_SPEED_PER_SECOND 200.0f
#define BULLET_MOVE_SPEED_PER_SECOND 500.0f
#define BULLET_REGEN_SECOND 1.0f
#define MAX_BULLET_COUNT 6
#define BLOCK_COUNT_X 19 // 11.21 ���� ���� ����, �ʿ��� �� ����,���� ����
#define BLOCK_COUNT_Y 19 // 11.21 ���� ���� ����, �ʿ��� �� ����,���� ����
#define BLOCK_SIZE_X 40 // 11.21 ���� ���� px , ���� �ȼ� ũ��
#define BLOCK_SIZE_Y 40 // 11.21 ���� ���� px , ���� �ȼ� ũ��
#define BULLET_SIZE 8 //11.21 �߰� : �Ѿ� ������ 
#define PLAYER_SIZE 25 //11.21 �߰� : �÷��̾�, �� ������
#define EXPLOSION_SIZE 3 //11.21 �߰� : ���� �� ���� (3x3) ��
#define MAP_SIZE_X (BLOCK_COUNT_X * BLOCK_SIZE_X) //���� px, �� ��ü �ȼ� ũ��
#define MAP_SIZE_Y (BLOCK_COUNT_Y * BLOCK_SIZE_Y) //���� px, �� ��ü �ȼ� ũ��
#define SERVER_PORT 15073

#pragma pack(1)

enum NetGameMessageType : unsigned char
{
	MSG_MESSAGE_NULL = 0,

	//WAIT ROOM MSG
	MSG_WAIT_ROOM_DATA = 1,			// S->C �Ķ���� ���� (����)
	MSG_GAME_ALL_READY = 2,		// S->C �Ķ���� ����
	MSG_REQ_READY = 3,			// C->S �Ķ���� ���� �غ� ���·� ���� ��û
	MSG_CANCLE_READY = 4,		// C->S �Ķ���� ����

	//PLAY SCENE MSG
	MSG_GAME_START = 5,			// S->C �Ķ���� ����
	MSG_SCENE_DATA = 6,			// S->C �Ķ���� ���� (����) ��+�÷��̾� ����
	MSG_BULLET_DATA = 7,		// S->C �Ķ���� ���� (���� ����)
	MSG_MOB_DATA = 8,			// S->C �Ķ���� ���� (���� ����)

	//PLAY SCENE EVENT
	MSG_EVENT_EXPLOSION = 9,	// S->C �Ķ���� ���� (���� ����)
	MSG_EVENT_SPAWN = 10,		// S->C �Ķ���� ���� (���� ����)

	MSG_PLAYER_INPUT = 11,		// C->S �Ķ���� ���� (����)

	MSG_GAME_END = 12
};



enum PlayerWaitState : unsigned char
{
	WAIT_NOT_CONNECTED = 0,
	WAIT_CONNECTED_NORMAL = 1,
	WAIT_READY = 2
};

struct WaitRoomData // ���� ������ ��Ÿ���� ����ü
{
	PlayerWaitState playerWaitStates[MAX_PLAYER_LENGTH];
};

struct PlayerPlayState // �÷��̾� ������ ��Ÿ���� ����ü
{
	short positionX = -1;
	short positionY = -1;
	unsigned int remainBullet = 6;
	bool isDead = false;
};
struct PlayMapData // �� ������ ��Ÿ���� ����ü
{
	bool blockState[BLOCK_COUNT_Y][BLOCK_COUNT_X];
};

struct GameSceneData // �� ����, �÷��̾� ����, �÷��̾� ����� �ѹ��� �����ϱ� ���� ����ü
{
	unsigned char leftLifeCount = MAX_LIFE_COUNT;
	PlayerPlayState playerState[MAX_PLAYER_LENGTH];
	PlayMapData mapData;
};

enum PlayerShootType : unsigned char
{
	None = 0,
	ShootUp = 1,
	ShootDown = 2,
	ShootLeft = 3,
	ShootRight = 4
};

struct BulletData // �Ѿ� ������ ��Ÿ���� ����ü
{
	short positionX;
	short positionY;
	unsigned char ownerPlayer;
	PlayerShootType shootDirection;
};

struct MobData // �� ������ ��Ÿ���� ����ü
{
	short positionX;
	short positionY;
	bool isSpecialMob = false;
};

enum EventOwnerType : unsigned char
{
	PLAYER0,
	PLAYER1,
	PLAYER2,
	PLAYER3,
	NormalMob,
	SpecialMob
};
struct EventParameter // �̺�Ʈ(����) ������ ��Ÿ���� ����ü
{
	short positionX;
	short positionY;
	EventOwnerType owner;
};

struct PlayerInput // �÷��̾� �Է��� ��Ÿ���� ����ü
{
	bool isPressedMoveUp = false;
	bool isPressedMoveDown = false;
	bool isPressedMoveLeft = false;
	bool isPressedMoveRight = false;
	PlayerShootType shootInput = PlayerShootType::None;
};

struct NetGameMessage
{
	NetGameMessageType type;
	UINT parameterSize; //�ڿ� ����� �޽��� �ļ� ������ ũ�� (���� * ����ü ������)
};
#pragma pack()

//Utill Function
inline UINT GetMessageParameterSize(NetGameMessageType type)
{
	switch (type)
	{
	case MSG_MESSAGE_NULL: return 0;
	case MSG_WAIT_ROOM_DATA: return sizeof(WaitRoomData);
	case MSG_GAME_ALL_READY: return 0;
	case MSG_REQ_READY: return 0;
	case MSG_CANCLE_READY: return 0;
	case MSG_GAME_START: return 0;
	case MSG_GAME_END: return 0;
	case MSG_SCENE_DATA: return sizeof(GameSceneData);
	case MSG_BULLET_DATA: return sizeof(BulletData);
	case MSG_MOB_DATA: return sizeof(MobData);
	case MSG_PLAYER_INPUT: return sizeof(PlayerInput);
	case MSG_EVENT_EXPLOSION: return sizeof(EventParameter);
	case MSG_EVENT_SPAWN: return sizeof(EventParameter);
	}
}


#define TOSTRING(x) #x

inline const char* GetMessageString(NetGameMessageType type)
{
	switch (type)
	{
	case MSG_MESSAGE_NULL: return TOSTRING(MSG_MESSAGE_NULL);
	case MSG_WAIT_ROOM_DATA: return TOSTRING(MSG_WAIT_ROOM_DATA);
	case MSG_GAME_ALL_READY: return TOSTRING(MSG_GAME_ALL_READY);
	case MSG_REQ_READY: return TOSTRING(MSG_REQ_READY);
	case MSG_CANCLE_READY: return TOSTRING(MSG_CANCLE_READY);
	case MSG_GAME_START: return TOSTRING(MSG_GAME_START);
	case MSG_GAME_END: return TOSTRING(MSG_GAME_END);
	case MSG_SCENE_DATA: return TOSTRING(MSG_SCENE_DATA);
	case MSG_BULLET_DATA: return TOSTRING(MSG_BULLET_DATA);
	case MSG_MOB_DATA: return TOSTRING(MSG_MOB_DATA);
	case MSG_PLAYER_INPUT: return TOSTRING(MSG_PLAYER_INPUT);
	case MSG_EVENT_EXPLOSION: return TOSTRING(MSG_EVENT_EXPLOSION);
	case MSG_EVENT_SPAWN: return TOSTRING(MSG_EVENT_SPAWN);
	default:
		return "MSG_ERROR_NOT_DEFINED";
	}
}

inline UINT GetMessageParameterCount(NetGameMessageType type, UINT parameterSize)
{
	switch (type)
	{
	case MSG_MESSAGE_NULL: return 0;
	case MSG_WAIT_ROOM_DATA: return parameterSize / sizeof(WaitRoomData);
	case MSG_GAME_ALL_READY: return 0;
	case MSG_REQ_READY: return 0;
	case MSG_CANCLE_READY: return 0;
	case MSG_GAME_START: return 0;
	case MSG_GAME_END: return 0;
	case MSG_SCENE_DATA: return parameterSize / sizeof(GameSceneData);
	case MSG_BULLET_DATA: return parameterSize / sizeof(BulletData);
	case MSG_MOB_DATA: return parameterSize / sizeof(MobData);
	case MSG_PLAYER_INPUT: return parameterSize / sizeof(PlayerInput);
	case MSG_EVENT_EXPLOSION: return parameterSize / sizeof(EventParameter);
	case MSG_EVENT_SPAWN: return parameterSize / sizeof(EventParameter);
	}
}

inline UINT GetMessageParameterCount(const NetGameMessage& message)
{
	return GetMessageParameterCount(message.type, message.parameterSize);
}