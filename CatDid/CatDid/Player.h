#pragma once
#include "GameObject.h"
#include "InversusEnum.h"
#include "ClientSocketManager.h"

class Player : public GameObject
{
	static constexpr float speedNumber = 200;
	static constexpr float rotateSpeed = 200.0f;
	static constexpr float chargingSpeed = 400.0f;
	static constexpr float maxCharge = 1.5f;
	static constexpr float SpecialMaxDelay = 0.25;
	static constexpr float bulletRegenDelay = 1.5f;
private:
	int SpecialBulletCount = 0;
	int BulletCount = 0;
	int life = 0;
	float bulletRotate = 0;
	float regenTime = 0;
	float CharginCount = 0.0f;
	float SpecialDelay = 0.0f;
	float SpecialCharging = false;
	bool firstGen = true;
	bool invulnerable = false;
	Vec2DF curCharging;
public:
	GenState state = GenState::Off;
	Player(InversusFramework* framework);
	void RefreshFromData( const GameSceneData& data );
	virtual void Update(float deltaTime) override;
	virtual void Draw(PaintInfo info) override;
	virtual void Deactive() override;
	virtual bool GetActiveState() const override;
	virtual void Reset() override;
	void Regen();
	void AddSpecialBullet(int count);
	void OnInvulnerable();
	void OffInvulnerable();
	bool GetInvulnerable() const;
	bool SwtichInvulnerable();
};
