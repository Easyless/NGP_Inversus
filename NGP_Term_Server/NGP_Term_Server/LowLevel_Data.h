#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#include <winsock2.h>
#include <utility>

#pragma pack
#define MAX_PLAYER_LENGTH 4
#define MAX_LIFE_COUNT 3
#define BLOCK_COUNT_X 19 // ���� ����, �ʿ��� �� ����,���� ����
#define BLOCK_COUNT_Y 19 // ���� ����, �ʿ��� �� ����,���� ����
#define BLOCK_SIZE_X 50 //���� px , ���� �ȼ� ũ��
#define BLOCK_SIZE_Y 50 //���� px , ���� �ȼ� ũ��
#define MAP_SIZE_X (BLOCK_COUNT_X * BLOCK_SIZE_X) //���� px, �� ��ü �ȼ� ũ��
#define MAP_SIZE_Y (BLOCK_COUNT_Y * BLOCK_SIZE_Y) //���� px, �� ��ü �ȼ� ũ��

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

	MSG_PLAYER_INPUT = 11		// C->S �Ķ���� ���� (����)
};



enum PlayerWaitState : unsigned char
{
	WAIT_NOT_CONNECTED = 0,
	WAIT_CONNECTED_NORMAL = 1,
	WIAT_READY = 2
};

struct WaitRoomData // ���� ������ ��Ÿ���� ����ü
{
	PlayerWaitState playerWaitStates[MAX_PLAYER_LENGTH];
};

struct PlayerPlayState // �÷��̾� ������ ��Ÿ���� ����ü
{
	short positionX = -1;
	short positionY = -1;
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
	PLAYER,
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
inline UINT GetMessageParameterSize( NetGameMessageType type )
{
	switch ( type )
	{
	case MSG_MESSAGE_NULL: return 0;
	case MSG_WAIT_ROOM_DATA: return sizeof( WaitRoomData );
	case MSG_GAME_ALL_READY: return 0;
	case MSG_REQ_READY: return 0;
	case MSG_CANCLE_READY: return 0;
	case MSG_GAME_START: return 0;
	case MSG_SCENE_DATA: return sizeof( GameSceneData );
	case MSG_BULLET_DATA: return sizeof( BulletData );
	case MSG_MOB_DATA: return sizeof( MobData );
	case MSG_PLAYER_INPUT: return sizeof( PlayerInput );

	}
}

inline UINT GetMessageParameterCount( NetGameMessageType type, UINT parameterSize )
{
	switch ( type )
	{
	case MSG_MESSAGE_NULL: return 0;
	case MSG_WAIT_ROOM_DATA: return parameterSize / sizeof( WaitRoomData );
	case MSG_GAME_ALL_READY: return 0;
	case MSG_REQ_READY: return 0;
	case MSG_CANCLE_READY: return 0;
	case MSG_GAME_START: return 0;
	case MSG_SCENE_DATA: return parameterSize / sizeof( GameSceneData );
	case MSG_BULLET_DATA: return parameterSize / sizeof( BulletData );
	case MSG_MOB_DATA: return parameterSize / sizeof( MobData );
	case MSG_PLAYER_INPUT: return parameterSize / sizeof( PlayerInput );
	}
}

inline UINT GetMessageParameterCount( const NetGameMessage& message )
{
	return GetMessageParameterCount( message.type, message.parameterSize );
}