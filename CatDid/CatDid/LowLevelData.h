#pragma once
#include <Windows.h>
#define MAX_PLAYER_LENGTH 4
#define MAX_LIFE_COUNT 3
#define PLAYER_MOVE_SPEED_PER_SECOND 200.0f
#define BULLET_MOVE_SPEED_PER_SECOND 500.0f
#define BULLET_REGEN_SECOND 1.0f
#define MAX_BULLET_COUNT 6
#define BLOCK_COUNT_X 19 // 11.21 수정 단위 갯수, 맵에서 블럭 가로,세로 개수
#define BLOCK_COUNT_Y 19 // 11.21 수정 단위 갯수, 맵에서 블럭 가로,세로 개수
#define BLOCK_SIZE_X 40 // 11.21 수정 단위 px , 블럭의 픽셀 크기
#define BLOCK_SIZE_Y 40 // 11.21 수정 단위 px , 블럭의 픽셀 크기
#define BULLET_SIZE 8 //11.21 추가 : 총알 사이즈 
#define PLAYER_SIZE 25 //11.21 추가 : 플레이어, 적 사이즈
#define EXPLOSION_SIZE 3 //11.21 추가 : 폭발 블럭 갯수 (3x3) 블럭
#define MAP_SIZE_X (BLOCK_COUNT_X * BLOCK_SIZE_X) //단위 px, 맵 전체 픽셀 크기
#define MAP_SIZE_Y (BLOCK_COUNT_Y * BLOCK_SIZE_Y) //단위 px, 맵 전체 픽셀 크기
#define SERVER_PORT 15073

#pragma pack(1)

enum NetGameMessageType : unsigned char
{
	MSG_MESSAGE_NULL = 0,

	//WAIT ROOM MSG
	MSG_WAIT_ROOM_DATA = 1,			// S->C 파라메터 있음 (단일)
	MSG_GAME_ALL_READY = 2,		// S->C 파라메터 없음
	MSG_REQ_READY = 3,			// C->S 파라메터 없음 준비 상태로 변경 요청
	MSG_CANCLE_READY = 4,		// C->S 파라메터 없음

	//PLAY SCENE MSG
	MSG_GAME_START = 5,			// S->C 파라메터 없음
	MSG_SCENE_DATA = 6,			// S->C 파라메터 있음 (단일) 맵+플레이어 정보
	MSG_BULLET_DATA = 7,		// S->C 파라메터 있음 (개수 가변)
	MSG_MOB_DATA = 8,			// S->C 파라메터 있음 (개수 가변)

	//PLAY SCENE EVENT
	MSG_EVENT_EXPLOSION = 9,	// S->C 파라메터 있음 (개수 가변)
	MSG_EVENT_SPAWN = 10,		// S->C 파라메터 있음 (개수 가변)

	MSG_PLAYER_INPUT = 11,		// C->S 파라메터 있음 (단일)

	MSG_GAME_END = 12
};



enum PlayerWaitState : unsigned char
{
	WAIT_NOT_CONNECTED = 0,
	WAIT_CONNECTED_NORMAL = 1,
	WAIT_READY = 2
};

struct WaitRoomData // 대기방 정보를 나타내는 구조체
{
	PlayerWaitState playerWaitStates[MAX_PLAYER_LENGTH];
};

struct PlayerPlayState // 플레이어 정보를 나타내는 구조체
{
	short positionX = -1;
	short positionY = -1;
	unsigned int remainBullet = 6;
	bool isDead = false;
};
struct PlayMapData // 맵 정보를 나타내는 구조체
{
	bool blockState[BLOCK_COUNT_Y][BLOCK_COUNT_X];
};

struct GameSceneData // 맵 정보, 플레이어 정보, 플레이어 목숨을 한번에 전송하기 위한 구조체
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

struct BulletData // 총알 정보를 나타내는 구조체
{
	short positionX;
	short positionY;
	unsigned char ownerPlayer;
	PlayerShootType shootDirection;
};

struct MobData // 몹 정보를 나타내는 구조체
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
struct EventParameter // 이벤트(폭발) 정보를 나타내는 구조체
{
	short positionX;
	short positionY;
	EventOwnerType owner;
};

struct PlayerInput // 플레이어 입력을 나타내는 구조체
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
	UINT parameterSize; //뒤에 따라올 메시지 후속 정보의 크기 (갯수 * 구조체 사이즈)
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
	case MSG_GAME_END: return 0;
	case MSG_SCENE_DATA: return sizeof( GameSceneData );
	case MSG_BULLET_DATA: return sizeof( BulletData );
	case MSG_MOB_DATA: return sizeof( MobData );
	case MSG_PLAYER_INPUT: return sizeof( PlayerInput );
	case MSG_EVENT_EXPLOSION: return sizeof( EventParameter );
	case MSG_EVENT_SPAWN: return sizeof( EventParameter );
	}
}


#define TOSTRING(x) #x

inline const char* GetMessageString( NetGameMessageType type )
{
	switch ( type )
	{
	case MSG_MESSAGE_NULL: return TOSTRING( MSG_MESSAGE_NULL );
	case MSG_WAIT_ROOM_DATA: return TOSTRING( MSG_WAIT_ROOM_DATA );
	case MSG_GAME_ALL_READY: return TOSTRING( MSG_GAME_ALL_READY );
	case MSG_REQ_READY: return TOSTRING( MSG_REQ_READY );
	case MSG_CANCLE_READY: return TOSTRING( MSG_CANCLE_READY );
	case MSG_GAME_START: return TOSTRING( MSG_GAME_START );
	case MSG_GAME_END: return TOSTRING( MSG_GAME_END );
	case MSG_SCENE_DATA: return TOSTRING( MSG_SCENE_DATA );
	case MSG_BULLET_DATA: return TOSTRING( MSG_BULLET_DATA );
	case MSG_MOB_DATA: return TOSTRING( MSG_MOB_DATA );
	case MSG_PLAYER_INPUT: return TOSTRING( MSG_PLAYER_INPUT );
	case MSG_EVENT_EXPLOSION: return TOSTRING( MSG_EVENT_EXPLOSION );
	case MSG_EVENT_SPAWN: return TOSTRING( MSG_EVENT_SPAWN );
	default:
		return "MSG_ERROR_NOT_DEFINED";
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
	case MSG_GAME_END: return 0;
	case MSG_SCENE_DATA: return parameterSize / sizeof( GameSceneData );
	case MSG_BULLET_DATA: return parameterSize / sizeof( BulletData );
	case MSG_MOB_DATA: return parameterSize / sizeof( MobData );
	case MSG_PLAYER_INPUT: return parameterSize / sizeof( PlayerInput );
	case MSG_EVENT_EXPLOSION: return parameterSize / sizeof( EventParameter );
	case MSG_EVENT_SPAWN: return parameterSize / sizeof( EventParameter );
	}
}

inline UINT GetMessageParameterCount( const NetGameMessage& message )
{
	return GetMessageParameterCount( message.type, message.parameterSize );
}