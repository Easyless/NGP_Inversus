#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "CatFrameWork.h"
#include "InversusFramework.h"
#include "InversusClasses.h"
#include "GameObject.h"
#include "InversusMenu.h"
#include <array>
#include "ClientSocketManager.h"
#pragma comment(lib,"msimg32")
InversusFramework* InversusFramework::instance = nullptr;


InversusFramework::InversusFramework()
	:menu(new InversusMenu(this)),container(new InversusContainer(this)), controller(new InversusNetworkController(this))
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
	this->socket = ClientSocketManager::GetInstance();
	this->controller->InitlizeWithSocket( this->socket );
}

void InversusFramework::Update(float deltaTime)
{
	this->controller->Update(deltaTime);
	//container->player.Update(deltaTime);
	//for (auto& effect : container->explosionEffect) { effect.Update(deltaTime); }
	//for (auto& bullet : container->bullets) { bullet.Update(deltaTime); }
	//for (auto& enemy : container->enemys) { enemy.Update(deltaTime); }
	//for (auto& drop : container->dropBullets) { drop.Update(deltaTime); }
	container->BlockMap.Update(deltaTime);
	this->menu->Update(deltaTime);

}

void InversusFramework::Draw(PaintInfo info)
{
	container->BlockMap.Draw(info);
	for (auto& bullet : container->bullets) { bullet.Draw(info); }
	for (auto& enemy : container->enemys) { enemy.Draw(info); }
	for ( auto& p : container->player ) { p.Draw( info ); }
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
