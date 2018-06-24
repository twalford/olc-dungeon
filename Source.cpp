#include <iostream>
#include "TArray.h"
#include "olcConsoleGameEngine.h"
#include "GameTypes.h"
#include "DungeonModule.h"
#include "SpriteSheet.h"
using namespace std;

class Dungeon : public olcConsoleGameEngine
{
public:
	Dungeon()
	{

	}

private:
	GAMEBOARD m_xBoard;
	SpriteSheet spriteFONT;

	olcSprite *sprites[16] =
	{
		new olcSprite(L"sprites/dungeon_empty.spr"),
		new olcSprite(L"sprites/dungeon_stone.spr"),
		new olcSprite(L"sprites/dungeon_wall.spr"),
		new olcSprite(L"sprites/dungeon_ice.spr"),
		new olcSprite(L"sprites/dungeon_fire.spr"),
		new olcSprite(L"sprites/dungeon_hero.spr"),
		new olcSprite(L"sprites/dungeon_wood.spr"),
		new olcSprite(L"sprites/dungeon_web.spr"),
		new olcSprite(L"sprites/dungeon_weakstone.spr"),
		new olcSprite(L"sprites/dungeon_portal_start_blue.spr"),
		new olcSprite(L"sprites/dungeon_portal_end_blue.spr"),
		new olcSprite(L"sprites/dungeon_portal_start_red.spr"),
		new olcSprite(L"sprites/dungeon_portal_end_red.spr"),
		new olcSprite(L"sprites/dungeon_portal_start_green.spr"),
		new olcSprite(L"sprites/dungeon_portal_end_green.spr"),
		new olcSprite(L"sprites/dungeon_pot.spr")
	};

	DungeonModule *dmSTARTING	= new DungeonModule(L"modules/dm_starting.dumo");
	DungeonModule *dmTESTING	= new DungeonModule(L"modules/dm_testing.dumo");
	DungeonModule *dmLOOPS		= new DungeonModule(L"modules/dm_loops.dumo");

	const short CONST_PLAYER_ROW = 29;
	const short CONST_SCREEN_TILE_OFFSET = 19;
	const short CONST_TILE_SIZE = 8;
	const short CONST_IDLE_THRESHOLD = 500;
	const short CONST_DOOM_START_POS = 4;
	const short CONST_DOOM_CREEP_SPEED = 60;
	float fAcc;
	float fet;
	int ticks;
	int smy;
	int smx;
	int doomCreepPos;
	int idleTimer;

	// Player states
	COORD2 m_cPos;
	DIRECTION m_dLastDirection;
	bool m_bSliding;
	bool m_bAlive;
	bool m_bInWeb;
	bool m_bTeleporting;
	short m_sDepth;
	short m_sCoins;

	void DrawStringFont(int x, int y, const wstring& chars)
	{
		for (wchar_t c : chars)
		{
			if (c - ' ' >= spriteFONT.GetTileCount())
				continue;

			DrawSprite(x, y, spriteFONT[c - ' ']);
			x += spriteFONT.GetTileWidth();
		}
	}

	void ModuleToBoard(int x, int y, TILE t)
	{
		if (x >= 0 && x < m_xBoard.rows() && y >= 0 && y < m_xBoard.cols()) {
			m_xBoard(x, y).type = t.type;
			m_xBoard(x, y).e_state = t.e_state;
		}
	}
	void DrawModule(short x, short y, DungeonModule *dm)
	{
		if (dm == nullptr)
			return;

		TILE t;
		t.e_state = 0;

		for (int i = 0; i < dm->height; i++)
		{
			for (int j = 0; j < dm->width; j++)
			{
				t.type = dm->GetEntity(j, i);
				t.e_state = 0;

				if (t.type == 9 || t.type == 11 || t.type == 13)
					t.e_state = SetEncodeToPortal(j, i, dm, t.type);

				if (t.type >= 4096 && t.type <= 4100)
					t = SetEncodeForPot(dm, t.type);

				ModuleToBoard(x + i, y + j, t);
			}
		}
	}

	TILE SetEncodeForPot(DungeonModule *dm, ENTITY e)
	{
		TILE rt;
		switch (e)
		{
		case POT_ICE:
			rt.type = POT_ICE;
			rt.e_state = 0;
			break;
		case POT_ICE_BROKEN:
			rt.type = POT_ICE;
			rt.e_state = 1;
			break;
		case POT_STONE:
			rt.type = POT_STONE;
			rt.e_state = 0;
			break;
		case POT_STONE_BROKEN:
			rt.type = POT_STONE;
			rt.e_state = 1;
			break;
		default:
			break;
		}
		return rt;
	}
	short SetEncodeToPortal(short ax, short ay, DungeonModule *dm, ENTITY e)
	{
		for (int i = 0; i < dm->height; i++)
			for (int j = 0; j < dm->width; j++)
				if (dm->GetEntity(j, i) == e + 1)
				{
					/* Yes, I'm attempting to store 2 shorts into 1.
					The first byte will hold the ROW offset.
					The second byte will hold the COL offset.
					Short to char should okay here because we will
					only be teleporting around 2 to 5 tiles.

						 dx = 0000 0000  0000 0011 = 0x0003 =  3
						 dy = 1111 1111  1111 1101 = 0xfffd = -3
				    e_state = 0000 0011  1111 1101 = 0x03fd = 1021	
					
					I'm hoping this will save some computation time 
					in ShiftBoard()	by having 1 less short to assign 
					600 times.	*/

					short v = j - ax;
					v <<= 8;
					v |= ((i - ay) & 0x00ff);
					return v;
				}

		return 0;
	}
	void TeleportUsingState(COORD2 c)
	{
		// Extracting 2 shorts(chars) from a short.
		short state = m_xBoard(c).e_state;
		// dx will be the change in width.
		short dx = state >> 8;
		// dy will be the change in height.
		short dy = state & 0x00ff;
		// If its a negative, we have to set the
		// first byte to ff.
		if (dy > 0x00f0)
			dy |= 0xff00;

		m_cPos.col = c.col + dx;
		m_sDepth -= dy;

		// If teleporting forward.. the idletime should subtract
		// travel distance * CREEP_SPEED but not go into negatives.
		if (dy < 0) idleTimer = max(idleTimer + (dy * CONST_DOOM_CREEP_SPEED), 0); //dy negative, so we add
		doomCreepPos = max(doomCreepPos + dy, 0);

		for (int i = 0; i < abs(dy); i++)
			ShiftBoard(dy < 0 ? NORTH : SOUTH, false);
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
			ShiftBoard(d, true);
	}
	bool OnPlayerStep(COORD2 dp)
	{
		TILE nextTile = m_xBoard(m_cPos + dp);
		m_bSliding = false;
		m_bInWeb = false;
		m_bTeleporting = false;
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

		case PORTAL_START_BLUE:
		case PORTAL_START_RED:
		case PORTAL_START_GREEN:
			TeleportUsingState(m_cPos + dp);
			m_bTeleporting = true;
			return true;
			break;

		case POT_ICE:
			m_xBoard(m_cPos + dp).e_state++;
			if (nextTile.e_state == 1)
			{
				m_xBoard(m_cPos + dp).type = ICE;
				m_xBoard(m_cPos + dp).e_state = 0;
				m_bSliding = true;
			}
			else
				return false;
			break;

		case POT_STONE:
			m_xBoard(m_cPos + dp).e_state++;
			if (nextTile.e_state == 1)
			{
				m_xBoard(m_cPos + dp).type = STONE;
				m_xBoard(m_cPos + dp).e_state = 0;
			}
			else
				return false;
			break;

		default:
			break;
		}

		// Update the depth and column postition
		m_sDepth -= dp.row;
		m_cPos.col += dp.col;

		// Update the depth of creeping doom
		if (dp.row == -1)
		{
			idleTimer = max(min(idleTimer - 100,400),0);
			if (doomCreepPos > 0)
				doomCreepPos--;
		}
		else if (dp.row == 1)
			doomCreepPos++;

		return true;
	}
	void ShiftBoard(DIRECTION d, bool smooth)
	{
		//shift the screen output 1/8th 8 times
		if (smooth && m_bTeleporting == false)
		{
			if (d == SOUTH)
				smy = 8;
			else if (d == NORTH)
				smy = -8;
			else if (d == EAST)
				smx = -8;
			else if (d == WEST)
				smx = 8;
		}

		//shift the tile data up or down
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
						m_xBoard(r, c).type = FIRE;
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
		for (int r = 0; r < m_xBoard.rows() - CONST_SCREEN_TILE_OFFSET; r++)
		{
			for (int c = 0; c < m_xBoard.cols(); c++)
			{
				// The TArray implementation is diagonally flipped,
				// so account for it when converting to OLC buffer.
				// [r,c] becomes [c,r]
				switch (m_xBoard(r + CONST_SCREEN_TILE_OFFSET, c).type)
				{
				case WEAKSTONE:
					if (m_xBoard(r + CONST_SCREEN_TILE_OFFSET, c).e_state > 0)
					{
						COORD2 cell(r + CONST_SCREEN_TILE_OFFSET, c);
						if (ticks % 20 == 0)
							UpdateWeak(cell);
					}
					DrawPartialSprite(c * CONST_TILE_SIZE, r * CONST_TILE_SIZE + smy, 
						sprites[WEAKSTONE], m_xBoard(r + CONST_SCREEN_TILE_OFFSET, c).e_state * 8, 0, 8, 8);
					break;

				case POT_ICE:
					DrawSprite(c * CONST_TILE_SIZE, r * CONST_TILE_SIZE + smy, sprites[3]);
					DrawPartialSprite(c * CONST_TILE_SIZE, r * CONST_TILE_SIZE + smy, 
						sprites[15], m_xBoard(r + CONST_SCREEN_TILE_OFFSET, c).e_state * 8, 0, 8, 8);
					break;

				case POT_STONE:
					DrawSprite(c * CONST_TILE_SIZE, r * CONST_TILE_SIZE + smy, sprites[1]);
					DrawPartialSprite(c * CONST_TILE_SIZE, r * CONST_TILE_SIZE + smy, 
						sprites[15], m_xBoard(r + CONST_SCREEN_TILE_OFFSET, c).e_state * 8, 0, 8, 8);
					break;

				default:	
					// everything else
					DrawSprite(c * CONST_TILE_SIZE, r * CONST_TILE_SIZE + smy,
						sprites[m_xBoard(r + CONST_SCREEN_TILE_OFFSET, c).type]);
					break;
				}
			}
		}

		// Draw the hero
		DrawSprite(m_cPos.col * CONST_TILE_SIZE + smx, (m_cPos.row - CONST_SCREEN_TILE_OFFSET) * CONST_TILE_SIZE, sprites[5]);

		// Stats
		Fill(120, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
		DrawStringFont(120, 0,  L"TICKS " + to_wstring(ticks));
		DrawStringFont(120, 8,  L"IDLE  " + to_wstring(idleTimer));
		DrawStringFont(120, 16, L"DOOM  " + to_wstring(doomCreepPos));
	}
	void UpdateDoomCreep()
	{
		//for (int r = m_xBoard.rows() - 1; r >= m_xBoard.rows() - 1 - doomCreepPos; r--)
		int r = m_xBoard.rows() - doomCreepPos;
			for (int c = m_xBoard.cols() - 1; c >= 0; c--)
				m_xBoard(r, c).type = FIRE;
	}

	void OnReset()
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
		m_xBoard.resize(40, 15); // (Height, Width)

		m_cPos.row = CONST_PLAYER_ROW;
		m_cPos.col = 7;
		m_dLastDirection = NORTH;
		m_bSliding = false;
		m_bInWeb = false;
		m_bAlive = true;
		m_bTeleporting = false;
		m_sDepth = 0;
		m_sCoins = 0;

		fAcc = 0.0f;
		fet = 0.0f;
		ticks = 0;
		smy = 0;
		smx = 0;
		doomCreepPos = CONST_DOOM_START_POS;
		idleTimer = 0;

		//Make everything empty
		for (int i = 0; i < m_xBoard.rows(); i++)
			for (int j = 0; j < m_xBoard.cols(); j++)
				m_xBoard(i, j).type = EMPTY;

		for (int i = m_xBoard.rows() - 1; i >= m_xBoard.rows() - CONST_DOOM_START_POS; i--)
			for (int j = 0; j < m_xBoard.cols(); j++)
				m_xBoard(i, j).type = FIRE;

		DrawModule(28, 6, dmSTARTING);
		DrawModule(19, 3, dmTESTING);
		DrawModule(12, 4, dmLOOPS);

		m_xBoard(0, 0).type = ICE;
		m_xBoard(0, 0).e_state = 2;
		m_xBoard(0, 1).type = ICE;
	}
	void OnDeath() 
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_HALF, FG_RED);
		DrawStringFont(120, 28, L"DEPTH " + to_wstring(m_sDepth));
		DrawStringFont(120, 36, L"COINS " + to_wstring(m_sCoins));
		if (m_keys[VK_SPACE].bPressed) OnReset();
	}

protected:
	virtual bool OnUserCreate() 
	{
		spriteFONT.Load(L"sprites/javidx9_nesfont8x8.spr", 8, 8);
		OnReset();	
		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime)
	{	
		//Skip frame is window is unactive
		if (GetConsoleWindow() != GetForegroundWindow())
			return true;

		fAcc += fElapsedTime;
		if (fAcc >= 0.008)
		{
			fAcc -= 0.008f;
			ticks++;
			idleTimer++;

			if (smy > 0)
				smy--;
			else if (smy < 0)
				smy++;
			else if (smx < 0)
				smx++;
			else if (smx > 0)
				smx--;

			if (idleTimer >= CONST_IDLE_THRESHOLD && (idleTimer - 100) % 200 == 0)
			{
				doomCreepPos++;
				UpdateDoomCreep();
			}

			UpdateScreen();
		}

		//Skip if moving
		if (smy != 0 || smx != 0)
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

public:

};

int main()
{
	Dungeon dungeonGame;
	dungeonGame.ConstructConsole(200, 136, 4, 4); //change first param for width
	dungeonGame.Start();
	return 0;
}

/* TODO **********************/
/*
- Add more ENTITY.
- Show stats on right side of screen

- Make a main menu
*/