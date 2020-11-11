
#include "stdafx.h"
#include "UIObject.h"
#include "InversusFramework.h"
#include "InversusClasses.h"

#include <string>
#include <memory>
#include <sstream>
static constexpr float MarginX = 10;
static constexpr float MarginY = 10;
UILifeObject::UILifeObject(InversusFramework* framework)
	:GameObject(framework, "UILife")
{
	this->transform.Position = Vec2DF{ 820,70 } -Vec2DF{ MarginX,MarginY };
	this->transform.Size = { 50,50 };
}

void UILifeObject::Update(float deltaTime)
{

}

void UILifeObject::Draw(PaintInfo info)
{
	const std::wstring comment = L"Life";

	HFONT font = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±Ã¼­");
	auto oldPen = (HBRUSH)SelectObject(info.hdc, GetStockObject(WHITE_BRUSH));
	auto oldbr = (HBRUSH)SelectObject(info.hdc, GetStockObject(WHITE_BRUSH));
	auto oldFont = (HFONT)SelectObject(info.hdc, font);
	for (size_t i = 0; i < this->framework->controller->life; i++)
	{
		auto rt = RectF(this->transform.Position + Vec2DF{ (this->transform.Size.x + 10) * i ,0 }, this->transform.Size.x, this->transform.Size.y);
		RoundRect(info.hdc, rt.left, rt.top, rt.right, rt.bottom, 10, 10);
		RECT rtp = rt;
		DrawText(info.hdc, comment.data(), comment.length(), &rtp, DT_CENTER | DT_VCENTER | DT_NOCLIP | DT_SINGLELINE);
	}
	SelectObject(info.hdc, oldFont);
	SelectObject(info.hdc, oldPen);
	SelectObject(info.hdc, oldbr);
	DeleteObject(font);
}

UIScoreObject::UIScoreObject(InversusFramework * framework)
	:GameObject(framework, "UIScore")
{
	this->transform.Size = { 170,300 };
	this->transform.Position = Vec2DF{ 795 + transform.Size.x / 2,110 + transform.Size.y / 2 } -Vec2DF{ MarginX,MarginY };;
}

void UIScoreObject::Update(float deltaTime)
{

}

void UIScoreObject::Draw(PaintInfo info)
{
	const std::wstring ScoreComment = L"Score";
	std::wstringstream ScoreWss;

	HFONT font = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±Ã¼­");
	auto oldbr = (HBRUSH)SelectObject(info.hdc, GetStockObject(WHITE_BRUSH));
	auto oldPen = (HBRUSH)SelectObject(info.hdc, GetStockObject(WHITE_PEN));
	auto oldFont = (HFONT)SelectObject(info.hdc, font);

	auto rt = RectF(this->transform.Position, this->transform.Size.x, this->transform.Size.y);
	RoundRect(info.hdc, rt.left, rt.top, rt.right, rt.bottom, 10, 10);

	ScoreWss.str(L"");
	ScoreWss << this->framework->controller->Score;
	SIZE textSize;
	GetTextExtentPoint(info.hdc, ScoreWss.str().data(), ScoreWss.str().length(), &textSize);
	TextOut(info.hdc, this->transform.Position.x - transform.Size.x / 2 + 5, this->transform.Position.y - transform.Size.y / 2 + 15, ScoreComment.data(), ScoreComment.length());
	TextOut(info.hdc, this->transform.Position.x - transform.Size.x / 2 + 5 + transform.Size.x - textSize.cx - 15, this->transform.Position.y - transform.Size.y / 2 + 15 + textSize.cy + 2, ScoreWss.str().data(), ScoreWss.str().length());
	SelectObject(info.hdc, oldPen);
	DeleteObject(font);
	font = CreateFont(40, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±Ã¼­");
	oldFont = (HFONT)SelectObject(info.hdc, font);
	ScoreWss.str(L"");
	if (this->framework->GetPauseSate())
	{
		std::wstring pausestr = L"Press P to Restart";
		HFONT pfont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±Ã¼­");
		HFONT oldpFont = (HFONT)SelectObject(info.hdc, pfont);
		GetTextExtentPoint(info.hdc, pausestr.data(), pausestr.length(), &textSize);
		TextOut(info.hdc, this->transform.Position.x - textSize.cx / 2, this->transform.Position.y + 45, pausestr.data(), pausestr.length());
		SelectObject(info.hdc, oldpFont);
		DeleteObject(pfont);

		ScoreWss << L"PAUSE";
		GetTextExtentPoint(info.hdc, ScoreWss.str().data(), ScoreWss.str().length(), &textSize);
	}
	else if (this->framework->controller->isCombing)
	{
		ScoreWss << L"X" << this->framework->controller->Combo;
		GetTextExtentPoint(info.hdc, ScoreWss.str().data(), ScoreWss.str().length(), &textSize);
		{
			auto rrt = RectF(
				Vec2DF{ this->transform.Position.x, this->transform.Position.y + textSize.cy / 2 }
			, 96 + 50, 96 + 50);
			SelectObject(info.hdc, GetStockObject(BLACK_BRUSH));
			SelectObject(info.hdc, GetStockObject(NULL_PEN));
			Vec2DF first = { rrt.left + rrt.GetSize().x / 2, rrt.top };
			float rotate = this->framework->controller->curComboTime / this->framework->controller->curMaxComboTime * 360;
			Vec2DF second = first.RotatedPoint(rrt.GetCenter(), rotate - 90);
			Pie(info.hdc, rrt.left, rrt.top, rrt.right, rrt.bottom,
				first.x, first.y,
				second.x, second.y
			);
			SelectObject(info.hdc, GetStockObject(WHITE_BRUSH));
			rrt = RectF(
				Vec2DF{ this->transform.Position.x, this->transform.Position.y + textSize.cy / 2 }
			, 96 + 12, 96 + 12);
			Ellipse(info.hdc, rrt.left, rrt.top, rrt.right, rrt.bottom);
		}
	}
	else 
	{
		ScoreWss << "X0";
		GetTextExtentPoint(info.hdc, ScoreWss.str().data(), ScoreWss.str().length(), &textSize);
	}
	TextOut(info.hdc, this->transform.Position.x - textSize.cx / 2, this->transform.Position.y, ScoreWss.str().data(), ScoreWss.str().length());
	SelectObject(info.hdc, oldFont);
	SelectObject(info.hdc, oldbr);
	SelectObject(info.hdc, oldPen);
	DeleteObject(font);
}
