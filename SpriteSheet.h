/********************
 ORGINAL BY Itay2805
********************/
#pragma once

#include "olcConsoleGameEngine.h"

#include <string>

class SpriteSheet {
private:
	olcSprite* spritesheet;

	olcSprite* sprites;
	int spriteCount;
	int tileWidth, tileHeight;

public:

	~SpriteSheet() {
		delete[] sprites;
	}

	SpriteSheet() {}

	SpriteSheet(const wstring& file, int tileWidth, int tileHeight = -1) {
		Load(file, tileWidth, tileHeight);
	}

	void Load(const wstring& file, int tileWidth, int tileHeight) {
		this->tileHeight = tileHeight;
		this->tileWidth = tileWidth;

		spritesheet = new olcSprite(file);

		int tileCountX = spritesheet->nWidth / tileWidth;
		int tileCountY = spritesheet->nHeight / tileHeight;
		spriteCount = tileCountX * tileCountY;

		//delete[] sprites;
		sprites = new olcSprite[spriteCount];

		for (int i = 0; i < spriteCount; i++) {
			sprites[i] = olcSprite(tileWidth, tileHeight);
			int baseX = (i % tileCountX);
			int baseY = ((i - baseX) / tileCountX);
			baseX *= tileWidth;
			baseY *= tileHeight;
			for (int y = 0; y < tileHeight; y++) {
				for (int x = 0; x < tileWidth; x++) {
					sprites[i].SetColour(x, y, spritesheet->GetColour(baseX + x, baseY + y));
					sprites[i].SetGlyph(x, y, spritesheet->GetGlyph(baseX + x, baseY + y));
				}
			}
		}

		delete spritesheet;
		spritesheet = nullptr;
	}

	olcSprite* operator[](int index) const {
		return &sprites[index];
	}

	inline int GetTileWidth() const { return this->tileWidth; }
	inline int GetTileHeight() const { return this->tileHeight; }
	inline int GetTileCount() const { return this->spriteCount; }

public:

};
