#include "stdafx.h"
#include "UtillFunction.h"

bool Utill::approximation(float a, float b, float error)
{
	return a + error > b - error && b + error > a - error;
}