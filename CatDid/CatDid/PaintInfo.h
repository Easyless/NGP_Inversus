#pragma once
#include <Windows.h>
#include "Vec2D.h"
// �ۼ��� : ������
// �ۼ������� : 19.05.02

// ����:
// API ��� �� �ʿ��� �������� ������� ����ü

// ������:
// �߰��� API���� �̸� �޾ƿ;� �ϴ� ������ ���ϰ� ���⿡ �߰��ϱ� �ٶ�
struct PaintInfo
{
	HINSTANCE hinst = NULL;
	HDC hdc = NULL; // �׷��� DC : ���� ����DC
	HDC imageDc = NULL; // �̹���DC : �̹��� �ӽ÷� �����صδ� DC
	HDC stretchDc = NULL; // ��Ʈ��ġDC : Ȯ���ϱ� ���� �����صδ� DC
	Vec2DU margin;
	Vec2DU DrawSize;
	float AntiAliasing;
};