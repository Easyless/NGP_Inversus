#include "stdafx.h"
//#include "UtillFunction.h"
//#include "Vec2D.h"
//#include "Rect.h"
//#include "CatFrameWork.h"
//#include "GameObject.h"
//#include "InversusClasses.h"
//#include "InversusFramework.h"
//#include "InversusMenu.h"
//void InversusController::Reset(Difficulty diff)
//{
//	//난이도 변수 초기화
//
//	//게임변수 초기화
//	Score = 0;
//	Combo = 0;
//	life = 3;
//	curComboTime = 0;
//	curEnemyGenTime = maxGenTime;
//	curMaxComboTime = maxComboTime;
//	curEnemyGenCount = 0;
//	curPlayerGenCount = 0;
//	explosionNum = 0;
//	curDiff = diff;
//	this->framework->container->Reset(curDiff);
//	this->framework->SetPauseSate(false);
//}
//void InversusController::Start()
//{
//	this->state = GameState::On;
//	this->framework->container->player.Regen();
//	this->framework->SetPauseSate(false);
//
//}
//void InversusController::Update(float deltaTime)
//{
//	if (!this->framework->GetPauseSate())
//	{
//		if (this->explosionNum > 0)
//		{
//			this->explosionNum -= deltaTime;
//		}
//		else
//		{
//			this->explosionNum = 0;
//		}
//		if (this->state == GameState::On)
//		{
//			if (curEnemyGenCount > curEnemyGenTime)
//			{
//				curEnemyGenCount = 0.0f;
//				this->AddEnemy();
//				curEnemyGenTime = Utill::clamp(curEnemyGenTime * 0.75f, minGenTime, maxGenTime);
//			}
//			else
//			{
//				curEnemyGenCount += deltaTime;
//			}
//
//			if (isCombing)
//			{
//				if (curComboTime > curMaxComboTime)
//				{
//					BreakCombo();
//				}
//				else
//				{
//					curComboTime += deltaTime;
//				}
//			}
//
//
//			if (curPlayerGenCount > 0)
//			{
//				curPlayerGenCount -= deltaTime;
//			}
//			else
//			{
//				if (regening)
//				{
//					life -= 1;
//					this->framework->container->player.Regen();
//					regening = false;
//				}
//				curPlayerGenCount = 0;
//			}
//		}
//	}
//}
//
//void InversusController::AddEnemy()
//{
//	Vec2DF GenPosition;
//	RectF PlayerRect = RectF(
//		this->framework->container->player.transform.Position,
//		this->framework->GetDisplaySize().x / 2,
//		this->framework->GetDisplaySize().y / 2
//	);
//	while (true)
//	{
//		GenPosition = Vec2DF{
//			static_cast<float>(rand() % static_cast<int>(this->framework->GetDisplaySize().x)),
//			static_cast<float>(rand() % static_cast<int>(this->framework->GetDisplaySize().y))
//		};
//		if (!PlayerRect.hasIn(GenPosition))
//		{
//			break;
//		}
//	}
//	bool isSpecial = false;
//	if (this->Combo > 25 && (rand() % 10 == 0))
//	{
//		isSpecial = true;
//	}
//	this->framework->container->AddEnemy(GenPosition, isSpecial);
//}
//
//void InversusController::PostPlayerDead()
//{
//	BreakCombo();
//	if (life > 0)
//	{
//		curPlayerGenCount = playerRegenTime;
//		regening = true;
//	}
//	else 
//	{
//		PostGameOver();
//	}
//}
//
//void InversusController::PostGameOver()
//{
//	this->BreakCombo();
//	this->state = GameState::Over;
//	this->framework->menu->Active();
//	//this->framework->RenewMaxScore(this->Score);
//}
//void InversusController::PostExplosion()
//{
//	explosionNum = Utill::clamp((explosionNum+1)*1.2f,0.0f,3.0f);
//}
//void InversusController::PostAddLife()
//{
//}
//void InversusController::PostAddScore(bool isSpecial)
//{
//	this->Score = Utill::clamp(this->Score +10 + 500* isSpecial + 10.f * (Combo),0.f, ScoreMax);
//	this->Combo = Utill::clamp(this->Combo + 1, 0, 999);
//	if (this->Combo != 0 && (this->Combo % 100 == 0)) {this->life = Utill::clamp(this->life + 1, 0, 3);}
//	this->curMaxComboTime = Utill::clamp(this->curMaxComboTime* 0.97f, minComboTime, maxComboTime);
//	this->curComboTime = 0;
//	isCombing = true;
//}
//void InversusController::BreakCombo()
//{
//	this->Combo = 0;
//	this->curComboTime = 0;
//	this->curMaxComboTime = this->maxComboTime;
//	isCombing = false;
//}
//void InversusController::Pause(bool isRestart)
//{
//	this->framework->SetPauseSate(!isRestart);
//}