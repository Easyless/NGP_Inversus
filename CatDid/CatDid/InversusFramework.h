#pragma once
#include "PaintInfo.h"
#include "GameObject.h"
#include <vector>


class InversusContainer;
class InversusController;
class InversusMenu;
class InversusFramework
{
private:
	int maxScore = 0;
	bool isPause = false;
public:
	InversusMenu* menu = nullptr;
	InversusContainer* container = nullptr;
	InversusController* controller = nullptr;
	InversusFramework();
	~InversusFramework();
	static InversusFramework* instance;
	void MenuStart();
	void Start();
	void Create();
	void Update(float deltaTime);
	void Draw(PaintInfo info);
	void UIDraw(PaintInfo info);
	void MenuDraw(PaintInfo info);
	bool CheckCollision(GameObject& obj);
	Vec2DF GetDisplaySize() const;
	Vec2DF GetMargin() const;
	void MouseInput(Vec2DU MousePos, UINT iMessage);
	void RenewMaxScore(int Score);
	int GetScore() const;
	bool GetPauseSate() const { return isPause; }
	void SetPauseSate(bool pause) { isPause = pause; }
};