#include "PlayerState.h"

PlayerState::PlayerState() {};

PlayerState::PlayerState(short row, short col)
{
	cPos.row = row;
	cPos.col = col;
	dLastDirection = NORTH;
	bSliding = false;
	bInWeb = false;
	bAlive = true;
	bTeleporting = false;
	sDepth = 0;
	sCoins = 0;
}

PlayerState::~PlayerState() {
	
}

COORD2 PlayerState::NextPositionDelta(DIRECTION d)
{
	COORD2 dp = { 0,0 }; //delta position
	dLastDirection = d;

	switch (d)
	{
	case NORTH: dp.row -= 1; break;
	case EAST:  dp.col += 1; break;
	case SOUTH: dp.row += 1; break;
	case WEST:  dp.col -= 1; break;
	}

	//if (cPos.col + dp.col < 0) return;
	//else if (cPos.col + dp.col > 14) return;

	return dp;
}