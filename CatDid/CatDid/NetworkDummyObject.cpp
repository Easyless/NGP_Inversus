#include "stdafx.h"
#include "NetworkDummyObject.h"

void ClientObject::Destroy()
{
	this->isDestroy = true;
	this->isRender = false;
}

bool ClientObject::IsDestroy() const
{
	return this->isDestroy;
}
