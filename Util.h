#ifndef UTILITY_H
#define UTILITY_H

#include "TArray.h"

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

#endif