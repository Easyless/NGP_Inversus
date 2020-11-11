// 작성자 : 김태형
// 작성시작일 : 19.04.27

// 설명:
// 게임 프레임 워크에 쓰일 기본적인 사각형 RECT 클래스
// 템플릿 적용 (내부 요소 Vec2D도 같은 자료형으로 사용됨)

// 주의점:
// RECT로 변환하기 위해 Windows 헤더 추가
// union 사용

#pragma once

#include <Windows.h>
#include "Vec2D.h"

template <class Ty>
class Rect
{
	using ThisType = Rect<Ty>;
	//using Ty = float;
public:

	Rect() :top(0), left(0), bottom(0), right(0) {};

	//
	Rect(const Ty& left,const Ty& top,const Ty& right, const Ty& bottom) 
		:TL(top,left), RB(right,bottom) 
	{

	};
	//점 두개로 렉트 생성
	Rect(const Vec2D<Ty>& TL, const Vec2D<Ty>& RB) :TL(TL), RB(RB) {};
	//중간점과 가로, 세로 사이즈로 렉트 생성
	Rect(const Vec2D<Ty>& center, Ty width, Ty height) :top(center.y - height/2),left(center.x - width/2),bottom(center.y + height/2),right(center.x + width/2){}
	//중간점과 그 둘레로 정사각형 렉트 생성
	Rect(const Vec2D<Ty>& center, Ty round):Rect(center,round,round){}
	//API RECT to Rect
	Rect(const RECT& rect) : top(rect.top), left(rect.left), bottom(rect.bottom), right(rect.right) {}

	union
	{
		struct
		{
			Ty top;
			Ty left;
		};
		Vec2D<Ty> TL;
	};
	union
	{
		struct
		{
			Ty right;
			Ty bottom;
		};
		Vec2D<Ty> RB;
	};

	Ty Width() const
	{
		return right - left;
	}
	Ty Heigth() const
	{
		return bottom - top;
	}
	Vec2D<Ty> GetSize() const 
	{
		return Vec2D<Ty>(this->Width(), this->Heigth());
	}
	operator RECT() const
	{
		return RECT{
			static_cast<decltype(RECT::left)>(left),
			static_cast<decltype(RECT::top)>(top),
			static_cast<decltype(RECT::right)>(right),
			static_cast<decltype(RECT::bottom)>(bottom)
		};
	}
	//충돌 관련 함수
	//1. Rect와 Rect 가 겹치는지 (isCollision)
	//2. Vec2D를 포함하고 있는지 (isIn)
	//3. Rect를 포함하고 있는지 (isIn)
	bool isCollision(const ThisType& rect, ThisType* intercest = nullptr ) const
	{
		if (intercest != nullptr)
		{
			intercest->left = (this->left > rect.left) ? this->left : rect.left;
			intercest->right = (this->right < rect.right) ? this->right : rect.right;
			intercest->top = (this->top > rect.top) ? this->top : rect.top;
			intercest->bottom = (this->bottom < rect.bottom) ? this->bottom : rect.bottom;
		}
		return (this->left < rect.right && this->right > rect.left && this->top < rect.bottom && this->bottom > rect.top);
	};

	//해당 점이 현재 렉트에 포함되는지
	bool hasIn(const Vec2D<Ty>& point) const
	{
		return (this->left <= point.x && point.x <= this->right && this->top <= point.y && point.y <= this->bottom);
	}
	//해당 영역이 현재 영역에 포함되는지
	bool hasIn(const ThisType& rect) const
	{
		return this->left <= rect.left&& this->top <= rect.top&& this->right >= rect.right&& this->bottom >= rect.right;
	}
	//현재 영역이 해당 영역에 포함되는지
	bool isIn(const ThisType& rect) const
	{
		return rect.hasIn(*this);
	}

	template<typename TTy>
	ThisType operator+(const Vec2D<TTy>& vec)
	{
		ThisType rt = *this;
		rt.TL += vec;
		rt.RB += vec;
		return rt;
	}
	template<typename TTy>
	ThisType operator-(const Vec2D<TTy>& vec)
	{
		ThisType rt = *this;
		rt.TL -= vec;
		rt.RB -= vec;
		return rt;
	}
	template<typename TTy>
	ThisType& operator+=(const Vec2D<TTy>& vec)
	{
		ThisType& rt = *this;
		rt.TL += vec;
		rt.RB += vec;
		return rt;
	}
	template<typename TTy>
	ThisType& operator-=(const Vec2D<TTy>& vec)
	{
		ThisType& rt = *this;
		rt.TL -= vec;
		rt.RB -= vec;
		return rt;
	}
	Vec2D<Ty> GetCenter() const
	{
		auto vec = this->GetSize()/2;
		return vec + Vec2D<Ty>{this->left, this->top};
	}
};

using RectU = Rect<int>;
using RectF = Rect<float>;