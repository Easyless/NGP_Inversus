#pragma once
#include "NetworkDummyObject.h"
//class Bullet : public ClientObject
//{
//	static constexpr float MoveSpeed = 500.0f;
//	static constexpr float SpecialMoveSpeed = 600.0f;
//	static constexpr float GradientNumber = 0.12;
//	static constexpr float GradientMaxLength = 300.0f;
//	static constexpr COLORREF SpecialBulletRColor = 0x37C8;
//	static constexpr COLORREF SpecialBulletGColor = 0xB54A;
//	static constexpr COLORREF SpecialBulletBColor = 0xD52A;
//	static constexpr COLORREF SpecialBulletColor = RGB(56, 182, 214);
//	static constexpr COLORREF BulletColor = RGB(0, 0, 0);
//private:
//	Vec2DF MoveVector;
//	Vec2DF StartPosition;
//	bool isSpecial = false;
//public:
//	Bullet(Vec2DF position, Vec2DF MoveVector );
//	virtual void Update(float deltaTime) override;
//	virtual void Draw(PaintInfo info) override;
//};

class Bullet : public RefreshObject<BulletData>
{
	static constexpr float MoveSpeed = 500.0f;
	static constexpr float SpecialMoveSpeed = 600.0f;
	static constexpr float GradientNumber = 0.12;
	static constexpr float GradientMaxLength = 300.0f;
	static constexpr COLORREF SpecialBulletRColor = 0x37C8;
	static constexpr COLORREF SpecialBulletGColor = 0xB54A;
	static constexpr COLORREF SpecialBulletBColor = 0xD52A;
	static constexpr COLORREF SpecialBulletColor = RGB( 56, 182, 214 );
	static constexpr COLORREF BulletColor = RGB( 0, 0, 0 );
private:
	Vec2DF MoveVector;
	Vec2DF StartPosition;
	bool isSpecial = false;
public:
	Bullet();
	virtual void RefreshFromData( const BulletData& data ) override;
	virtual void Update( float deltaTime ) override;
	virtual void Draw( PaintInfo info ) override;
};

