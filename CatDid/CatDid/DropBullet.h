#pragma once
#include "GameObject.h"

class DropBullet : public GameObject
{
	static constexpr float rotateSpeed = 200.0f;
private:
	int SpecialBulletCount = 0;
	float curRotate = 30.0f;
public:
	DropBullet(InversusFramework* framework);
	virtual void Active();
	virtual void Deactive();
	virtual void Update(float deltaTime) override;
	virtual void Draw(PaintInfo info) override;
	virtual bool isCollision(GameObject& other) override;
	virtual void Reset();
	void ActiveDrop(Vec2DF position, int count);
};
