#pragma once
#include <iostream>
#include <string>
#include "GameTypes.h";

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

private:
	
	ENTITY *tempTiles = nullptr;
	ENTITY *m_Tiles = nullptr;

	void Create(short w, short h)
	{
		width = w;
		height = h;
		m_Tiles = new ENTITY[w*h];
		for (int i = 0; i < w*h; i++)
			m_Tiles[i] = EMPTY;
	}

	void CreateTemp(short w, short h)
	{
		tempTiles = new ENTITY[w*h];
		for (int i = 0; i < w*h; i++)
			tempTiles[i] = EMPTY;
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
			return EMPTY;
		else
			return m_Tiles[y * width + x];
	}

	bool Save(wstring sFile)
	{
		short ver = 2;

		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"wb");
		if (f == nullptr)
			return false;

		fwrite("DuMo", sizeof(char), 4, f);		//FILETYPE
		fwrite(&ver, sizeof(short), 1, f);		//VERSION
		fwrite(&width, sizeof(short), 1, f);	//WIDTH
		fwrite(&height, sizeof(short), 1, f);	//HEIGHT
		fwrite(m_Tiles, sizeof(int), width * height, f); //TILES

		fclose(f);
		return true;
	}

	bool Load(wstring sFile)
	{
		char fileType[5];
		short ver = 0;
		width = 0;
		height = 0;

		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"rb");
		if (f == nullptr)
			return false;

		fread(fileType, sizeof(char), 4, f);	//FILETYPE
		fread(&ver, sizeof(short), 1, f);		//VERSION
		fread(&width, sizeof(short), 1, f);		//WIDTH
		fread(&height, sizeof(short), 1, f);	//HEIGHT

		Create(width, height);
		fread(m_Tiles, sizeof(int), width * height, f); //TILES

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