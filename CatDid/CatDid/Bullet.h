#pragma once
#include "GameObject.h"
class Bullet : public GameObject
{
	static constexpr float MoveSpeed = 500.0f;
	static constexpr float SpecialMoveSpeed = 600.0f;
	static constexpr float GradientNumber = 0.12;
	static constexpr float GradientMaxLength = 300.0f;
	static constexpr COLORREF SpecialBulletRColor = 0x37C8;
	static constexpr COLORREF SpecialBulletGColor = 0xB54A;
	static constexpr COLORREF SpecialBulletBColor = 0xD52A;
	static constexpr COLORREF SpecialBulletColor = RGB(56, 182, 214);
	static constexpr COLORREF BulletColor = RGB(0, 0, 0);
private:
	Vec2DF MoveVector;
	Vec2DF StartPosition;
	bool isSpecial = false;
public:
	Bullet(InversusFramework* framework);
	virtual void Update(float deltaTime) override;
	virtual void Draw(PaintInfo info) override;
	virtual bool isCollision(GameObject& other) override;
	virtual void Active() { GameObject::Active();};
	virtual void Deactive() { GameObject::Deactive(); };
	virtual void Reset() override;
	void TurnActive(Vec2DF position, Vec2DF MoveVector, bool isSpecial = false);
};

