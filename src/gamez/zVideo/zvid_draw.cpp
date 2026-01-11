#include <GL/glew.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_render.h>

#include "zvid.h"

#include "gamez/zUtil/util_systemio.h"

f32 lastRenderTime = 0.0f;
f32 curRenderTime = 0.0f;

void zVid_FrameRestore()
{

}

void zVid_Swap(bool doSwap)
{
	lastRenderTime = 0.0f;
	curRenderTime = 0.0f;

	lastRenderTime = SDL_GetTicks();
	SDL_GL_SwapWindow(theWindow->GetWindow());
	curRenderTime = SDL_GetTicks();

	// zVid.frameTime = (curRenderTime - lastRenderTime) * zSys.timerScale;
	// zVid.frameRate = 1.0f / zVid.frameTime + 0.5f;
	// zVid.runTime += zVid.frameTime;
	// zVid.frameNumber++;
}

void zVid_ClearColor(f32 red, f32 green, f32 blue)
{
	glClearColor(red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void CVideo::RestoreImage(const char* img, bool raw)
{
	if (!raw)
	{
		SDL_Renderer* renderer = theWindow->GetRenderer();
		return;
	}
	
	void* buf = NULL;
	CFileIO file;
	if (file.Open(img))
	{
		buf = zmalloc(file.m_filesize);
		file.fread(buf, file.m_filesize);
		file.Close();
		
		SDL_Renderer* renderer = theWindow->GetRenderer();

		// Normally you would know the width and height from the image's metadata.
		// However, Zipper assumed that the width and height of the game screen would always be 640x448.
		// The RAW format only contains raw pixel data (stored in reverse), nothing else.
		// For now, just stretch the image to the screen.
		// TODO: Write a way to allow for any image to be passed to CVideo::RestoreImage
		SDL_Surface* surface = SDL_CreateSurfaceFrom(640, 448, SDL_PIXELFORMAT_XBGR8888, buf, 640 * (sizeof(u8) * 4));

		if (!surface)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
			return;
		}
		
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		if (!texture)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
			return;
		}
		
		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		SDL_DestroySurface(surface);
		zfree(buf);
	}
}
