#pragma once
#include <vector>
#include "NetworkDummyObject.h"
#include "InversusEnum.h"

class SpawnEffect : public ClientObject
{
private:
	float ChargingCount = 0.0f;
	COLORREF color = RGB( 0, 0, 0 );
	Vec2DF mainPosition;
public:
	SpawnEffect( Vec2DF position, COLORREF color );
	virtual void Update( float deltaTime ) override;
	virtual void Draw( PaintInfo info ) override;
};
