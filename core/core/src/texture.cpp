#include "texture.h"

bool LTexture::loadFile(SDL_Renderer* gRenderer, std::string path)
{
	free();
	SDL_Surface* loadedSurface = NULL;
	SDL_Texture* newTexture = NULL;

	loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("couldn't load file! SDL error:%s\n", SDL_GetError());
	}
	else {
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));

		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("fail to create texture");
		}
		else {
			mHeight = loadedSurface->h;
			mWidth = loadedSurface->w;
		}
	}
	SDL_FreeSurface(loadedSurface);
	mTexture = newTexture;
	return mTexture != NULL;

}

void LTexture::free()
{
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mTexture = 0;
		mWidth = 0;
	}
}

void LTexture::render(SDL_Renderer* gRenderer, int x, int y, SDL_Rect* clip)
{
	SDL_Rect rect = { x, y, mWidth, mHeight };
	if (clip != NULL) {
		rect.w = clip->w;
		rect.h = clip->h;
	}
	SDL_RenderCopy(gRenderer, mTexture, clip, &rect);
}

bool LTexture::loadText(TTF_Font* gFont, SDL_Renderer* gRenderer, std::string textureText, SDL_Color textColor)
{
	free();

	SDL_Surface* loadedSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);

	if (loadedSurface == NULL)
	{
		printf("couldn't load font: SDL error:%s\n", SDL_GetError());
	}
	else {
		mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (mTexture == NULL)
		{
			printf("unable to create texture from surface!SDL Error:%s\n", SDL_GetError());
		}
		else {
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}
		SDL_FreeSurface(loadedSurface);

	}
	return mTexture != NULL;
}

LTexture::LTexture()
{
	mWidth = 0;
	mHeight = 0;
	mTexture = NULL;
}

LTexture::~LTexture()
{
	free();
}

int LTexture::getHeight()
{
	return mHeight;
}

int LTexture::getWidth()
{
	return mWidth;
}