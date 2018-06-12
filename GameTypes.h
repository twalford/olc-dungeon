#pragma once
#include "TArray.h";

enum compass_directions { NORTH = 0, EAST, SOUTH, WEST };
enum entity_types { 
	EMPTY = 0, 
	STONE, 
	WALL, 
	ICE, 
	FIRE, 
	HERO,
	WOOD,
	WEB
};

typedef entity_types ENTITY;

typedef struct _tile
{
	ENTITY type;
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