#pragma once
#include <string>
#include "IFrameworkObject.h"
#include "InversusFramework.h"
#include "PaintInfo.h"
#include "Transform.h"

using Tag = std::string;

class InversusFramework;

class NetworkDummyObject : public IFrameworkObject
{
private:
protected:
	bool isActive = false;
public:
	Transform transform;
	NetworkDummyObject( InversusFramework* framework )
		: IFrameworkObject( framework ), transform( *this, framework ) {};
	virtual ~GameObject() = default;
	virtual void Update( float deltaTime ) = 0;
	virtual void Draw( PaintInfo info ) = 0;
	virtual void Destory() { this->isDestroy = true; };
	virtual void Active() { this->isActive = true; };
	virtual void Deactive() { this->isActive = false; };
	virtual bool GetActiveState() const { return this->isActive; }
	virtual void Reset() {};
	//true == StopMove, false == Moving
	virtual bool isCollision( GameObject& other ) { return false; };
};