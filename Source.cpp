#include <iostream>
#include "TArray.h"
#include "olcConsoleGameEngine.h";
#include "GameTypes.h";
#include "DungeonModule.h";
using namespace std;

class Dungeon : public olcConsoleGameEngine
{
public:
	Dungeon()
	{

	}

private:
	GAMEBOARD m_xBoard;

	olcSprite *spriteHERO	= new olcSprite(L"sprites/dungeon_hero.spr");
	olcSprite *spriteSTONE  = new olcSprite(L"sprites/dungeon_stone.spr");
	olcSprite *spriteWALL	= new olcSprite(L"sprites/dungeon_wall.spr");
	olcSprite *spriteICE	= new olcSprite(L"sprites/dungeon_ice.spr");
	olcSprite *spriteFIRE	= new olcSprite(L"sprites/dungeon_fire.spr");
	olcSprite *spriteEMPTY	= new olcSprite(L"sprites/dungeon_empty.spr");
	olcSprite *spriteWOOD	= new olcSprite(L"sprites/dungeon_wood.spr");
	olcSprite *spriteWEB	= new olcSprite(L"sprites/dungeon_web.spr");
	olcSprite *spriteWEAK	= new olcSprite(L"sprites/dungeon_weakstone.spr");

	DungeonModule *dmSTARTING	= new DungeonModule(L"modules/dm_starting.dumo");
	DungeonModule *dmTESTING	= new DungeonModule(L"modules/dm_testing.dumo");
	DungeonModule *dmLOOPS		= new DungeonModule(L"modules/dm_loops.dumo");

	const short PLAYER_ROW_CONST = 28;
	const short TILE_SIZE = 8;
	pair<short, short> m_sBufOffset;

	float fAcc = 0.0f;
	float fet = 0.0f;
	int ticks = 0;

	// Player states
	COORD2 m_cPos;
	DIRECTION m_dLastDirection;
	bool m_bSliding;
	bool m_bAlive;
	bool m_bInWeb;
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

		for (int i = 0; i < dm->height; i++)
		{
			for (int j = 0; j < dm->width; j++)
			{
				ModuleToBoard(x + i, y + j, dm->GetEntity(j, i));
			}
		}
	}

	void UpdateWeb(DIRECTION d) 
	{
		//TODO
		// Make randomizer
		m_bInWeb = false;
	}
	void UpdateWeak(COORD2 c)
	{
		m_xBoard(c).e_state++;
		if (m_xBoard(c).e_state == 8)
		{
			m_xBoard(c).e_state = 0;
			m_xBoard(c).type = EMPTY;
			if (m_cPos == c)
				m_bAlive = false;
		}
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

		if (OnPlayerStep(dp))
			ShiftBoard(d);
		//UpdateScreen();
	}
	bool OnPlayerStep(COORD2 dp)
	{
		TILE nextTile = m_xBoard(m_cPos + dp);
		m_bSliding = false;
		m_bInWeb = false;
		switch (nextTile.type)
		{
		case WALL:
			return false; // don't step
			break;

		case EMPTY:
		case FIRE:
			m_bAlive = false; // dead
			break;

		case ICE:
			m_bSliding = true;
			break;

		case WEB:
			m_bInWeb = true;
			break;

		case WEAKSTONE:
			UpdateWeak(m_cPos + dp);
			break;

		default:
			break;
		}

		// Update the position
		COORD2 newPos = m_cPos + dp;
		// Update the depth
		m_sDepth -= dp.row;
		//Payers row position should never change.
		newPos.row = PLAYER_ROW_CONST;
		m_cPos = newPos;

		return true;
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
					{
						m_xBoard(r, c).type = m_xBoard(r - 1, c).type;
						m_xBoard(r, c).e_state = m_xBoard(r - 1, c).e_state;
					}
					else
					{
						m_xBoard(r, c).type = EMPTY;
						m_xBoard(r, c).e_state = 0;
					}
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
					{
						m_xBoard(r, c).type = m_xBoard(r + 1, c).type;
						m_xBoard(r, c).e_state = m_xBoard(r + 1, c).e_state;
					}
					else
					{
						m_xBoard(r, c).type = EMPTY;
						m_xBoard(r, c).e_state = 0;
					}
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
				case STONE:	DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteSTONE);	break;
				case WALL:	DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteWALL);	break;
				case ICE:	DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteICE);	break;	   
				case FIRE:	DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteFIRE);	break;
				case WOOD:	DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteWOOD);	break;
				case WEB:	DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteWEB);	break;
				case WEAKSTONE:
					if (m_xBoard(r + m_sBufOffset.second, c).e_state > 0)
					{
						COORD2 cell(r + m_sBufOffset.second, c);
						if (ticks % 5 == 0)
							UpdateWeak(cell);
					}
					DrawPartialSprite(c * TILE_SIZE, r * TILE_SIZE, spriteWEAK, m_xBoard(r + m_sBufOffset.second, c).e_state * 8, 0, 8, 8);
					break;
				default:	   
					DrawSprite(c * TILE_SIZE, r * TILE_SIZE, spriteEMPTY);
					break;
				}
			}
		}

		DrawSprite(m_cPos.col * TILE_SIZE, (m_cPos.row - m_sBufOffset.second) * TILE_SIZE, spriteHERO);
	}

	void OnReset()
	{
		m_xBoard.resize(40, 15); // (Height, Width)

		m_sBufOffset.first = 0;
		m_sBufOffset.second = 18;

		m_cPos.row = 28;
		m_cPos.col = 7;
		m_dLastDirection = NORTH;
		m_bSliding = false;
		m_bInWeb = false;
		m_bAlive = true;
		m_sDepth = 0;

		//Make everything empty
		for (int i = 15; i < m_xBoard.rows(); i++)
			for (int j = 0; j < m_xBoard.cols(); j++)
				m_xBoard(i, j).type = EMPTY;

		DrawModule(27, 6, dmSTARTING);
		DrawModule(18, 3, dmTESTING);
		DrawModule(11, 4, dmLOOPS);

		m_xBoard(0, 0).type = ICE;
		m_xBoard(0, 0).e_state = 2;
		m_xBoard(0, 1).type = ICE;
	}
	void OnDeath() 
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_HALF, FG_RED);
		if (m_keys[VK_SPACE].bPressed) OnReset();
	}

protected:
	virtual bool OnUserCreate() 
	{
		OnReset();		

		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime)
	{	
		fAcc += fElapsedTime;
		if (fAcc > 0.05)
		{
			fAcc = 0;
			ticks++;
			UpdateScreen();
			DrawString(0, 0, to_wstring(ticks));
		}

		//Skip frame is window is unactive
		if (GetConsoleWindow() != GetForegroundWindow())
			return true;

		if (!m_bAlive)
		{
			OnDeath();
			return true;
		}

		if (m_bSliding)
			MovePlayer(m_dLastDirection);

		if (m_bInWeb)
		{
			if (m_keys[VK_UP].bPressed) UpdateWeb(NORTH);
			else if (m_keys[VK_DOWN].bPressed) UpdateWeb(SOUTH);
			else if (m_keys[VK_LEFT].bPressed) UpdateWeb(WEST);
			else if (m_keys[VK_RIGHT].bPressed)UpdateWeb(EAST);
		}
		else
		{
			if (m_keys[VK_UP].bPressed) MovePlayer(NORTH);
			else if (m_keys[VK_DOWN].bPressed) MovePlayer(SOUTH);
			else if (m_keys[VK_LEFT].bPressed) MovePlayer(WEST);
			else if (m_keys[VK_RIGHT].bPressed) MovePlayer(EAST);
		}

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
- Add more ENTITY.
- Figure out how display font larger than a tile.
- Show stats on right side of screen

- Make a main menu
*/