#pragma once
#include <Windows.h>
#include "Vec2D.h"
// 작성자 : 김태형
// 작성시작일 : 19.05.02

// 설명:
// API 사용 중 필요한 변수들을 묶어놓는 구조체

// 주의점:
// 추가로 API에서 미리 받아와야 하는 변수는 말하고 여기에 추가하기 바람
struct PaintInfo
{
	HINSTANCE hinst = NULL;
	HDC hdc = NULL; // 그려질 DC : 보통 버퍼DC
	HDC imageDc = NULL; // 이미지DC : 이미지 임시로 저장해두는 DC
	HDC stretchDc = NULL; // 스트레치DC : 확대하기 전에 저장해두는 DC
	Vec2DU margin;
	Vec2DU DrawSize;
	float AntiAliasing;
};