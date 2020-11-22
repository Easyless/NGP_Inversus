#pragma once
#include "PaintInfo.h"
#include "GameObject.h"
#include "ClientSocketManager.h"
#include "InversusNetworkController.h"
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
	ClientSocketManager* socket = nullptr;
	InversusMenu* menu = nullptr;
	InversusContainer* container = nullptr;
	InversusNetworkController* controller = nullptr;
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
	Vec2DF GetDisplaySize() const;
	Vec2DF GetMargin() const;
	void MouseInput(Vec2DU MousePos, UINT iMessage);
	bool GetPauseSate() const { return isPause; }
	void SetPauseSate(bool pause) { isPause = pause; }
};