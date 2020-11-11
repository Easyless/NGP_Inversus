#pragma once
#include <vector>
#include "GameObject.h"
#include "InversusEnum.h"


struct _ExplosionEffect
{
	Vec2DF position;
	float maxScale = 40.0f;
	float startTime = 0.0f;
	float duration = 0.2f;
	float scale = 0.1f;
	void Draw(PaintInfo info,float thisTime, Vec2DF mainPosition, COLORREF color);
};
class Explosion : public GameObject
{
private:
	float time = 0.0f;
	float endTime = 0.0f;
	COLORREF color = RGB(0, 0, 0);
	Vec2DF mainPosition;
	bool effecting = true;
	bool checkCollision = true;
public:
	std::vector<_ExplosionEffect> subEffect;
	GenState state = GenState::Off;
	Explosion(InversusFramework* framework);
	virtual void Active();
	virtual void Deactive();
	virtual void Update(float deltaTime) override;
	virtual void Draw(PaintInfo info) override;
	virtual bool isCollision(GameObject& other) override;
	void TurnActive(Vec2DF position, COLORREF color,bool CheckCollision = true);
};
