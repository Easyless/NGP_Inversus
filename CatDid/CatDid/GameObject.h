#pragma once
#include <string>
#include "IFrameworkObject.h"
#include "PaintInfo.h"
#include "Transform.h"

using Tag = std::string;


class InversusFramework;

class GameObject : public IFrameworkObject
{
private:
protected:
	bool isDestroy = false;
	bool isActive = false;
public:
	const Tag tag;
	Transform transform;
	GameObject(InversusFramework* framework, const std::string& tag) : IFrameworkObject(framework), tag(tag), transform(*this, framework) {};
	virtual ~GameObject() = default;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw(PaintInfo info) = 0;
	virtual void Destory() { this->isDestroy = true; };
	virtual void Active() { this->isActive = true; };
	virtual void Deactive() { this->isActive = false; };
	virtual bool GetActiveState() const { return this->isActive; }
	virtual void Reset() {};
	//true == StopMove, false == Moving
	virtual bool isCollision(GameObject& other) { return false; };
};