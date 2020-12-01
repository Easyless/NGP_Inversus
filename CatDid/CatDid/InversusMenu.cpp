#include "stdafx.h"
#include "InversusMenu.h"
#include "InversusFramework.h"
#include "InversusClasses.h"
#include <string>
#include <sstream>
#include "ClientSocketManager.h"
#include "resource.h"

InversusMenu::InversusMenu(InversusFramework* framework)
	:framework(framework)
{
	this->Position = { 0,0 };
	this->Size = { 0.0f,0.0f };

	int i = 0;
	InversusButton but;

	float positionX = 360;
	float positionY = 710;
	but.Position = { positionX, positionY + 75.f*i };
	but.Size = { 220,50 };
	but.text = L"READY";
	but.isActive = true;
	but.isClicked = [&]() 
	{
		this->buttons[0].isActive = false;
		this->buttons[1].isActive = true;
		ClientSocketManager::GetInstance()->SendReqReadyMsg();
	};
	this->buttons.push_back(but);

	but.Position = { positionX, positionY};
	but.Size = { 220,50 };
	but.text = L"CANCEL";
	but.isActive = false;
	but.isClicked = [&]()
	{
		this->buttons[0].isActive = true;
		this->buttons[1].isActive = false;
		ClientSocketManager::GetInstance()->SendCancelReadyMsg();
	};
	this->buttons.push_back( but );

	i = 1;
	but.Position = { positionX + 240.0f * i, positionY };
	but.Size = { 220,50 };
	but.text = L"EXIT";
	but.isActive = true;
	but.isClicked = []() {PostQuitMessage(0); };
	this->buttons.push_back(but);

	InversusPlayerInfo info;
	i = 0;
	positionX = 480;
	positionY = 250;
	float SizeX = 500;
	float SizeY = 100;
	float moveY = SizeY + 20;
	info.Position = { positionX, positionY + moveY * i };
	info.isActive = true;
	info.isDraw = true;
	info.Size = { SizeX, SizeY };
	info.text = L"Player 1";
	info.waitState = PlayerWaitState::WAIT_NOT_CONNECTED;
	this->waits.push_back( info );

	i = 1;
	info.Position = { positionX, positionY + moveY * i };
	info.isActive = true;
	info.isDraw = true;
	info.Size = { SizeX, SizeY };
	info.text = L"Player 2";
	info.waitState = PlayerWaitState::WAIT_NOT_CONNECTED;
	this->waits.push_back( info );

	i = 2;
	info.Position = { positionX, positionY + moveY * i };
	info.isActive = true;
	info.isDraw = true;
	info.Size = { SizeX, SizeY };
	info.text = L"Player 3";
	info.waitState = PlayerWaitState::WAIT_NOT_CONNECTED;
	this->waits.push_back( info );

	i = 3;
	info.Position = { positionX, positionY + moveY * i };
	info.isActive = true;
	info.isDraw = true;
	info.Size = { SizeX, SizeY };
	info.text = L"Player 4";
	info.waitState = PlayerWaitState::WAIT_NOT_CONNECTED;
	this->waits.push_back( info );

	ClientSocketManager::GetInstance()->recvWaitDataFunction =
		[this](const WaitRoomData& roomData)
	{
		for ( size_t i = 0; i < 4; i++ )
		{
			this->waits[i].waitState = roomData.playerWaitStates[i];
		}
	};

	ClientSocketManager::GetInstance()->recvAllReadyFunction =
		[this]( )
	{
		this->Deactive(); 
		this->framework->Start();
	};


	////난이도 조절
	//i = 0;
	//but.Position = { 750, 500 + 75.f * i };
	//but.Size = { 220,50 };
	//but.text = L"EASY";
	//but.isActive = false;
	//but.isClicked = [&]() {this->Deactive(); this->framework->controller->Reset(Difficulty::Normal); this->framework->Start(); };
	//this->buttons.push_back(but);

	//i = 1;
	//but.Position = { 750, 500 + 75.f * i };
	//but.Size = { 220,50 };
	//but.text = L"HARD";
	//but.isActive = false;
	//but.isClicked = [&]() {this->Deactive(); this->framework->controller->Reset(Difficulty::Hard); this->framework->Start(); };
	//this->buttons.push_back(but);

	//i = 2;
	//but.Position = { 750, 500 + 75.f * i };
	//but.Size = { 220,50 };
	//but.text = L"EXTREME";
	//but.isActive = false;
	//but.isClicked = [&]() {this->Deactive(); this->framework->controller->Reset(Difficulty::Extreme); this->framework->Start(); };
	//this->buttons.push_back(but);

	//but.Position = Vec2DF{ 840,343 };
	//but.Size = { 30,30 };
	//but.text = L"";
	//but.isActive = true;
	//but.isClicked = [&]() 
	//{
	//	Invulneralbe = this->framework->container->player.SwtichInvulnerable();
	//};
	//this->buttons.push_back(but);
}

void InversusMenu::Draw(PaintInfo info)
{
	if (helpBit == 0)
	{
		helpBit = LoadBitmap(info.hinst, MAKEINTRESOURCE(IDB_BITMAP1));
	}
	const std::wstring title = L"INVERSUS";	
	const std::wstring creator = L"2016182010 김태형, 2016180016 김현진 | 네트워크게임프로그래밍";
	std::wstringstream wss;
	if (this->Size.x == 0)
	{
		this->Size = info.DrawSize;
	}
	if (this->isActive)
	{
		auto oldbr = (HBRUSH)SelectObject(info.hdc, GetStockObject(BLACK_BRUSH));
		Rectangle(info.hdc, Position.x, 0, Position.x + info.DrawSize.x, info.DrawSize.y);


		//타이틀 텍스트
		{
			//HFONT font = CreateFont(200, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Agency FB");
			//HFONT font = CreateFont(200, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Bahnschrift");
			HFONT font = CreateFont(170, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"MS UI Gothic");
			
			auto oldFont = (HFONT)SelectObject(info.hdc, font);
			auto oldPen = (HBRUSH)SelectObject(info.hdc, GetStockObject(WHITE_PEN));
			SelectObject(info.hdc, GetStockObject(WHITE_BRUSH));
			SetTextColor(info.hdc, RGB(255, 255, 255));
			SetBkMode(info.hdc, TRANSPARENT);
			
			float moveX = -30;
			float moveY = -215;

			Rectangle(info.hdc, this->Position.x + info.DrawSize.x * 1 / 10 - 50 + moveX, this->Position.y + info.DrawSize.y * 1 / 4 + 172 + moveY, this->Position.x + info.DrawSize.x * 1 / 10 + 765 + moveX, this->Position.y + info.DrawSize.y * 1 / 4 + 12 + 172 + moveY );
			TextOut(info.hdc, this->Position.x + info.DrawSize.x * 1 / 10 - 50 + moveX, this->Position.y + info.DrawSize.y * 1 / 4 + 20 + moveY,
				title.data(), title.length());
			SetBkMode(info.hdc, OPAQUE);
			SelectObject(info.hdc, oldbr);
			SelectObject(info.hdc, oldPen);
			SelectObject(info.hdc, oldFont);
			DeleteObject(font);
		}
		//제작자
		{
			HFONT font = CreateFont(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, 0, 0, 0, 0, L"돋움");

			auto oldFont = (HFONT)SelectObject(info.hdc, font);
			SetTextColor(info.hdc, RGB(125, 125, 125));
			SetBkMode(info.hdc, TRANSPARENT);
			TextOut(info.hdc, this->Position.x + this->Size.x/2 - 100 + 230, this->Position.y + this->Size.y - 20, creator.data(), creator.length());
			SetBkMode(info.hdc, OPAQUE);
			SelectObject(info.hdc, oldFont);
			DeleteObject(font);
		}
		//Button
		{
			for (auto& button : buttons)
			{
				if (button.isActive && button.isDraw)
				{
					HFONT font = CreateFont(40, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"MS UI Gothic");
					auto oldFont = (HFONT)SelectObject(info.hdc, font);
					auto oldPen = (HBRUSH)SelectObject(info.hdc, GetStockObject(WHITE_PEN));
					HBRUSH oldBrush;
					HBRUSH newBrush;
					if (button.state == ButtonState::Over)
					{
						newBrush = CreateSolidBrush(RGB(180, 180, 180));
						oldBrush = (HBRUSH)SelectObject(info.hdc, newBrush);
					}
					else if (button.state == ButtonState::Click)
					{
						newBrush = CreateSolidBrush(RGB(120, 120, 120));
						oldBrush = (HBRUSH)SelectObject(info.hdc, newBrush);
					}
					else 
					{
						newBrush = CreateSolidBrush(RGB(255, 255, 255));
						oldBrush = (HBRUSH)SelectObject(info.hdc, newBrush);
					}

					SetTextColor(info.hdc, RGB(0, 0, 0));
					SetBkMode(info.hdc, TRANSPARENT);
					auto rt = RectF(button.Position + this->Position, button.Size.x, button.Size.y);
					RECT rrt = rt;
					Rectangle(info.hdc,rt.left,rt.top,rt.right,rt.bottom);
					DrawText(info.hdc, button.text.data(), button.text.length(), &rrt, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
					SetBkMode(info.hdc, OPAQUE);
					SelectObject(info.hdc, oldBrush);
					SelectObject(info.hdc, oldbr);
					SelectObject(info.hdc, oldPen);
					SelectObject(info.hdc, oldFont);
					DeleteObject(newBrush);
					DeleteObject(font);
				}
			}
		}

		//PlayerInfo
		{
			for ( auto& wait : waits )
			{
				if ( wait.isActive && wait.isDraw )
				{
					HFONT font = CreateFont( 40, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"MS UI Gothic" );
					auto oldFont = (HFONT)SelectObject( info.hdc, font );
					auto oldPen = (HBRUSH)SelectObject( info.hdc, GetStockObject( WHITE_PEN ) );
					HBRUSH oldBrush = 0;
					HBRUSH newBrush = 0;

					newBrush = CreateSolidBrush( RGB( 255, 255, 255 ) );
					oldBrush = (HBRUSH)SelectObject( info.hdc, newBrush );

					SetTextColor( info.hdc, RGB( 0, 0, 0 ) );
					SetBkMode( info.hdc, TRANSPARENT );
					auto rt = RectF( wait.Position + this->Position, wait.Size.x, wait.Size.y );
					auto rt2 = RectF( wait.Position + this->Position + Vec2DF{ 0,-25 }, wait.Size.x - 30, wait.Size.y );
					auto rt3 = RectF( wait.Position + this->Position + Vec2DF{ 0,+25 }, wait.Size.x - 30, wait.Size.y );
					RECT rrt = rt2;
					RECT rrt2 = rt3;
					Rectangle( info.hdc, rt.left, rt.top, rt.right, rt.bottom );
					DrawText( info.hdc, wait.text.data(), wait.text.length(), &rrt, DT_SINGLELINE | DT_VCENTER | DT_LEFT );

					std::wstring statusText;
					switch ( wait.waitState )
					{
					case PlayerWaitState::WAIT_CONNECTED_NORMAL:
						statusText = L"WAIT";
						break;
					case PlayerWaitState::WAIT_NOT_CONNECTED:
						statusText = L"EMPTY";
						break;
					case PlayerWaitState::WAIT_READY:
						statusText = L"READY";
						break;
					}
					DrawText( info.hdc, statusText.c_str(), statusText.length(), &rrt2, DT_SINGLELINE | DT_VCENTER | DT_RIGHT );

					SetBkMode( info.hdc, OPAQUE );
					SelectObject( info.hdc, oldBrush );
					SelectObject( info.hdc, oldbr );
					SelectObject( info.hdc, oldPen );
					SelectObject( info.hdc, oldFont );
					DeleteObject( newBrush );
					DeleteObject( font );
				}
			}
		}


		//타이틀 플레이어
		//{
		//	Vec2DF cubePos = Vec2DF{840,343} + this->Position;
		//	Vec2DF cubeSize = {30,30};
		//	HPEN hPen = 0;
		//	HBRUSH hBrush = 0;
		//	if (Invulneralbe)
		//	{
		//		hPen = CreatePen(PS_SOLID, 7, RGB(255, 201, 14));
		//		hBrush = CreateSolidBrush(RGB(255, 201, 14));
		//	}
		//	else 
		//	{
		//		hPen = CreatePen(PS_SOLID, 7, RGB(255, 255, 255));
		//		hBrush = CreateSolidBrush(RGB(255, 255, 255));
		//	}
		//	auto oldPen = (HPEN)SelectObject(info.hdc, hPen);
		//	auto oldBrush = (HBRUSH)SelectObject(info.hdc, hBrush);
		//	auto rt = RectF(cubePos, cubeSize.x, cubeSize.y);
		//	RoundRect(info.hdc, rt.left, rt.top, rt.right, rt.bottom, 11, 11);
		//	Ellipse(info.hdc, rt.left, rt.top, rt.right, rt.bottom);

		//	size_t i = 0;
		//	for (i = 0; i < 6; i++)
		//	{
		//		COLORREF color = RGB(0, 0, 0);
		//		HPEN hBulletPen = CreatePen(PS_SOLID, 1, color);
		//		auto oldBulletPen = (HPEN)SelectObject(info.hdc, hBulletPen);
		//		HBRUSH hBulletBrush = CreateSolidBrush(color);
		//		auto oldBulletBrush = (HBRUSH)SelectObject(info.hdc, hBulletBrush);
		//		auto bulletPos = (cubePos + Vec2DF{ 0,45.f * cubeSize.y / 130 }).RotatedPoint(cubePos, i * 60 + rotate);
		//		auto bulletRt = RectF(bulletPos, cubeSize.x * 0.25, cubeSize.y * 0.25);
		//		Ellipse(info.hdc, bulletRt.left, bulletRt.top, bulletRt.right, bulletRt.bottom);
		//		SelectObject(info.hdc, oldBulletBrush);
		//		SelectObject(info.hdc, oldBulletPen);
		//		DeleteObject(hBulletBrush);
		//		DeleteObject(hBulletPen);
		//	}
		//	SelectObject(info.hdc, oldPen);
		//	SelectObject(info.hdc, oldBrush);
		//	DeleteObject(hBrush);
		//	DeleteObject(hPen);
		//}

	}
	{
	// 점수 텍스트
		//HFONT font = CreateFont(40, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"MS UI Gothic");
		//wss << " Max Score : " << this->framework->GetScore();
		//auto oldFont = (HFONT)SelectObject(info.hdc, font);
		//auto oldPen = (HBRUSH)SelectObject(info.hdc, GetStockObject(WHITE_PEN));
		//SelectObject(info.hdc, GetStockObject(WHITE_BRUSH));
		//SetTextColor(info.hdc, RGB(255, 255, 255));
		//SetBkMode(info.hdc, TRANSPARENT);
		//TextOut(info.hdc, this->Position.x + info.DrawSize.x * 1 / 10 - 50, this->Position.y + info.DrawSize.y * 6 / 8 + 20, wss.str().data(), wss.str().length());
		//SetBkMode(info.hdc, OPAQUE);
		//SelectObject(info.hdc, oldPen);
		//SelectObject(info.hdc, oldFont);
		//DeleteObject(font);
	}
	{
		//HELP
		//if (!HELP)
		//{
		//	//792 434
		//	SelectObject(info.imageDc, helpBit);
		//	BitBlt(info.hdc, this->Position.x + 20, this->Position.y+400, 594, 326, info.imageDc, 0, 0, SRCCOPY);
		//}
	}
}

void InversusMenu::Update(float deltaTime)
{
	if (this->isActive)
	{
		ClientSocketManager::GetInstance()->OnRecvWaitData();
		ClientSocketManager::GetInstance()->OnRecvAllReady();
		this->rotate += deltaTime * rotateSpeed;
		if (ClickDelay > 0)
		{
			ClickDelay -= deltaTime;
		}
		else 
		{
			ClickDelay = 0.0f;
		}
		if (isBacking)
		{
			this->Position += {-600.0f * deltaTime, 0};
			if (this->Position.x + this->Size.x  < 0)
			{
				this->buttons[0].isActive = true;
				this->buttons[1].isActive = false;
				this->buttons[2].isActive = true;
				isBacking = false;
				isReing = true;
				isActive = false;
				HELP = false;
			}
		}
		else if (isReing)
		{
			this->Position += {600.0f * deltaTime, 0};
			if (this->Position.x  >= 0)
			{
				this->Position = { 0,0 };
				ClientSocketManager::GetInstance()->RestartConnection();
				isReing = false;
			}
		}
	}
}

void InversusMenu::MouseInput(Vec2DU MousePos, UINT iMessage)
{
	for (auto& button : buttons )
	{
		if (button.isActive)
		{
			switch (iMessage)
			{
			case WM_LBUTTONUP:
				if (button.isIn(this->Position, MousePos))
				{
					if (button.state == ButtonState::Click && ClickDelay <= 0.0f)
					{
						button.isClicked();
						ClickDelay = 0.15f;
					}
					button.state = ButtonState::Over;
				}
				else
				{
					button.state = ButtonState::None;
				}
			case WM_MOUSEMOVE:
			{
				if (button.isIn(this->Position, MousePos))
				{
					if (button.state == ButtonState::Click)
					{
					}
					else 
					{
						button.state = ButtonState::Over;
					}
				}
				else 
				{
					button.state = ButtonState::None;
				}
			}
			break;
			case WM_LBUTTONDOWN:
			{
				if (button.isIn(this->Position, MousePos))
				{
					button.state = ButtonState::Click;
				}
				else
				{
					button.state = ButtonState::None;
				}
			}
			break;
			default:
				break;
			}
		}
	}
}
