// 작성자 : 김태형
// 작성시작일 : 19.05.02

// 설명:
// 게임 오브젝트들이 가지고 있을 위치, 회전, 사이즈 값을
// 저장하고 관리할 클래스

#pragma once
#include <array>
#include <Windows.h>
#include <unordered_map>
#include <map>
#include <string>

#include "IFrameworkObject.h"
#include "Vec2D.h"
#include "Rect.h"


enum class CollisionTag
{
	Block,
	Player,
	Bullet
};

using Tag = std::string;
class GameObject;

class Transform : public IFrameworkObject
{
	using Ty = float;
	//태그간의 충돌 관계 지정
	GameObject& obj;
	std::map<Tag, bool> checkMap;
public:
	Transform(GameObject& obj, InversusFramework* framework) : obj(obj), IFrameworkObject(framework) {};
	Vec2D<Ty> Position = Vec2D<Ty>{0, 0};
	Vec2D<Ty> Size = Vec2D<Ty>{0,0};
	Vec2D<Ty> Scale = Vec2D<Ty>{ 1,1 };
	Ty Roation = 0.0f;

	void Translate(Vec2D<Ty> vec);
	bool CheckCollision(Transform& other);
	bool GetCheckMap(const Tag& tag);
	void SetCheckMap(const Tag& tag, bool checkCollision);
	//std::array<POINT, 3> GetRotatedRect();


};