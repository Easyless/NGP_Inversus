#include "stdafx.h"
#include "Vec2D.h"
#include "Rect.h"
#include "Transform.h"
#include "InversusFramework.h"

void Transform::Translate( const Vec2DF& vec )
{
	this->Position += vec;
}
