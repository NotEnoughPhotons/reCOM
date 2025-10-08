#include "zsys.h"

#include <iostream>
#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_dialog.h>
#include <SDL3/SDL_thread.h>
#include <SDL3/SDL_ttf.h>

#include "Apps/FTS/gamever.h"
#include "gamez/zReader/zrdr.h"
#include "gamez/zVideo/zvid.h"

bool postinited = false;
size_t _HeapSize = 0;

SDL_Condition* wait_cond = NULL;
bool path_failed = false;
bool path_inited = false;
char gamez_GameRoot[256];
char gamez_GameRunPath[256];

SDL_Thread* file_thread;

void zSys_OpenFileDialog(void* userdata, const char * const *filelist, int filter)
{
	if (!*filelist)
	{
		path_failed = true;
		path_inited = true;
		SDL_SignalCondition(wait_cond);
		return;
	}
	
	size_t length = strlen(*filelist);
	
	if (length > 0)
	{
		path_failed = false;
		path_inited = true;
		strcpy_s(gamez_GameRoot, 256, *filelist);
		strcpy_s(gamez_GameRunPath, 256, *filelist);
		SDL_SignalCondition(wait_cond);
	}
}

int InterruptDmacBusError()
{
	printf("DMAC Bus Error Interrupt (dma chain error)\n");
	printf("Please contact you neighborhood friendly software expert\n");
	abort();
}

void zSysInit()
{
	// size_t allocsize = zsys_FullAllocAndFree();
	// zSys.isT10K = 0x1ffffff < allocsize;
	u32 flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK;
	zVid_Assert(SDL_Init(flags), LONG_MAX, __FILE__, __LINE__);
	
	zSys.isCdBoot = false;

	if (!TTF_Init())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load SDL TTF!");
	}
}

void zSysReset()
{
	zSys.timerTicksPerSecond = static_cast<u32>(zVid.hblnkRate);
	zSys.timerScale = 1.0f / zSys.timerTicksPerSecond;
}

size_t zsys_FullAllocAndFree()
{
	void* ptr = NULL;
	size_t size = 0;

	size = 4096;

	do
	{
		ptr = malloc(size);

		if (ptr == NULL)
		{
			break;
		}

		free(ptr);

		size += 4096;

	} while (size < 0x2000001);

	_HeapSize = size - 16;

	return _HeapSize;
}

void zSysPostInit()
{
	if (!postinited)
	{
		postinited = true;
	}

	SDL_Mutex* mutex = SDL_CreateMutex();
	wait_cond = SDL_CreateCondition();
	
	SDL_LockMutex(mutex);
	SDL_ShowOpenFolderDialog(zSys_OpenFileDialog, NULL, NULL, "D:/", false);
	SDL_WaitCondition(wait_cond, mutex);
	SDL_UnlockMutex(mutex);

	SDL_DestroyMutex(mutex);
	mutex = NULL;

	SDL_DestroyCondition(wait_cond);
	wait_cond = NULL;
	
	if (path_failed)
	{
		const SDL_MessageBoxButtonData buttons[]
		{
			{
				SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT,
				0,
				"OK"
			}
		};
		
		const SDL_MessageBoxData data =
		{
			SDL_MESSAGEBOX_ERROR,
			NULL,
			"GameZ - Initialization Error",
			"You must provide a valid game directory for GameZ to run!",
			1,
			buttons,
			NULL
		};
		
		SDL_ShowMessageBox(&data, NULL);
		SDL_Quit();
		exit(-1);
	}
	
	strcat_s(gamez_GameRunPath, 256, "/RUN");
	if (!SDL_GetPathInfo(gamez_GameRunPath, NULL))
	{
		const SDL_MessageBoxButtonData buttons[]
		{
			{
				SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT,
				0,
				"OK"
			}
		};
		
		const SDL_MessageBoxData data =
		{
			SDL_MESSAGEBOX_ERROR,
			NULL,
			"GameZ - Initialization Error",
			"Game directory does not have a RUN folder!",
			1,
			buttons,
			NULL
		};
		
		SDL_ShowMessageBox(&data, NULL);
		SDL_Quit();
		exit(-1);
	}

	GetGame();
}

void zVid_Assert(bool condition, unsigned int mask, const char* file, int line)
{
	if (!condition)
	{
		// zSysFifoEnd();
		char buffer[256];
		sprintf_s(buffer, 256, "A S S E R T: %s : %d", file, line);
		theTerminal.Print(buffer, 256);
		theTerminal.Render();
		// abort();
	}
}

void* operator new(size_t size)
{
	return __malloc(size, __FILE__, __LINE__);
}

void* __malloc(size_t size, const char* file, int line)
{
	void* p = malloc(size);
	zVid_Assert(size == 0 || p != NULL, INT_MAX, __FILE__, __LINE__);
	return p;
}

void* __calloc(size_t num, size_t size, const char* file, int line)
{
	size_t calc_size = num * size;
	void* p = malloc(calc_size);

	if (p == NULL)
	{
		zVid_Assert(false, INT_MAX, __FILE__, __LINE__);
		return NULL;
	}

	memset(p, 0, calc_size);
	zVid_Assert(calc_size == 0 || p != NULL, INT_MAX, __FILE__, __LINE__);
	return p;
}

void* __realloc(void* ptr, size_t new_size, const char* file, int line)
{
	void* p = realloc(ptr, new_size);
	zVid_Assert(new_size == 0 || p != NULL, INT_MAX, __FILE__, __LINE__);
	return p;
}

void* __memalign(size_t alignment, size_t size, const char* file, int line)
{
	void* p = _aligned_malloc(size, alignment);
	zVid_Assert(alignment == 0 || p != NULL, INT_MAX, __FILE__, __LINE__);
	return p;
}

char* __strdup(const char* str, const char* file, int line)
{
	size_t len = strlen(str);
	len++;
	char* p = (char*)malloc(len);

	if (p == NULL)
	{
		zVid_Assert(false, INT_MAX, __FILE__, __LINE__);
		return NULL;
	}

	strcpy(p, str);
	zVid_Assert(len == 0 || p != NULL, INT_MAX, __FILE__, __LINE__);
	return p;
}

void __free(void* block, const char* file, int line)
{
	free(block);
}