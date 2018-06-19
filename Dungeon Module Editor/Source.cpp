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
	int nBrushSet = 1;
	bool bCropMode = false;
	ORIENTATION oCropOrientation = VERTICAL;
	DIRECTION dInsertDirHori = SOUTH;
	DIRECTION dInsertDirVert = EAST;
	ENTITY eCurrentENTITY = EMPTY;
	olcSprite *cropSprites[4] = {
		new olcSprite(L"dumoEditor_cropNorth.spr"),
		new olcSprite(L"dumoEditor_cropEast.spr"),
		new olcSprite(L"dumoEditor_cropSouth.spr"),
		new olcSprite(L"dumoEditor_cropWest.spr"),
	};
	olcSprite *mySprites[15] = {
		new olcSprite(L"../OLCdungeon/sprites/dungeon_empty.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_stone.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_wall.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_ice.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_fire.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_hero.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_wood.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_web.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_weakstone.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_portal_start_blue.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_portal_end_blue.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_portal_start_red.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_portal_end_red.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_portal_start_green.spr"),
		new olcSprite(L"../OLCdungeon/sprites/dungeon_portal_end_green.spr")
	};
	olcSprite *mySprites_Pot = new olcSprite(L"../OLCdungeon/sprites/dungeon_pot.spr");

	DungeonModule *dm = nullptr;
	wstring sCurrentModuleFile;

protected:
	virtual bool OnUserCreate()
	{
		dm = new DungeonModule(5, 5);
		sCurrentModuleFile = L"../OLCdungeon/modules/dm_testing.dumo";

		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		//Skip frame is window is unactive
		if (GetConsoleWindow() != GetForegroundWindow())
			return true;

		// CropMode Selector
		if (m_keys['C'].bReleased)
			bCropMode = !bCropMode;

		// Zooming 
		if (m_keys[VK_PRIOR].bReleased)
			nZoom <<= 1;

		if (m_keys[VK_NEXT].bReleased)
			nZoom >>= 1;

		if (nZoom > 32) nZoom = 32;
		if (nZoom < 2) nZoom = 2;

		// Brushes
		if (nBrushSet == 1)
		{
			if (m_keys[0x31].bReleased) eCurrentENTITY = EMPTY;
			if (m_keys[0x32].bReleased) eCurrentENTITY = STONE;
			if (m_keys[0x33].bReleased) eCurrentENTITY = WALL;
			if (m_keys[0x34].bReleased) eCurrentENTITY = ICE;
			if (m_keys[0x35].bReleased) eCurrentENTITY = FIRE;
			if (m_keys[0x36].bReleased) eCurrentENTITY = WOOD;
			if (m_keys[0x37].bReleased) eCurrentENTITY = WEB;
			if (m_keys[0x38].bReleased) eCurrentENTITY = WEAKSTONE;
			//if (m_keys[0x39].bReleased) eCurrentENTITY =
			//if (m_keys[0x30].bReleased) eCurrentENTITY =
		}
		else if (nBrushSet == 2)
		{
			if (m_keys[0x31].bReleased) eCurrentENTITY = PORTAL_START_BLUE;
			if (m_keys[0x32].bReleased) eCurrentENTITY = PORTAL_END_BLUE;
			if (m_keys[0x33].bReleased) eCurrentENTITY = PORTAL_START_RED;
			if (m_keys[0x34].bReleased) eCurrentENTITY = PORTAL_END_RED;
			if (m_keys[0x35].bReleased) eCurrentENTITY = PORTAL_START_GREEN;
			if (m_keys[0x36].bReleased) eCurrentENTITY = PORTAL_END_GREEN;
			//if (m_keys[0x37].bReleased) eCurrentENTITY = 
			//if (m_keys[0x38].bReleased) eCurrentENTITY = 
			//if (m_keys[0x39].bReleased) eCurrentENTITY = 
			//if (m_keys[0x30].bReleased) eCurrentENTITY = 
		}
		else if (nBrushSet == 3)
		{
			if (m_keys[0x31].bReleased) eCurrentENTITY = POT_ICE;
			if (m_keys[0x32].bReleased) eCurrentENTITY = POT_ICE_BROKEN;
			if (m_keys[0x33].bReleased) eCurrentENTITY = POT_STONE;
			if (m_keys[0x34].bReleased) eCurrentENTITY = POT_STONE_BROKEN;
			//if (m_keys[0x35].bReleased) eCurrentENTITY = 
			//if (m_keys[0x36].bReleased) eCurrentENTITY = 
			//if (m_keys[0x37].bReleased) eCurrentENTITY = 
			//if (m_keys[0x38].bReleased) eCurrentENTITY = 
			//if (m_keys[0x39].bReleased) eCurrentENTITY = 
			//if (m_keys[0x30].bReleased) eCurrentENTITY = 
		}
		else if (nBrushSet == 4)
		{
			//if (m_keys[0x31].bReleased) eCurrentENTITY = 
			//if (m_keys[0x32].bReleased) eCurrentENTITY = 
			//if (m_keys[0x33].bReleased) eCurrentENTITY = 
			//if (m_keys[0x34].bReleased) eCurrentENTITY = 
			//if (m_keys[0x35].bReleased) eCurrentENTITY = 
			//if (m_keys[0x36].bReleased) eCurrentENTITY = 
			//if (m_keys[0x37].bReleased) eCurrentENTITY = 
			//if (m_keys[0x38].bReleased) eCurrentENTITY = 
			//if (m_keys[0x39].bReleased) eCurrentENTITY = 
			//if (m_keys[0x30].bReleased) eCurrentENTITY = 
		}
		
		// Brush set selector
		if (m_keys[VK_F1].bReleased) nBrushSet = 1;
		if (m_keys[VK_F2].bReleased) nBrushSet = 2;
		if (m_keys[VK_F3].bReleased) nBrushSet = 3;
		if (m_keys[VK_F4].bReleased) nBrushSet = 4;

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
			if (bCropMode) // Crop Mode
			{
				if (nPosX < 0) nPosX = 0;
				if (nPosX >= dm->width) nPosX = dm->width - 1;
				if (nPosY < 0) nPosY = 0;
				if (nPosY >= dm->height) nPosY = dm->height - 1;

				if (m_keys[VK_DELETE].bReleased)
					dm->CropRowOrCol(nPosX, nPosY, oCropOrientation);

				if (m_keys[VK_INSERT].bReleased)
					dm->InsertRowOrCol(nPosX, nPosY, dInsertDirVert, dInsertDirHori, oCropOrientation);

				if (m_keys[VK_SPACE].bReleased)
				{
					if (oCropOrientation == VERTICAL)
						dInsertDirVert = (DIRECTION)((dInsertDirVert + 2) % 4);
					if (oCropOrientation == HORIZONTAL)
						dInsertDirHori = (DIRECTION)((dInsertDirHori + 2) % 4);
				}

				if (m_keys['V'].bReleased)
					oCropOrientation = (ORIENTATION)((oCropOrientation + 1) % 2);
			}
			else //Edit Mode
			{
				if (nPosX < 0) nPosX = 0;
				if (nPosX >= dm->width) nPosX = dm->width - 1;
				if (nPosY < 0) nPosY = 0;
				if (nPosY >= dm->height) nPosY = dm->height - 1;

				if (m_keys[VK_SPACE].bReleased)
				{
					dm->SetEntity(nPosX - 0, nPosY - 0, eCurrentENTITY);
				}

				if (m_keys[VK_DELETE].bReleased)
				{
					dm->SetEntity(nPosX - 0, nPosY - 0, EMPTY);
				}

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
		DrawString(1, 1, L"F1-F4 = ENTITY SETS    F9 = Load File  F10 = Save File");
		DrawString(1, 2, L"....");
		Draw(nBrushSet, 2, L'o');
		if		(nBrushSet == 1) DrawString(1, 3, L"1=EMPTY   2=FLOOR   3=WALL    4=ICE     5=FIRE    6=WOOD    7=WEB     8=WEAK    9= ");
		else if (nBrushSet == 2) DrawString(1, 3, L"1=PTL BS  2=PTL BE  3=PTL RS  4=PTL RE  5=PTL GS  6=PTL GE  7=        8=        9= ");
		else if (nBrushSet == 3) DrawString(1, 3, L"1=POT I   2=POT IB  3=POT S   4=POT SB  5=        6=        7=        8=        9= ");
		else if (nBrushSet == 4) DrawString(1, 3, L"1=        2=        3=        4=        5=        6=        7=        8=        9= ");
		
		DrawString(1, 5, L"Current ENTITY:");
		//DrawSprite(1, 7, mySprites[eCurrentENTITY]);
		DrawPartialSprite(1,7,mySprites[eCurrentENTITY],0,0,8,8);

		// Draw Canvas
		for (int x = 10; x < 118; x++)
		{
			Draw(x, 9);
			Draw(x, 80);
		}

		for (int y = 9; y < 81; y++)
		{
			Draw(10, y);
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
						//if non regular
						if (dm->GetEntity(x - nOffsetX, y - nOffsetY) >= 0x1000)
						{
							//Draw the ground first.
							//Draw the pot type second.
							switch (dm->GetEntity(x - nOffsetX, y - nOffsetY))
							{
							case POT_ICE:
								DrawPartialSprite(x * nZoom + 11,
									y * nZoom + 10,
									mySprites[ICE],
									0, 0, 8, 8);
								DrawPartialSprite(x * nZoom + 11,
									y * nZoom + 10,
									mySprites_Pot,
									0, 0, 8, 8);
								break;
							case POT_ICE_BROKEN:
								DrawPartialSprite(x * nZoom + 11,
									y * nZoom + 10,
									mySprites[ICE],
									0, 0, 8, 8);
								DrawPartialSprite(x * nZoom + 11,
									y * nZoom + 10,
									mySprites_Pot,
									8, 0, 8, 8);
								break;
							case POT_STONE:
								DrawPartialSprite(x * nZoom + 11,
									y * nZoom + 10,
									mySprites[STONE],
									0, 0, 8, 8);
								DrawPartialSprite(x * nZoom + 11,
									y * nZoom + 10,
									mySprites_Pot,
									0, 0, 8, 8);
								break;
							case POT_STONE_BROKEN:
								DrawPartialSprite(x * nZoom + 11,
									y * nZoom + 10,
									mySprites[STONE],
									0, 0, 8, 8);
								DrawPartialSprite(x * nZoom + 11,
									y * nZoom + 10,
									mySprites_Pot,
									8, 0, 8, 8);
								break;
							}
						}
						else
						{
							//Draw the normal sprite
							DrawPartialSprite(x * nZoom + 11,
								y * nZoom + 10,
								mySprites[dm->GetEntity(x - nOffsetX, y - nOffsetY)],
								0,0,8,8);
						}

						// Draw Pixel Markers
						if (dm->GetEntity(x - nOffsetX, y - nOffsetY) == EMPTY)
							Draw((x) * nZoom + 11, (y) * nZoom + 10, L'.');

						// Draw Crop Markers
						if (bCropMode)
						{
							if (oCropOrientation == VERTICAL)
							{
								if (x == nPosX)
									DrawSprite((x)* nZoom + 11, (y)* nZoom + 10, cropSprites[dInsertDirVert]);
									//DrawPartialSprite((x)* nZoom + 11, (y)* nZoom + 10, cropSprites[dInsertDirVert], 0, 0, 8, 8);
							}
							else if (oCropOrientation == HORIZONTAL)
							{
								if (y == nPosY)
									DrawSprite((x)* nZoom + 11, (y)* nZoom + 10, cropSprites[dInsertDirHori]);
									//DrawPartialSprite((x)* nZoom + 11, (y)* nZoom + 10, cropSprites[dInsertDirHori], 0, 0, 8, 8);
									
							}
						}

					}
					if (x - nOffsetX == nPosX && y - nOffsetY == nPosY)
						Draw((x)* nZoom + 11, (y)* nZoom + 10, L'O');
					
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