#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#include "GameTypes.h"

class PlayerState
{
	public:
		COORD2		cPos;
		DIRECTION	dLastDirection;
		bool		bSliding;
		bool		bAlive;
		bool		bInWeb;
		bool		bTeleporting;
		short		sDepth;
		short		sCoins;

		~PlayerState();
		PlayerState();
		PlayerState(short row, short col);
		COORD2 NextPositionDelta(DIRECTION d);
};

#endif