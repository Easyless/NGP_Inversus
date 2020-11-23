#pragma once
#include <string>
#include "InversusFramework.h"
#include "PaintInfo.h"
#include "Transform.h"

class BaseObject
{
protected:
	bool isRender = true;
public:
	Transform transform;
	BaseObject() = default;
	virtual ~BaseObject() = default;
	virtual void Update( float deltaTime ) {};
	virtual void Draw( PaintInfo info ) = 0;
};

template <typename DataType>
class RefreshObject : public BaseObject
{
public:
	RefreshObject() = default;
	virtual void RefreshFromData( const DataType& data ) = 0;
};

class ClientObject : public BaseObject
{
protected:
	bool isDestroy = false;
public:
	ClientObject() = default;
	void Destroy();
	bool IsDestroy() const;
};