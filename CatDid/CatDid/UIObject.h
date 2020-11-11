#pragma once
#include <vector>
#include "GameObject.h"
#include "InversusEnum.h"

class UILifeObject : public GameObject
{
public:
	UILifeObject(InversusFramework* framework);
	virtual void Update(float deltaTime);
	virtual void Draw(PaintInfo info);
	virtual void Destory() { this->isDestroy = true; };
	virtual void Active() { this->isActive = true; };
	virtual void Deactive() { this->isActive = false; };
	virtual bool GetActiveState() const { return this->isActive; }
	//true == StopMove, false == Moving
};

class UIScoreObject : public GameObject
{
public:
	UIScoreObject(InversusFramework* framework);
	virtual void Update(float deltaTime);
	virtual void Draw(PaintInfo info);
	virtual void Destory() { this->isDestroy = true; };
	virtual void Active() { this->isActive = true; };
	virtual void Deactive() { this->isActive = false; };
	virtual bool GetActiveState() const { return this->isActive; }
	//true == StopMove, false == Moving
};