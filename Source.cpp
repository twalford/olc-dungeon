#include <iostream>
#include "TArray.h"
#include "olcConsoleGameEngine.h";
using namespace std;

#pragma region
enum compass_directions { NORTH = 0, EAST, SOUTH, WEST };
enum entity_types { EMPTY = 0, FLOOR, WALL, ICE, FIRE, HERO };

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

#pragma endregion GAME TYPES

class DungeonModule
{
public:
	DungeonModule() 
	{

	}

	DungeonModule(short w, short h)
	{
		Create(w, h);
	}

	short width = 0;
	short height = 0;

private:
	ENTITY *m_Tiles = nullptr;

	void Create(short w, short h)
	{
		width = w;
		height = h;
		m_Tiles = new entity_types[w*h];
		for (int i = 0; i < w*h; i++)
			m_Tiles[i] = EMPTY;
	}

public:
	void SetTile(short x, short y, ENTITY t)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return;
		else
			m_Tiles[y * width + x] = t;
	}

	ENTITY GetTile(short x, short y)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return EMPTY; // maybe change this to return null or some error tile.
		else
			return m_Tiles[y * width + x];
	}

};

class Dungeon : public olcConsoleGameEngine
{
public:
	Dungeon()
	{

	}

private:
	GAMEBOARD m_xBoard;

	olcSprite *spriteHERO	= nullptr;
	olcSprite *spriteFLOOR	= nullptr;
	olcSprite *spriteICE	= nullptr;
	olcSprite *spriteFIRE	= nullptr;
	olcSprite *spriteEMPTY	= nullptr;

	const short TILE_SIZE = 8;
	pair<short, short> m_sBufOffset;

	// Player states
	COORD2 m_cPos;
	DIRECTION m_dLastDirection;
	bool m_bSliding;
	bool m_bAlive;
	short m_sDepth;

	void ModuleToBoard(int x, int y, ENTITY t)
	{
		if (x >= 0 && x < m_xBoard.rows() && y >= 0 && y < m_xBoard.cols()) {
			m_xBoard(x, y).type = t;
		}
	}
	void DrawModule(short x, short y, DungeonModule *dm)
	{
		if (dm == nullptr)
			return;

		for (int i = 0; i < dm->width; i++)
		{
			for (int j = 0; j < dm->height; j++)
			{
				ModuleToBoard(x + i, y + j, dm->GetTile(i, j));
			}
		}
	}

	void CreateModuleStarting(DungeonModule *dm)
	{
		// top row
		dm->SetTile(0, 0, FLOOR);
		dm->SetTile(0, 1, FLOOR);
		dm->SetTile(0, 2, FLOOR);
		// middle row
		dm->SetTile(1, 0, FLOOR);
		dm->SetTile(1, 1, FLOOR);
		dm->SetTile(1, 2, FLOOR);
		// last row
		dm->SetTile(2, 0, FLOOR);
		dm->SetTile(2, 1, FLOOR);
		dm->SetTile(2, 2, FLOOR);
	}
	void CreateModuleTesting(DungeonModule *dm)
	{
		// top row
		dm->SetTile(0, 0, FLOOR);
		dm->SetTile(0, 1, FLOOR);
		dm->SetTile(0, 2, FLOOR);
		dm->SetTile(0, 3, FLOOR);
		dm->SetTile(0, 4, FLOOR);
		// second row
		dm->SetTile(1, 0, FLOOR);
		dm->SetTile(1, 1, ICE);
		dm->SetTile(1, 2, FLOOR);
		dm->SetTile(1, 3, FIRE);
		dm->SetTile(1, 4, FLOOR);
		// third row
		dm->SetTile(2, 0, FLOOR);
		dm->SetTile(2, 1, ICE);
		dm->SetTile(2, 2, FLOOR);
		dm->SetTile(2, 3, FIRE);
		dm->SetTile(2, 4, FLOOR);
		// fourth row
		dm->SetTile(3, 0, FLOOR);
		dm->SetTile(3, 1, ICE);
		dm->SetTile(3, 2, FLOOR);
		dm->SetTile(3, 3, FIRE);
		dm->SetTile(3, 4, FLOOR);
		// last row
		dm->SetTile(4, 0, FLOOR);
		dm->SetTile(4, 1, FLOOR);
		dm->SetTile(4, 2, FLOOR);
		dm->SetTile(4, 3, FLOOR);
		dm->SetTile(4, 4, FLOOR);
	}

	void MovePlayer(DIRECTION d)
	{
		COORD2 dp = {0,0}; //delta position
		m_dLastDirection = d;

		switch (d)
		{
		case NORTH: dp.row -= 1; break;
		case EAST:  dp.col += 1; break;
		case SOUTH: dp.row += 1; break;
		case WEST:  dp.col -= 1; break;
		}
		
		// Keep the player within the boundry. (left and right)
		if (m_cPos.col + dp.col < 0) return;
		else if (m_cPos.col + dp.col > m_xBoard.cols() - 1) return;

		OnPlayerStep(dp);
		ShiftBoard(d);
		UpdateScreen();
	}
	void OnPlayerStep(COORD2 dp)
	{
		ENTITY nextTile = m_xBoard(m_cPos + dp).type;
		m_bSliding = false;
		switch (nextTile)
		{
		case WALL:
			return; // don't step
			break;

		case EMPTY:
		case FIRE:
			m_bAlive = false; // dead
			break;

		case ICE:
			m_bSliding = true;
			break;

		default:
			break;
		}

		// Update the position
		COORD2 newPos = m_cPos + dp;
		// Update the depth
		m_sDepth -= dp.row;
		//Payers row position should never change.
		newPos.row = 18;
		m_cPos = newPos;

	}
	void ShiftBoard(DIRECTION d)
	{
		switch (d)
		{
			// Shift board down (player goes north)
		case NORTH:
			for (int r = m_xBoard.rows() - 1; r >= 0; r--)
			{
				for (int c = m_xBoard.cols() - 1; c >= 0; c--)
				{
					if (r > 0)
						m_xBoard(r, c).type = m_xBoard(r - 1, c).type;
					else
						m_xBoard(r, c).type = EMPTY;
				}
			}
			break;
			
			// Shift board up (player goes south)
		case SOUTH:
			for (int r = 0; r < m_xBoard.rows(); r++)
			{
				for (int c = 0; c < m_xBoard.cols(); c++)
				{
					if (r < m_xBoard.rows() - 1)
						m_xBoard(r, c).type = m_xBoard(r + 1, c).type;
					else
						m_xBoard(r, c).type = EMPTY;
				}
			}

			break;
		default:
			break;
		}
	}
	void UpdateScreen()
	{
		for (int r = 0; r < m_xBoard.rows() - m_sBufOffset.second; r++)
		{
			for (int c = 0; c < m_xBoard.cols(); c++)
			{
				// The TArray implementation is diagonally flipped,
				// so account for it when converting to OLC buffer.
				// [r,c] becomes [c,r]
				switch (m_xBoard(r + m_sBufOffset.second, c).type)
				{
				case EMPTY:	DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteEMPTY);	break;	   
				case HERO:	DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteHERO);	break;
				case FLOOR:	DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteFLOOR);	break;	   
				case ICE:	DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteICE);	break;	   
				case FIRE:	DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteFIRE);	break;	   
				default:	   
					DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteEMPTY);
					break;
				}
			}
		}
		//DrawSprite(m_sPos.second * TILE_SIZE, (m_sPos.first - m_sBufOffset.second) * TILE_SIZE, spriteHERO);
		DrawSprite(m_cPos.col * TILE_SIZE, (m_cPos.row - m_sBufOffset.second) * TILE_SIZE, spriteHERO);
	}

	void OnDeath() 
	{
		//GameOver
	}

protected:
	virtual bool OnUserCreate() 
	{
		m_xBoard.resize(30, 15); // (Height, Width)

		spriteHERO		= new olcSprite(L"../OLC Sprite Editor/dungeon_hero.spr");
		spriteFLOOR		= new olcSprite(L"../OLC Sprite Editor/dungeon_floor.spr");
		spriteICE		= new olcSprite(L"../OLC Sprite Editor/dungeon_ice.spr");
		spriteFIRE		= new olcSprite(L"../OLC Sprite Editor/dungeon_fire.spr");
		spriteEMPTY		= new olcSprite(L"../OLC Sprite Editor/dungeon_empty.spr");

		m_sBufOffset.first = 0;
		m_sBufOffset.second = 8;

		m_cPos.row = 18;
		m_cPos.col = 7;
		m_dLastDirection = NORTH;
		m_bSliding = false;
		m_bAlive = true;
		m_sDepth = 0;

		//Make everything empty
		for (int i = 15; i < m_xBoard.rows(); i++)
			for (int j = 0; j < m_xBoard.cols(); j++)
				m_xBoard(i, j).type = EMPTY;

		DungeonModule startingArea = DungeonModule(3, 3);
		CreateModuleStarting(&startingArea);
		DrawModule(17, 6, &startingArea);

		DungeonModule testingArea = DungeonModule(5, 5);
		CreateModuleTesting(&testingArea);
		DrawModule(12, 5, &testingArea);
		
//#pragma region 
//		
//		// I
//		m_xBoard(1, 11).type = ICE;
//		m_xBoard(1, 12).type = ICE;
//		m_xBoard(1, 13).type = ICE;
//		m_xBoard(2, 12).type = ICE;
//		m_xBoard(3, 12).type = ICE;
//		m_xBoard(4, 12).type = ICE;
//		m_xBoard(5, 11).type = ICE;
//		m_xBoard(5, 12).type = ICE;
//		m_xBoard(5, 13).type = ICE;
//		// N 
//		
//		m_xBoard(7, 11).type = FIRE;
//		m_xBoard(7, 12).type = FIRE;
//		m_xBoard(7, 13).type = FIRE;
//		m_xBoard(8, 11).type = FIRE;
//		m_xBoard(8, 13).type = FIRE;
//		m_xBoard(9, 11).type = FIRE;
//		m_xBoard(9, 13).type = FIRE;
//		m_xBoard(10, 11).type = FIRE;
//		m_xBoard(10, 13).type = FIRE;
//		
//		// D
//		m_xBoard(14, 11).type = ICE;
//		m_xBoard(14, 12).type = ICE;
//		m_xBoard(15, 11).type = ICE;
//		m_xBoard(15, 13).type = ICE;
//		m_xBoard(16, 11).type = ICE;
//		m_xBoard(16, 13).type = ICE;
//		m_xBoard(17, 11).type = ICE;
//		m_xBoard(17, 13).type = ICE;
//		m_xBoard(18, 11).type = ICE;
//		m_xBoard(18, 12).type = ICE;
//		// E
//		m_xBoard(20, 11).type = FIRE;
//		m_xBoard(20, 12).type = FIRE;
//		m_xBoard(20, 13).type = FIRE;
//		m_xBoard(21, 11).type = FIRE;
//		m_xBoard(22, 11).type = FIRE;
//		m_xBoard(22, 12).type = FIRE;
//		m_xBoard(22, 13).type = FIRE;
//		m_xBoard(23, 11).type = FIRE;
//		m_xBoard(24, 11).type = FIRE;
//		m_xBoard(24, 12).type = FIRE;
//		m_xBoard(24, 13).type = FIRE;
//		// V
//		m_xBoard(26, 11).type = ICE;
//		m_xBoard(26, 13).type = ICE;
//		m_xBoard(27, 11).type = ICE;
//		m_xBoard(27, 13).type = ICE;
//		m_xBoard(28, 12).type = ICE;
//		
//#pragma endregion DEV LEVEL TILES

/*#pragma region

		m_xBoard(19, 6).type = FLOOR;
		m_xBoard(18, 6).type = FLOOR;
		m_xBoard(17, 6).type = FLOOR;
		m_xBoard(16, 6).type = FLOOR;
		m_xBoard(15, 6).type = FLOOR;
		m_xBoard(14, 6).type = FLOOR;
		m_xBoard(13, 6).type = FLOOR;
		m_xBoard(12, 6).type = FIRE;
		m_xBoard(11, 6).type = FLOOR;

		m_xBoard(19, 7).type = FLOOR;
		m_xBoard(18, 7).type = FLOOR;
		m_xBoard(17, 7).type = FLOOR;
		m_xBoard(16, 7).type = FLOOR;
		m_xBoard(15, 7).type = ICE;
		m_xBoard(14, 7).type = ICE;
		m_xBoard(13, 7).type = FLOOR;
		m_xBoard(12, 7).type = FIRE;
		m_xBoard(11, 7).type = FLOOR;

		m_xBoard(19, 8).type = FLOOR;
		m_xBoard(18, 8).type = FLOOR;
		m_xBoard(17, 8).type = FLOOR;
		m_xBoard(16, 8).type = FLOOR;
		m_xBoard(15, 8).type = FLOOR;
		m_xBoard(14, 8).type = FLOOR;
		m_xBoard(13, 8).type = FLOOR;
		m_xBoard(12, 8).type = FLOOR;
		m_xBoard(11, 8).type = FLOOR;

#pragma endregion*/
		
		UpdateScreen();

		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (!m_bAlive)
			OnDeath();

		if (m_bSliding)
			MovePlayer(m_dLastDirection);

		if (m_keys[VK_UP].bPressed) MovePlayer(NORTH);
		else if (m_keys[VK_DOWN].bPressed) MovePlayer(SOUTH);
		else if (m_keys[VK_LEFT].bPressed) MovePlayer(WEST);
		else if (m_keys[VK_RIGHT].bPressed) MovePlayer(EAST);

		return true;
	}
};

int main()
{
	Dungeon dungeonGame;
	dungeonGame.ConstructConsole(120, 120, 5, 5); //change first param for width
	dungeonGame.Start();
	return 0;
}

/* TODO **********************/
/*
- Figure out how display font larger than a tile.
- Show stats on right side of screen

- Make a main menu
- Make a level module editor
*/


/* LEVEL MODULE EDITOR *******/
/*
- LOAD
- SAVE
- ENTITY PICKER
- ERASE ENTITY
- PLACE ENTITY
*/