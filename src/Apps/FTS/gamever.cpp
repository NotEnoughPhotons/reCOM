#include "gamever.h"

#include <cstring>

#include "gamez/zSystem/zsys.h"
#include "gamez/zUtil/util_systemio.h"

GameZ_FTSGame game_name;
GameZ_FTSGameInfo game_info;

// TODO: Write a better way to get the game name
GameZ_FTSGame GetGame()
{
    if (game_name != game_NONE)
    {
        return game_name;
    }

    CFileIO file;
    
    if (!strlen(gamez_GameRoot))
    {
        return game_NONE;
    }

    char systemfile_path[256];
    sprintf_s(systemfile_path, 256, "%s/%s", gamez_GameRoot, "SYSTEM.CNF");

    if (!file.Open(systemfile_path))
    {
        sprintf_s(systemfile_path, 256, "%s/%s", gamez_GameRoot, "SYSTEM.ELF");

        if (!file.Open(systemfile_path))
        {
            return game_NONE;
        }
    }
    
    file.fseek(16, SEEK_SET);
    file.fread(game_info.name, 11);
    game_info.name[11] = '\0';
    
    file.Close();
    
    // Data taken from:
    // https://psxdatacenter.com/psx2/ntsc-u_list2.html
    // https://psxdatacenter.com/psx2/pal_list2.html
    // https://psxdatacenter.com/psx2/ntsc-j_list2.html
    bool is_s1_us = !strcmp(game_info.name, "SCUS_971.34") || !strcmp(game_info.name, "SCUS_972.30");  // United States
    bool is_s1_eu = !strcmp(game_info.name, "SCES_509.28") || !strcmp(game_info.name, "SCES_516.18");  // Europe
    bool is_s1_jp = !strcmp(game_info.name, "SCPS_150.44");                                            // Japan
    bool is_s1_ka = !strcmp(game_info.name, "SCKA_200.07") || !strcmp(game_info.name, "SCKA_240.08");  // Korea

    bool is_s1_beta = !strcmp(game_info.name, "SCUS_972.05");

    if (is_s1_beta)
    {
        game_name = game_SOCOM1_BETA;
        return game_name;
    }
    
    if (is_s1_us || is_s1_eu || is_s1_jp || is_s1_ka)
    {
        game_name = game_SOCOM1;
        return game_name;
    }

    // Data taken from:
    // https://psxdatacenter.com/psx2/ntsc-u_list2.html
    // https://psxdatacenter.com/psx2/pal_list2.html
    // https://psxdatacenter.com/psx2/ntsc-j_list2.html
    bool is_s2_us = !strcmp(game_info.name, "SCUS_972.75");                                            // United States
    bool is_s2_eu = !strcmp(game_info.name, "SCES_523.06") || !strcmp(game_info.name, "SCES_519.04");  // Europe
    bool is_s2_jp = !strcmp(game_info.name, "SCPS_150.65");                                            // Japan
    bool is_s2_ka = !strcmp(game_info.name, "SCKA_200.20") || !strcmp(game_info.name, "SCKA_200.53")   // Korea
                 || !strcmp(game_info.name, "SCPS_150.65");  

    bool is_s2_beta = !strcmp(game_info.name, "SCUS_973.68");

    if (is_s2_beta)
    {
        game_name = game_SOCOM2_BETA;
        return game_name;
    }

    if (is_s2_us || is_s2_eu || is_s2_jp || is_s2_ka)
    {
        game_name = game_SOCOM2;
        return game_name;
    }

    return game_NONE;
}
