#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "InversusFramework.h"
#include "InversusClasses.h"
#include "GameObject.h"
#include "InversusMenu.h"
#include <array>
#pragma comment(lib,"msimg32")
InversusFramework* InversusFramework::instance = nullptr;


InversusFramework::InversusFramework()
	:menu(new InversusMenu(this)),container(new InversusContainer(this)), controller(new InversusController(this))
{

}

InversusFramework::~InversusFramework()
{
	delete container;
	delete controller;
	delete menu;
}

void InversusFramework::MenuStart()
{
}

void InversusFramework::Start()
{
	this->container->Start();
	this->controller->Start();
}

void InversusFramework::Create()
{

}

void InversusFramework::Update(float deltaTime)
{
	this->controller->Update(deltaTime);
	container->player.Update(deltaTime);
	for (auto& effect : container->explosionEffect) { effect.Update(deltaTime); }
	for (auto& bullet : container->bullets) { bullet.Update(deltaTime); }
	for (auto& enemy : container->enemys) { enemy.Update(deltaTime); }
	for (auto& drop : container->dropBullets) { drop.Update(deltaTime); }
	container->BlockMap.Update(deltaTime);
	this->menu->Update(deltaTime);

}

void InversusFramework::Draw(PaintInfo info)
{
	container->BlockMap.Draw(info);
	for (auto& bullet : container->bullets) { bullet.Draw(info); }
	for (auto& enemy : container->enemys) { enemy.Draw(info); }
	container->player.Draw(info);
	for (auto& effect : container->explosionEffect) { effect.Draw(info); }
	for (auto& drop : container->dropBullets) { drop.Draw(info); }
}

void InversusFramework::UIDraw(PaintInfo info)
{
	for (auto& uiobj : this->container->UIObject)
	{
		uiobj->Draw(info);
	}
}

void InversusFramework::MenuDraw(PaintInfo info)
{
	this->menu->Draw(info);
}

bool InversusFramework::CheckCollision(GameObject& obj)
{
	bool returnValue = false;

	if (obj.transform.GetCheckMap("Player"))
	{
		auto& targetObj = this->container->player;
		if (targetObj.GetActiveState() && obj.transform.CheckCollision(targetObj.transform)) 
		{
			returnValue |= obj.isCollision(targetObj);
			returnValue |= targetObj.isCollision(obj);
		}
	}
	if (obj.transform.GetCheckMap("Block"))
	{
		auto idx = this->container->BlockMap.GetPositionIndex(obj.transform.Position);
		int centerX = idx.first;
		int centerY = idx.second;
		int idxArr[9][2] =
		{
			{centerX - 1,centerY - 1},{centerX,centerY - 1},{centerX + 1,centerY - 1},
			{centerX - 1,centerY},{centerX,centerY},{centerX + 1,centerY},
			{centerX - 1,centerY + 1},{centerX,centerY + 1},{centerX + 1,centerY + 1}
		};
		for (size_t i = 0; i < 9; i++)
		{
			if (idxArr[i][0] >= 0 && idxArr[i][0] < this->container->BlockMap.GetSize().first
				&& idxArr[i][1] >= 0 && idxArr[i][1] < this->container->BlockMap.GetSize().second)
			{
				auto& targetObj = this->container->BlockMap.GetBlock(idxArr[i][0], idxArr[i][1]);
				if (targetObj.GetActiveState() && obj.transform.CheckCollision(targetObj.transform))
				{
					returnValue |= obj.isCollision(targetObj);
					returnValue |= targetObj.isCollision(obj);
				}
			}
		}
	}
	if (obj.transform.GetCheckMap("Bullet"))
	{
		for (auto& targetObj : this->container->bullets)
		{
			if (targetObj.GetActiveState())
			{
				if (targetObj.GetActiveState() && obj.transform.CheckCollision(targetObj.transform))
				{
					returnValue |= obj.isCollision(targetObj);
					returnValue |= targetObj.isCollision(obj);
				}
			} 
		}
	}
	if (obj.transform.GetCheckMap("Enemy"))
	{
		for (auto& targetObj : this->container->enemys)
		{
			if (targetObj.state == GenState::On)
			{
				if (targetObj.GetActiveState() && obj.transform.CheckCollision(targetObj.transform))
				{
					returnValue |= obj.isCollision(targetObj);
					returnValue |= targetObj.isCollision(obj);
				}
			}
		}
	}
	if (obj.transform.GetCheckMap("DropBullet"))
	{
		for (auto& targetObj : this->container->dropBullets)
		{
			if (targetObj.GetActiveState())
			{
				if (targetObj.GetActiveState() && obj.transform.CheckCollision(targetObj.transform))
				{
					returnValue |= obj.isCollision(targetObj);
					returnValue |= targetObj.isCollision(obj);
				}
			}
		}
	}
	return returnValue;
}

Vec2DF InversusFramework::GetDisplaySize() const
{
	return this->container->GetGameDisplaySize();
}
Vec2DF InversusFramework::GetMargin() const
{
	return this->container->GetMargin();
}

void InversusFramework::MouseInput(Vec2DU MousePos, UINT iMessage)
{
	this->menu->MouseInput(MousePos, iMessage);
}

void InversusFramework::RenewMaxScore(int Score)
{
	this->maxScore = max(Score, this->maxScore);
}
int InversusFramework::GetScore() const
{
	return this->maxScore;
}
