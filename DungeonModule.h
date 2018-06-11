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
		return true;
		// TODO
	}

	bool Load(wstring sFile)
	{
		return true;
		// TODO
	}

};