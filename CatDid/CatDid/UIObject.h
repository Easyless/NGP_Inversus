#pragma once
#include <vector>
#include "NetworkDummyObject.h"
#include "InversusEnum.h"

class UILifeObject : public RefreshObject<int>
{
	UINT lifeCount = 0;
public:
	UILifeObject();
	virtual void RefreshFromData( const int& data );
	virtual void Update(float deltaTime);
	virtual void Draw(PaintInfo info);
};

class UIScoreObject : public RefreshObject<int>
{
public:
	UIScoreObject();
	virtual void RefreshFromData( const int& data );
	virtual void Update(float deltaTime);
	virtual void Draw(PaintInfo info);
};