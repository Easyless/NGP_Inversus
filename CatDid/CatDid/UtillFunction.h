#pragma once
#include <Windows.h>
namespace Utill
{
	template <typename Ty>
	Ty clamp(Ty target, Ty _min, Ty _max)
	{
		return min(max(_min, target), _max);
	}

	template <typename Ty>
	Ty CircularNumber(Ty& target, Ty _min, Ty _max)
	{
		if (target < _min) target += _max;
		if (target > _max) target %= _max;
		return target;
	}

	bool approximation(float a, float b, float error = 5);
}