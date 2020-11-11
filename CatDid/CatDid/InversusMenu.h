#pragma once
#include "LowLevelData.h"
#include "Vec2D.h"
#include "Rect.h"
#include "PaintInfo.h"
#include <vector>
#include <string>
#include <functional>

enum class ButtonState
{
	None,
	Over,
	Click,
};
class InversusButton
{
public:
	bool isActive = false;
	bool isDraw = true;
	Vec2DF Position;
	Vec2DF Size;
	ButtonState state;
	std::wstring text;
	bool isIn(Vec2DF MotherPosition, Vec2DF MousePosition)
	{
		auto rt = RectF(Position + MotherPosition, this->Size.x, this->Size.y);
		return rt.hasIn(MousePosition);
	}
	std::function<void(void)> isClicked;
};

class InversusPlayerInfo
{
public:
	bool isActive = false;
	bool isDraw = true;
	Vec2DF Position;
	Vec2DF Size;
	std::wstring text;
	PlayerWaitState waitState;
};


class InversusFramework;
class InversusMenu
{
private:
	HBITMAP helpBit = 0;
public:
	bool HELP = true;
	bool Invulneralbe = false;
	InversusFramework* framework;
	std::vector<InversusButton> buttons;
	std::vector<InversusPlayerInfo> waits;
	float rotateSpeed = 150.0f;
	float rotate = 0;
	InversusMenu(InversusFramework* framework);
	~InversusMenu() { if (helpBit != 0) { DeleteObject(helpBit); } }
	bool isActive = true;
	Vec2DF Position;
	Vec2DF Size;
	void Draw(PaintInfo info);
	void Update(float deltaTime);
	void MouseInput(Vec2DU MousePos, UINT iMessage);
	bool isBacking = false;
	bool isReing = false;
	void Active() {
		this->isActive = true;
		this->isReing = true;
		this->isBacking = false;
	};
	void Deactive() {
		this->isBacking = true;
		this->isReing = false;
	};
	float ClickDelay = 0;
};