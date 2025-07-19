#include "gamez/zGame/zgame.h"
#include "gamez/zSound/zsnd.h"
#include "gamez/zSystem/zsys.h"

#include "SDL3/SDL_log.h"

// Uncomment to not use a game directory
// Useful for external testing
#define NOGAME

bool LoadWorld(const char* name);
char* SetDatabase(const char* db);

int main(int argc, char** argv)
{
	process_arguments(argc, argv);
	zSysInit();
	zSysPostInit();

#ifndef NOGAME
	SDL_Log("%s selected for GameZ.", gamez_GameRunPath);
#endif
	
	if (dumpLog)
	{
		COutput::DumpLog("ftslog.txt");
	}

	char* log = NULL;

	if (gopt.doLog)
	{
		log = "ftslog.txt";
	}

	zUtilInit(log);

	SetDatabase("ui");
	
	// theGame.StartEngine();
	// theGame.StartPlay();

	CRdrFile* file = zrdr_read("data/s1/common/zrdr/ai_moves.rdr");
	file->Compile();
	
	/* while (true)
	{
		theGame.Tick(0);
	}*/
}