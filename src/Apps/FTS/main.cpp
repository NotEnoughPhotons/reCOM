#include "gamez/zGame/zgame.h"
#include "gamez/zSound/zsnd.h"
#include "gamez/zSystem/zsys.h"

#include "SDL3/SDL_log.h"

bool LoadWorld(const char* name);
char* SetDatabase(const char* db);

int main(int argc, char** argv)
{
	process_arguments(argc, argv);
	zSysInit();
	zSysPostInit();

	SDL_Log("[GameZ] - %s selected as game path.", gamez_GameRunPath);
	
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
	
	theGame.StartEngine();
	theGame.StartPlay();

	while (true) theGame.Tick(0);
}