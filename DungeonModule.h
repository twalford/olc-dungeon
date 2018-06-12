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

};