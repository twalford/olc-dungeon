#pragma once
#include <iostream>
#include <string>
#include "GameTypes.h"

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

	DungeonModule(wstring sFile)
	{
		if (!Load(sFile))
			Create(5, 5);
	}

	short width = 0;
	short height = 0;
	short startCol = -1;
	short startAmt = 0;
	short endCol = -1;
	short endAmt = 0;

private:
	
	TILE *tempTiles = nullptr;
	TILE *m_Tiles = nullptr;

	void Create(short w, short h)
	{
		width = w;
		height = h;
		m_Tiles = new TILE[w*h];
		for (int i = 0; i < w*h; i++)
		{
			m_Tiles[i].type = EMPTY;
			m_Tiles[i].e_state = 0;
		}
		startCol = -1;
		startAmt = 0;
		endCol = -1;
		endAmt = 0;
	}

	void CreateTemp(short w, short h)
	{
		tempTiles = new TILE[w*h];
		for (int i = 0; i < w*h; i++)
		{
			tempTiles[i].type = EMPTY;
			tempTiles[i].e_state = 0;
		}
	}

public:
	void SetEntity(short x, short y, ENTITY t)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return;
		else
			m_Tiles[y * width + x].type = t;
	}

	ENTITY GetEntity(short x, short y)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return EMPTY;
		else
			return m_Tiles[y * width + x].type;
	}

	TILE GetTile(short x, short y)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
		{
			TILE t;
			t.type = EMPTY;
			t.e_state = 0;
			return t;
		}
		else
			return m_Tiles[y * width + x];
	}

	bool Save(wstring sFile)
	{
		short ver = 3;

		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"wb");
		if (f == nullptr)
			return false;

		fwrite("DuMo", sizeof(char), 4, f);		//FILETYPE
		fwrite(&ver, sizeof(short), 1, f);		//VERSION
		fwrite(&width, sizeof(short), 1, f);	//WIDTH
		fwrite(&height, sizeof(short), 1, f);	//HEIGHT
		fwrite(&startCol, sizeof(short), 1, f);	//START TILE COL
		fwrite(&startAmt, sizeof(short), 1, f);	//NUMDER OF START TILES
		fwrite(&endCol, sizeof(short), 1, f);	//END TILE COL
		fwrite(&endAmt, sizeof(short), 1, f);	//NUMDER OF END TILES

		for (int i = 0; i < width*height; i++)  //TILES.TYPE
			fwrite(&m_Tiles[i].type, sizeof(int), 1, f); 

		fclose(f);
		return true;
	}

	bool Load(wstring sFile)
	{
		char fileType[4];
		short ver = 0;
		short sC = -1, sA = 0, eC = -1, eA = 0;


		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"rb");
		if (f == nullptr)
			return false;

		fread(fileType, sizeof(char), 4, f);	//FILETYPE
		if (fileType[0] == "DuMo"[0])
		{
			fread(&ver, sizeof(short), 1, f);		//VERSION
			fread(&width, sizeof(short), 1, f);		//WIDTH
			fread(&height, sizeof(short), 1, f);	//HEIGHT
			if (ver == 3)
			{
				fread(&sC, sizeof(short), 1, f);	//START TILE COL
				fread(&sA, sizeof(short), 1, f);	//NUMDER OF START TILES
				fread(&eC, sizeof(short), 1, f);	//END TILE COL
				fread(&eA, sizeof(short), 1, f);	//NUMDER OF END TILES
			}
			Create(width, height);
			for (int i = 0; i < width*height; i++)  //TILES.TYPE
				fread(&m_Tiles[i].type, sizeof(int), 1, f);
		}

		startCol = sC;
		startAmt = sA;
		endCol = eC;
		endAmt = eA;

		fclose(f);
		return true;
	}

	bool CropRowOrCol(short x, short y, ORIENTATION o)
	{
		if (o == VERTICAL && width > 1)
			DeleteCol(x);
		else if (o == HORIZONTAL && height > 1)
			DeleteRow(y);
		else
			return false;

		return true;
	}
	void DeleteRow(short y)
	{
		CreateTemp(width, height - 1);
		//Above Cut
		for (int r = 0; r < y; r++)
			for (int c = 0; c < width; c++)
				tempTiles[r * (width)+c] = m_Tiles[r * (width)+c];

		//Below Cut
		for (int r = y + 1; r < height; r++)
			for (int c = 0; c < width; c++)
				tempTiles[(r - 1) * (width)+c] = m_Tiles[r * (width)+c];

		Create(width, height - 1);
		for (int i = 0; i < width*height; i++)
			m_Tiles[i] = tempTiles[i];
	}
	void DeleteCol(short x)
	{
		CreateTemp(width - 1, height);
		//Left of Cut
		for (int r = 0; r < height; r++)
			for (int c = 0; c < x; c++)
				tempTiles[r * (width - 1) + c] = m_Tiles[r * (width)+c];

		//Right of Cut
		for (int r = 0; r < height; r++)
			for (int c = x + 1; c < width; c++)
				tempTiles[r * (width - 1) + c - 1] = m_Tiles[r * (width)+c];

		Create(width - 1, height);
		for (int i = 0; i < width*height; i++)
			m_Tiles[i] = tempTiles[i];
	}

	bool InsertRowOrCol(short x, short y, DIRECTION v, DIRECTION h, ORIENTATION o)
	{
		if (o == VERTICAL)
			InsertCol(x, v);
		else if (o == HORIZONTAL)
			InsertRow(y, h);
		else
			return false;

		return true;
	}
	void InsertRow(short y, DIRECTION h)
	{
		CreateTemp(width, height + 1);
		//Above Cut
		for (int r = 0; r < y + (h / 2); r++)
			for (int c = 0; c < width; c++)
				tempTiles[r * (width)+c] = m_Tiles[r * (width)+c];

		//Below Cut
		for (int r = y + (h / 2); r < height; r++)
			for (int c = 0; c < width; c++)
				tempTiles[(r + 1) * (width)+c] = m_Tiles[r * (width)+c];

		Create(width, height + 1);
		for (int i = 0; i < width*height; i++)
			m_Tiles[i] = tempTiles[i];
	}
	void InsertCol(short x, DIRECTION v)
	{
		CreateTemp(width + 1,height);
		
		if (v == EAST) x += 1;

		//Left of Cut
		for (int r = 0; r < height; r++)
			for (int c = 0; c < x; c++)
				tempTiles[r * (width + 1) + c] = m_Tiles[r * (width)+c];

		//Right of Cut
		for (int r = 0; r < height; r++)
			for (int c = x; c < width; c++)
				tempTiles[r * (width + 1) + c + 1] = m_Tiles[r * (width)+c];

		Create(width + 1, height);
		for (int i = 0; i < width*height; i++)
			m_Tiles[i] = tempTiles[i];
	}
};