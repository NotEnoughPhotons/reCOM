#include "zvehicle.h"

#include "gamez/zReader/zrdr.h"
#include "gamez/zMath/zmath.h"

CVehicleRdrEntry* _dest = NULL;

bool CVehicleRdr::Open(const char* name)
{
	CRdrFile* file = zrdr_read(name, NULL, 0);
	bool success = Load(file);
	zrdr_free(file);
	return success;
}

void CVehicleRdr::Close()
{
	clear();
}

bool CVehicleRdr::Load(_zrdr* file)
{
	_zrdr* node = NULL;
	FIRETEAM team = FIRETEAM::FT_MP_PLAYER;

	bool debug, disabled, nofade, noshoot, recycle, nosnooze = false;

	if (!file)
		return false;

	resize(file->array->integer - 1);

	for (u32 i = 0; node = &file->array[i], i < file->array->integer - 1; i++)
	{
		_dest = &data()[i];

		char* name = zrdr_findstring(node, "name");
		char* display = zrdr_findstring(node, "display");

		_zrdr* type_node = NULL;

		if (_zrdr* team_tag = zrdr_findtag(node, "team"))
		{
			for (u32 j = 1; type_node = &team_tag->array[j], j < team_tag->array->integer - 1; j++)
			{
				team = (FIRETEAM)type_node->integer;
				_dest->m_teamMask = 1 << ((u8)team & 31);
			}
		}

		zrdr_findbool(node, "debug", &debug);
		zrdr_findbool(node, "disabled", &disabled);
		zrdr_findbool(node, "nofade", &nofade);
		zrdr_findbool(node, "noshoot", &noshoot);
		zrdr_findbool(node, "recycle", &recycle);
		zrdr_findbool(node, "nosnooze", &nosnooze);

		char* character = zrdr_findstring(node, "character");
		_dest->m_aiParams.Parse(zrdr_findtag(node, "ai_params"));

		strncpy(_dest->m_name, name, 32);
		strncpy(_dest->m_display_name, display, 32);
		strncpy(_dest->m_character, character, 48);

		_dest->m_debug = debug;
		_dest->m_disabled = disabled;
		_dest->m_nofade = nofade;
		_dest->m_noshoot = noshoot;
		_dest->m_recycle = recycle;
		_dest->m_nosnooze = nosnooze;

		if (!_dest->m_display_name)
		{
			if (SDL_strcasecmp(name, "player") == 0)
				strncpy(_dest->m_display_name, "Kahuna", 32);

			if (SDL_strcasecmp(name, "Hutchins") == 0)
				strncpy(_dest->m_display_name, "Boomer", 32);

			if (SDL_strcasecmp(name, "Dimone") == 0)
				strncpy(_dest->m_display_name, "Spectre", 32);

			if (SDL_strcasecmp(name, "Bailey") == 0)
				strncpy(_dest->m_display_name, "Jester", 32);
		}

		if (_zrdr* start = zrdr_findtag(node, "start"))
		{
			char* start_name = NULL;
			s32 direction = 0;

			if (start->array->integer > 2)
				start = &start->array[2];

			if (start->type == ZRDR_STRING)
				start_name = start->string;

			zrdr_findint(start, "dir", &direction, 1);
			_dest->m_start.m_count = 1;
			saturate<u32>(_dest->m_start.m_count, 0, 4);

			strncpy(_dest->m_start.m_start->m_name, start_name, 16);
			_dest->m_start.m_start[0].m_dir = direction;
		}
		else if (_zrdr* setup = zrdr_findtag(node, "setup"))
		{
			_zrdr* setup_node = NULL;
			for (u32 j = 0; setup_node = &setup->array[j], j != setup->array->integer - 1; j++)
			{
				s32 direction = 0;

				if (setup_node->type == ZRDR_INTEGER)
					direction = setup_node->integer;

				if (setup_node->type == ZRDR_STRING)
					strncpy(_dest->m_start.m_start->m_name, setup_node->string, 16);

				_dest->m_start.m_start[j].m_dir = direction;
			}
		}
	}

	return true;
}

CVehicleRdrEntry* CVehicleRdr::GetEntryByName(const char* name)
{
	if (!name)
		return NULL;

	for (auto i = begin(); i != end(); i++)
	{
		CVehicleRdrEntry entry = *i;
		if (SDL_strcasecmp(entry.m_name, name) == 0)
			return &entry;
	}

	return NULL;
}

void CVehicleRdrEntry::SetCharacterName(const char* name)
{
	if (!name)
		m_character[0] = NULL;
	else
		strncpy(m_character, name, 48);
}