#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <SDL_ttf.h>
#include <cmath>

class LTexture {
private:
	int mWidth;
	int mHeight;
	SDL_Texture* mTexture;


public:
	bool loadFile(SDL_Renderer* gRenderer, std::string path);
	void free();
	void render(SDL_Renderer* gRenderer, int x, int y, SDL_Rect* clip = NULL);
	bool loadText(TTF_Font* gFont, SDL_Renderer* gRenderer, std::string textureText, SDL_Color textColor);

	LTexture();
	~LTexture();
	int getHeight();
	int getWidth();
};


