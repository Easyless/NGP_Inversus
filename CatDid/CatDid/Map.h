#pragma once
#include <vector>
#include "NetworkDummyObject.h"
#include "InversusEnum.h"
#include "ClientSocketManager.h"

class Map : public RefreshObject<GameSceneData>
{
private:
	size_t sizeX;
	size_t sizeY;
	Vec2DF blockSize;
	std::vector<BlockState> map;
public:
	auto begin() { return map.begin(); };
	auto end() { return map.end(); };
	Map( size_t sizeX, size_t sizeY, Vec2DF blockSize );
	auto& GetBlock( int x, int y ) { return map[y * sizeX + x]; };
	std::pair<size_t, size_t> GetPositionIndex( Vec2DF Position ) const;
	virtual void Update( float deltaTime ) override;
	virtual void Draw( PaintInfo info ) override;
	Vec2DF GetBlockPosition( int x, int y );
	std::pair<size_t, size_t> GetSize() const;
	void DrawBlock( PaintInfo info, const Vec2DF& position, const Vec2DF& size, BlockState state );
	Vec2DF GetBlockSize() const;
	virtual void RefreshFromData( const GameSceneData& data ) override;
};