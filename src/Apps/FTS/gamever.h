#ifndef APP_GAMEVER_H
#define APP_GAMEVER_H
enum GameZ_FTSGame
{
    game_NONE,
    game_SOCOM1_BETA,
    game_SOCOM1,
    game_SOCOM2_BETA,
    game_SOCOM2,
    game_SOCOM3,
    game_SOCOMCA
};

struct GameZ_FTSGameInfo
{
    char name[12];
    char version[5];
};

extern GameZ_FTSGameInfo game_info;
extern GameZ_FTSGame game_name;

GameZ_FTSGame GetGame();
#endif