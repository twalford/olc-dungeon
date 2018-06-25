#ifndef GAMETYPES_H
#define GAMETYPES_H

#pragma once
#include "TArray.h"

enum orientations { VERTICAL = 0, HORIZONTAL };
typedef orientations ORIENTATION;

enum compass_directions { NORTH = 0, EAST, SOUTH, WEST };
enum entity_types { 
	EMPTY = 0x0000, 
	STONE, 
	WALL, 
	ICE, 
	FIRE, 
	HERO,
	WOOD,
	WEB,
	WEAKSTONE,
	PORTAL_START_BLUE,
	PORTAL_END_BLUE,
	PORTAL_START_RED,
	PORTAL_END_RED,
	PORTAL_START_GREEN,
	PORTAL_END_GREEN,

	POT_STONE = 0x1000,
	POT_STONE_BROKEN,
	POT_ICE,
	POT_ICE_BROKEN
};

typedef entity_types ENTITY;

typedef struct _tile
{
	ENTITY type;
	short e_state;

} TILE, *LPTILE;

typedef compass_directions DIRECTION;
typedef TArray2D<TILE> GAMEBOARD;

#endif