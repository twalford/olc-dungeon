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
#pragma endregion GAME TYPES

class Dungeon : public olcConsoleGameEngine
{
public:
	Dungeon()
	{

	}

private:
	GAMEBOARD m_xBoard;

	//Sprites
	olcSprite *spriteHERO	= nullptr;
	olcSprite *spriteFLOOR	= nullptr;
	olcSprite *spriteICE	= nullptr;
	olcSprite *spriteFIRE	= nullptr;
	olcSprite *spriteEMPTY	= nullptr;

	const short TILE_SIZE = 8;

	pair<short, short> m_sPos;
	pair<short, short> m_sBufOffset;

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
	}

	void MovePlayer(DIRECTION d)
	{
		// Overwrite the old position with floor.
		m_xBoard(m_sPos.first, m_sPos.second).type = FLOOR;

		switch (d)
		{
		case NORTH: m_sPos.first  -= 1; break;
		case EAST:  m_sPos.second += 1;	break;
		case SOUTH: m_sPos.first  += 1; break;
		case WEST:  m_sPos.second -= 1;	break;
		}

		//stay in boundry
		if (m_sPos.first < 0) m_sPos.first = 0; //technically shouldnt need this
		else if (m_sPos.second < 0) m_sPos.second = 0;
		else if (m_sPos.first  > m_xBoard.rows() - 1) m_sPos.first  = m_xBoard.rows() - 1; //technically shouldnt need this either
		else if (m_sPos.second > m_xBoard.cols() - 1) m_sPos.second = m_xBoard.cols() - 1;

		// save the new position.
		m_xBoard(m_sPos.first, m_sPos.second).type = HERO;

		//Payers row position should never change.
		m_sPos.first = 21;
		//ShiftBoard
		ShiftBoard(d);

		//Update screen
		UpdateScreen();
	}

protected:
	virtual bool OnUserCreate() 
	{
		spriteHERO		= new olcSprite(L"../OLC Sprite Editor/dungeon_hero.spr");
		spriteFLOOR		= new olcSprite(L"../OLC Sprite Editor/dungeon_floor.spr");
		spriteICE		= new olcSprite(L"../OLC Sprite Editor/dungeon_ice.spr");
		spriteFIRE		= new olcSprite(L"../OLC Sprite Editor/dungeon_fire.spr");
		spriteEMPTY		= new olcSprite(L"../OLC Sprite Editor/dungeon_empty.spr");

		m_sPos.first = 21;
		m_sPos.second = 7;
		
		m_sBufOffset.first = 0;
		m_sBufOffset.second = 8;

		m_xBoard.resize(30, 15);

		//Make everything empty
		for (int i = 0; i < m_xBoard.rows(); i++)
			for (int j = 0; j < m_xBoard.cols(); j++)
				m_xBoard(i, j).type = FLOOR;

		m_xBoard(m_sPos.first, m_sPos.second).type = HERO;

		//m_xBoard(0, 1).type = FIRE;
		//m_xBoard(2, 1).type = EMPTY;
		//m_xBoard(3, 1).type = ICE;
		//m_xBoard(5, 1).type = FIRE;

#pragma region 
		
		// I
		m_xBoard(1, 11).type = ICE;
		m_xBoard(1, 12).type = ICE;
		m_xBoard(1, 13).type = ICE;
		m_xBoard(2, 12).type = ICE;
		m_xBoard(3, 12).type = ICE;
		m_xBoard(4, 12).type = ICE;
		m_xBoard(5, 11).type = ICE;
		m_xBoard(5, 12).type = ICE;
		m_xBoard(5, 13).type = ICE;
		// N 
		
		m_xBoard(7, 11).type = FIRE;
		m_xBoard(7, 12).type = FIRE;
		m_xBoard(7, 13).type = FIRE;
		m_xBoard(8, 11).type = FIRE;
		m_xBoard(8, 13).type = FIRE;
		m_xBoard(9, 11).type = FIRE;
		m_xBoard(9, 13).type = FIRE;
		m_xBoard(10, 11).type = FIRE;
		m_xBoard(10, 13).type = FIRE;
		
		// D
		m_xBoard(14, 11).type = ICE;
		m_xBoard(14, 12).type = ICE;
		m_xBoard(15, 11).type = ICE;
		m_xBoard(15, 13).type = ICE;
		m_xBoard(16, 11).type = ICE;
		m_xBoard(16, 13).type = ICE;
		m_xBoard(17, 11).type = ICE;
		m_xBoard(17, 13).type = ICE;
		m_xBoard(18, 11).type = ICE;
		m_xBoard(18, 12).type = ICE;
		// E
		m_xBoard(20, 11).type = FIRE;
		m_xBoard(20, 12).type = FIRE;
		m_xBoard(20, 13).type = FIRE;
		m_xBoard(21, 11).type = FIRE;
		m_xBoard(22, 11).type = FIRE;
		m_xBoard(22, 12).type = FIRE;
		m_xBoard(22, 13).type = FIRE;
		m_xBoard(23, 11).type = FIRE;
		m_xBoard(24, 11).type = FIRE;
		m_xBoard(24, 12).type = FIRE;
		m_xBoard(24, 13).type = FIRE;
		// V
		m_xBoard(26, 11).type = ICE;
		m_xBoard(26, 13).type = ICE;
		m_xBoard(27, 11).type = ICE;
		m_xBoard(27, 13).type = ICE;
		m_xBoard(28, 12).type = ICE;
		
#pragma endregion DEV LEVEL TILES
		
		UpdateScreen();

		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime)
	{
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
	dungeonGame.ConstructConsole(120, 120, 5, 5);
	dungeonGame.Start();
	return 0;
}