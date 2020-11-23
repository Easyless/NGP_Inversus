#pragma once
#include "NetworkDummyObject.h"
#include "InversusEnum.h"

class Enemy : public RefreshObject<MobData>
{
private:
	float CharginCount = 0;
	bool isSpecial = false;
public:
	Enemy();
	virtual void RefreshFromData( const MobData& data ) override;
	virtual void Update(float deltaTime) override;
	virtual void Draw(PaintInfo info) override;
};