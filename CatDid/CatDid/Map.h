#pragma once
#include <vector>
#include "GameObject.h"
#include "InversusEnum.h"
#include "ClientSocketManager.h"

class Block : public GameObject
{
	static constexpr COLORREF unbreakbleColor = RGB(60,60,60);
	static constexpr COLORREF unbreakblePenColor = RGB(60, 60, 60);
	static constexpr COLORREF breakbleColor = RGB(0, 0, 0);
	static constexpr COLORREF breakblePenColor = RGB(100, 100, 120);
	static constexpr COLORREF offColor = RGB(255, 255, 255);
	static constexpr COLORREF offPenColor = RGB(100, 100, 120);
	static COLORREF GetBrushColor(BlockState state) 
	{
		switch (state)
		{
		case BlockState::BreakableOn: return breakbleColor;
		case BlockState::UnbreakableOn: return unbreakbleColor;
		case BlockState::Off: return offColor;
		default:
			break;
		}
	}
	static COLORREF GetPenColor(BlockState state)
	{
		switch (state)
		{
		case BlockState::BreakableOn: return breakblePenColor;
		case BlockState::UnbreakableOn: return unbreakblePenColor;
		case BlockState::Off: return offPenColor;
		default:
			break;
		}
	}
public:
	bool DrawHatched = false;
	BlockState state = BlockState::Off;
	Block(InversusFramework* framework);
	virtual void Update(float deltaTime) override;
	virtual void Draw(PaintInfo info) override;
	virtual bool isCollision(GameObject& other) override;
	virtual void Active();
	virtual void Deactive();
	BlockState GetState() const { return state; }
};

class Map : public GameObject
{
private:
	size_t sizeX;
	size_t sizeY;
	Vec2DF blockSize;
	std::vector<Block> map;
public:
	auto begin() { return map.begin(); };
	auto end() { return map.end(); };
	Map(InversusFramework* framework, size_t sizeX, size_t sizeY, Vec2DF blockSize);
	Block& GetBlock(int x, int y) { return map[y * sizeX + x]; };
	std::pair<size_t, size_t> GetPositionIndex(Vec2DF Position) const;
	virtual void Update(float deltaTime) override;
	virtual void Draw(PaintInfo info) override;
	std::pair<size_t, size_t> GetSize() const;
	Vec2DF GetBlockSize() const;
	void Reset();
	void Reset(Difficulty dif);
	void RefreshFromData( const GameSceneData& data );
};