#include <iostream>
#include <string>
#include "../OLCdungeon/olcConsoleGameEngine.h"
#include "../OLCdungeon/DungeonModule.h"
#include "../OLCdungeon/GameTypes.h"
using namespace std;

class OLCDungeon_ModuleEditor : public olcConsoleGameEngine
{
public:
	OLCDungeon_ModuleEditor()
	{
		m_sAppName = L"Module Editor";
	}

private:
	int nPosX = 0;
	int nPosY = 0;
	int nOffsetX = 0;
	int nOffsetY = 0;
	int nZoom = 8;
	ENTITY eCurrentENTITY = EMPTY;
	olcSprite *mySprites[6] = {
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};

	int glyphsColors[6] = { 
		FG_BLACK,		//EMPTY
		FG_GREY,		//FLOOR
		FG_DARK_GREY,	//WALL
		FG_CYAN,		//ICE
		FG_RED,			//FIRE
		FG_GREEN		//HERO
	};

	DungeonModule *dm = nullptr;
	wstring sCurrentModuleFile;

protected:
	virtual bool OnUserCreate()
	{
		dm = new DungeonModule(5, 5);
		sCurrentModuleFile = L"dm_test";

		mySprites[0] = new olcSprite(L"../OLCdungeon/sprites/dungeon_empty.spr");
		mySprites[1] = new olcSprite(L"../OLCdungeon/sprites/dungeon_floor.spr");
		mySprites[2] = new olcSprite(L"../OLCdungeon/sprites/dungeon_wall.spr");
		mySprites[3] = new olcSprite(L"../OLCdungeon/sprites/dungeon_ice.spr");
		mySprites[4] = new olcSprite(L"../OLCdungeon/sprites/dungeon_fire.spr");
		mySprites[5] = new olcSprite(L"../OLCdungeon/sprites/dungeon_hero.spr");

		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Zooming 
		if (m_keys[VK_PRIOR].bReleased)
			nZoom <<= 1;

		if (m_keys[VK_NEXT].bReleased)
			nZoom >>= 1;

		if (nZoom > 32) nZoom = 32;
		if (nZoom < 2) nZoom = 2;

		// Brushes
		if (m_keys[VK_F1].bReleased) eCurrentENTITY = EMPTY;
		if (m_keys[VK_F2].bReleased) eCurrentENTITY = FLOOR;
		if (m_keys[VK_F3].bReleased) eCurrentENTITY = WALL;
		if (m_keys[VK_F4].bReleased) eCurrentENTITY = ICE;
		if (m_keys[VK_F5].bReleased) eCurrentENTITY = FIRE;

		// Future brushes
		//if (m_keys[VK_F6].bReleased) eCurrentENTITY = ;
		//if (m_keys[VK_F7].bReleased) eCurrentENTITY = ;
		//if (m_keys[VK_F8].bReleased) eCurrentENTITY = ;
				// Once these are all used, implement shift key
		//if (m_keys[L'1'].bReleased) eCurrentENTITY = ;
		//if (m_keys[L'2'].bReleased) eCurrentENTITY = ;
		//if (m_keys[L'3'].bReleased) eCurrentENTITY = ;
		//if (m_keys[L'4'].bReleased) eCurrentENTITY = ;
		//if (m_keys[L'5'].bReleased) eCurrentENTITY = ;
		//if (m_keys[L'6'].bReleased) eCurrentENTITY = ;
		//if (m_keys[L'7'].bReleased) eCurrentENTITY = ;
		//if (m_keys[L'8'].bReleased) eCurrentENTITY = ;
		
		// Cursing
		if (m_keys[VK_SHIFT].bHeld)
		{
			if (m_keys[VK_UP].bReleased) nOffsetY++;
			if (m_keys[VK_DOWN].bReleased) nOffsetY--;
			if (m_keys[VK_LEFT].bReleased) nOffsetX++;
			if (m_keys[VK_RIGHT].bReleased) nOffsetX--;
		}
		else
		{
			if (m_keys[VK_UP].bReleased) nPosY--;
			if (m_keys[VK_DOWN].bReleased) nPosY++;
			if (m_keys[VK_LEFT].bReleased) nPosX--;
			if (m_keys[VK_RIGHT].bReleased) nPosX++;
		}

		if (dm != nullptr)
		{
			if (nPosX < 0) nPosX = 0;
			if (nPosX >= dm->width) nPosX = dm->width - 1;
			if (nPosY < 0) nPosY = 0;
			if (nPosY >= dm->height) nPosY = dm->height - 1;

			if (m_keys[VK_SPACE].bReleased)
			{
				dm->SetTile(nPosX - 0, nPosY - 0, eCurrentENTITY);
			}

			if (m_keys[VK_DELETE].bReleased)
			{
				dm->SetTile(nPosX - 0, nPosY - 0, EMPTY);
			}

			if (m_keys[VK_F9].bReleased)
			{
				dm->Load(sCurrentModuleFile);
			}

			if (m_keys[VK_F10].bReleased)
			{
				dm->Save(sCurrentModuleFile);
			}
		}

		// Erase canvas
		Fill(0,0,ScreenWidth(),ScreenHeight(), L' ', 0);

		// Draw Menu
		DrawString(1, 1, L"F1 = EMPTY  F2 = FLOOR  F3 = WALL  F4 = ICE  F5 = FIRE");
		DrawString(1, 2, L"F9 = Load File  F10 = Save File");

		DrawString(1, 5, L"Current ENTITY = ");
		Draw(18, 5, PIXEL_SOLID, glyphsColors[eCurrentENTITY]);

		// Draw Canvas
		for (int x = 9; x < 118; x++)
		{
			Draw(x, 9);
			Draw(x, 80);
		}

		for (int y = 9; y < 81; y++)
		{
			Draw(9, y);
			Draw(118, y);
		}

		// Draw Visible Module
		if (dm != nullptr)
		{
			int nVisiblePixelsX = 128 / nZoom;
			int nVisiblePixelsY = 64 / nZoom;

			for (int x = 0; x < nVisiblePixelsX; x++)
				for (int y = 0; y < nVisiblePixelsY; y++)
				{
					if (x - nOffsetX < dm->width && y - nOffsetY < dm->height && x - nOffsetX >= 0 && y - nOffsetY >= 0)
					{
						// Draw Sprite
						DrawSprite(x * nZoom + 10,
							y * nZoom + 10,
							//(x + 1) * nZoom + 10,
							//(y + 1) * nZoom + 10,
							mySprites[dm->GetTile(x - nOffsetX, y - nOffsetY)]);

						// Draw Pixel Markers
						if (dm->GetTile(x - nOffsetX, y - nOffsetY) == EMPTY)
							Draw((x) * nZoom + 10, (y) * nZoom + 10, L'.');
					}

					if (x - nOffsetX == nPosX && y - nOffsetY == nPosY)
						Draw((x) * nZoom + 10, (y) * nZoom + 10, L'O');
				}
		}


		return true;
	}
};

int main()
{
	// Use olcConsoleGameEngine derived app
	OLCDungeon_ModuleEditor game;
	game.ConstructConsole(164, 86, 8, 8);
	game.Start();

	return 0;
}