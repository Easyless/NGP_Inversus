#pragma once
enum class BlockState
{
	Creating, BreakableOn, Breaking, Off , UnbreakableOn
};
enum class GenState
{
	Gen, On, Dead, Off
};
enum class Difficulty
{
	Normal,
	Hard,
	Extreme
};