#pragma once
#include "TArray.h";

enum orientations { VERTICAL = 0, HORIZONTAL };
typedef orientations ORIENTATION;

enum compass_directions { NORTH = 0, EAST, SOUTH, WEST };
enum entity_types { 
	EMPTY = 0, 
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
	PORTAL_END_GREEN
};

typedef entity_types ENTITY;

typedef struct _tile
{
	ENTITY type;
	short e_state;

} TILE, *LPTILE;

typedef compass_directions DIRECTION;
typedef TArray2D<TILE> GAMEBOARD;

COORD2 operator+=(COORD2 const& lhs, COORD2 const& rhs)
{
	COORD2 c = lhs;
	c.col += rhs.col;
	c.row += rhs.row;
	return c;
}

COORD2 operator+(COORD2 const& lhs, COORD2 const& rhs)
{
	return lhs += rhs;
}

bool operator==(COORD2 const& lhs, COORD2 const& rhs)
{
	if (lhs.col == rhs.col && lhs.row == rhs.row)
		return true;
	else
		return false;
}