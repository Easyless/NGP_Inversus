#pragma once
#include "InversusEnum.h"
#include "NetworkDummyObject.h"
#include "ClientSocketManager.h"

class Player : public RefreshObject<GameSceneData>
{
	static constexpr float speedNumber = 200;
	static constexpr float rotateSpeed = 200.0f;
	static constexpr float chargingSpeed = 400.0f;
	static constexpr float maxCharge = 1.5f;
	static constexpr float SpecialMaxDelay = 0.25;
	static constexpr float bulletRegenDelay = 1.5f;
private:
	int BulletCount = 0;
	int life = 0;
	float bulletRotate = 0;
	float CharginCount = 0.0f;
	float regenTime = 0.0f;
	UINT playerIndex;
public:
	Player(UINT index);
	virtual void RefreshFromData( const GameSceneData& data) override;
	virtual void Update( float deltaTime ) override;
	virtual void Draw(PaintInfo info) override;
};
