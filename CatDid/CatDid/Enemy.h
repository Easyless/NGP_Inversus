#pragma once
#include "GameObject.h"
#include "InversusEnum.h"

class Enemy : public GameObject
{
private:
	float CharginCount = 0;
	bool isSpecial = false;
	float moveSpeed = 30.0f;
	COLORREF color = RGB(56, 182, 214);
public:
	GenState state = GenState::On;
	Enemy(InversusFramework* framework);
	virtual void Update(float deltaTime) override;
	virtual void Draw(PaintInfo info) override;
	virtual bool isCollision(GameObject& other) override;
	virtual void Active();
	virtual void Deactive();
	virtual bool GetActiveState() const override;
	virtual void Reset() override;
	void TurnActive(Vec2DF Position,bool isSpecial = false);
};