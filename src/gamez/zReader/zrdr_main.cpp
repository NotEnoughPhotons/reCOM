#include "zrdr.h"

#include <cstring>

#include "zrdr_local.h"
#include "Apps/FTS/gamever.h"
#include "gamez/zMath/zmath.h"
#include "gamez/zSystem/zsys.h"
#include "gamez/zUtil/util_systemio.h"
#include "SDL3/SDL_stdinc.h"

bool zrdr_init = false;
bool warnonce = false;

char cur_zrdr_path[256];
s32 cur_zrdr_flags = 0;

_zrdr::_zrdr()
{
	type = ZRDR_NULL;
	isclone = false;
	packed = false;
	unused = 0;
	length = 0;

	array = NULL;
}

_zrdr::_zrdr(_zrdr* other, CSTable* stable)
{
	type = ZRDR_NULL;
	isclone = false;
	packed = false;
	unused = 0;
	length = 0;

	Clone(other, stable);
}

bool _zrdr::IsArray() const
{
	return type == ZRDR_ARRAY;
}

void _zrdr::Clone(const _zrdr* other, const CSTable* table)
{
	if (!zrdr_init)
	{
		warnonce = true;
		zrdr_init = true;
	}

	if (other == NULL)
	{
		return;
	}
	
	type = other->type;
	if (type == ZRDR_ARRAY)
	{

	}
}

int _zrdr::GetInt() const
{
	if (type == ZRDR_INTEGER)
		return integer;
	else if (type == ZRDR_REAL)
		return static_cast<s32>(real);

	return 0;
}

_zrdr* _zrdr::Get(s32 offset) const
{
	u32 len = 0;

	if (type == ZRDR_ARRAY)
		len = array->integer - 1;

	if (offset < len)
		return &array[offset + 1];

	return NULL;
}

bool _zrdr::Write(FILE* file)
{
	return _OutputASCII(file, this, 0);
}

_zrdr* zrdr_findtag(_zrdr* reader, const char* name)
{
	if (GetGame() <= game_SOCOM1)
		return zrdr_findtag_startidx(reader, name, 1);
	else if (GetGame() > game_SOCOM1)
		return zrdr_findtag_startidx(reader, name, 0);
}

_zrdr* zrdr_findtag_startidx(_zrdr* reader, const char* name, u32 startidx)
{
	if (!reader)
		return NULL;

	if (reader->type != ZRDR_ARRAY)
		return NULL;

	u32 length = 0;

	if (GetGame() <= game_SOCOM1)
		length = reader->array->integer;
	else if (GetGame() == game_SOCOM2_BETA || GetGame() == game_SOCOM2)
		length = reader->length;
	
	for (; startidx < length; startidx++)
	{
		_zrdr* node = &reader->array[startidx];

		if (node->type == ZRDR_ARRAY)
		{
			node = zrdr_findtag_startidx(node, name, 1);

			if (node)
				return node;
		}
		else if (node->type == ZRDR_STRING && strcmp(node->string, name) == 0)
			return &node[1];
	}

	return NULL;
}

char* zrdr_findstring(_zrdr* reader, const char* name)
{
	auto tag = zrdr_findtag_startidx(reader, name, 1);

	if (!tag)
		return NULL;

	if (tag->type == ZRDR_STRING)
		return tag->string;

	if (tag->type == ZRDR_ARRAY && tag->array[1].type == ZRDR_STRING)
		return tag->array[1].string;

	return NULL;
}

bool zrdr_findSTRING(_zrdr* reader, const char* name, const char** list)
{
	auto tag = zrdr_findtag_startidx(reader, name, 1);

	if (!tag)
		return false;

	if (!list)
		return false;

	if (tag->type )

	if (tag->type == ZRDR_ARRAY)
	{
		if (tag->array->length > 1)
			tag = &tag->array[1];
	}

	if (tag->type == ZRDR_STRING)
	{
		*list = tag->string;
		return true;
	}

	if (tag->type == ZRDR_INTEGER && tag->integer == 0)
	{
		*list = NULL;
		return true;
	}

	return false;
}

bool zrdr_findreal(_zrdr* reader, const char* name, f32* output, s32 startidx)
{
	_zrdr* tag = zrdr_findtag_startidx(reader, name, 1);

	if (!tag)
		return false;

	if (tag->type != ZRDR_ARRAY)
		return false;

	if (tag->array->integer < startidx + 1)
		return false;

	for (u32 i = 0; i < startidx; i++)
	{
		f32 real = 0.0f;
		_zrdr* node = &tag->array[startidx];

		if (node->type == ZRDR_REAL)
			real = node->real;

		if (node->type == ZRDR_INTEGER)
			real = static_cast<f32>(node->integer);

		*output = real;
	}

	return true;
}

bool zrdr_findint(_zrdr* reader, const char* name, s32* output, s32 startidx)
{
	_zrdr* tag = zrdr_findtag_startidx(reader, name, 1);

	if (!tag)
		return false;

	if (tag->type != ZRDR_ARRAY)
		return false;

	if (tag->array->integer < startidx + 1)
		return false;

	for (u32 i = 0; i < startidx; i++)
	{
		s32 integer = 0;
		_zrdr* node = tag->array;

		if (node->type == ZRDR_REAL)
			integer = static_cast<s32>(node->real);

		if (node->type == ZRDR_INTEGER)
			integer = node->integer;

		*output = integer;
	}

	return true;
}

bool zrdr_finduint(_zrdr* reader, const char* name, u32* output, s32 startidx)
{
	_zrdr* tag = zrdr_findtag_startidx(reader, name, startidx);
	return zrdr_toINT(tag, reinterpret_cast<s32*>(output), startidx);
}

bool zrdr_findbool(_zrdr* reader, const char* tag, bool* output)
{
	_zrdr* rdr = zrdr_findtag_startidx(reader, tag, 1);
	return zrdr_tobool(rdr, output);
}

bool zrdr_findPNT2D(_zrdr* reader, const char* name, PNT2D* output)
{
	_zrdr* tag = zrdr_findtag_startidx(reader, name, 1);

	if (!tag)
		return false;

	if (tag->type != ZRDR_ARRAY)
		return false;

	if (tag->array->integer < 3)
		return false;

	f32 axis = 0.0f;

	for (u32 i = 0; i < 2; i++)
	{
		_zrdr* node = &tag->array[1];

		if (node[i].type == ZRDR_REAL)
			axis = node[i].real;

		if (node[i].type == ZRDR_INTEGER)
			axis = static_cast<f32>(node[i].integer);

		(&output->x)[i] = axis;
	}

	return true;
}

bool zrdr_findPNT3D(_zrdr* reader, const char* name, PNT3D* output)
{
	_zrdr* tag = zrdr_findtag_startidx(reader, name, 1);

	if (!tag)
		return false;

	if (tag->type != ZRDR_ARRAY)
		return false;

	if (tag->array->integer < 3)
		return false;

	f32 axis = 0.0f;

	for (u32 i = 0; i < 3; i++)
	{
		_zrdr* node = &tag->array[1];

		if (node[i].type == ZRDR_REAL)
			axis = node[i].real;

		if (node[i].type == ZRDR_INTEGER)
			axis = static_cast<f32>(node[i].integer);

		(&output->x)[i] = axis;
	}

	return true;
}

bool zrdr_toINT(_zrdr* reader, s32* output, s32 size)
{
	if (!reader)
		return false;

	if (reader->type != ZRDR_ARRAY)
		return false;

	if (reader->array->integer < size + 1)
		return false;

	for (u32 i = 1; i < size + 1; i++)
	{
		s32 integer = 0;
		_zrdr* node = &reader->array[i];

		if (node->type == ZRDR_REAL)
			integer = static_cast<s32>(node->real);

		if (node->type == ZRDR_INTEGER)
			integer = node->integer;

		*output = integer;
	}

	return true;
}

bool zrdr_toREAL(_zrdr* reader, f32* output, s32 size)
{
	if (!reader)
		return false;

	if (reader->type != ZRDR_ARRAY)
		return false;

	if (reader->array->integer < size + 1)
		return false;

	for (u32 i = 1; i < size; i++)
	{
		s32 integer = 0;
		_zrdr* node = &reader->array[i];

		if (node->type == ZRDR_INTEGER)
			integer = static_cast<f32>(node->integer);

		if (node->type == ZRDR_REAL)
			integer = node->real;

		*output = integer;
	}

	return true;
}

bool zrdr_tobool(_zrdr* reader, bool* output)
{
	if (!reader)
		return false;

	// Invalid pointer
	if (!output)
		return false;

	if (reader->type == ZRDR_ARRAY && reader->array->integer > 1)
		reader = reader->array;

	if (reader->type == ZRDR_INTEGER)
	{
		if (reader->integer == 0)
			*output = false;
		else
			*output = true;
	}

	if (reader->type == ZRDR_STRING)
	{
		if (SDL_strcasecmp(reader->string, "true") == 0 || SDL_strcasecmp(reader->string, "on") == 0)
		{
			*output = true;
			return true;
		}
		else if (SDL_strcasecmp(reader->string, "false") == 0 || SDL_strcasecmp(reader->string, "off") == 0)
		{
			*output = false;
			return true;
		}
	}

	return false;
}

_zrdr* _zrdr_nexttag(_zrdr* reader, const char* name, size_t size, _zrdr* other)
{
	_zrdr* node = reader->array;

	if (node->integer <= 1)
		return NULL;

	if (node == other)
		other = NULL;

	if (node->type == ZRDR_ARRAY)
		return _zrdr_nexttag(node, name, size, other);

	if (node->type == ZRDR_STRING && !other)
	{
		if (strncmp(node->string, name, size) == 0)
			return &node[1];
	}

	return NULL;
}

void zrdr_freearray(_zrdr* reader)
{
	_zrdr* node = NULL;

	if (!reader)
		return;

	if (reader->type == ZRDR_STRING)
		reader->string = NULL;

	if (reader->type != ZRDR_ARRAY)
		return;

	for (u32 i = 0; node = &reader->array[i], i <= reader->array->integer; i++)
	{
		if (node->type == ZRDR_STRING)
			node->string = NULL;

		if (node->type == ZRDR_ARRAY && node->integer != 0)
		{
			for (u32 j = 0; j < node->array->integer; j++)
				zrdr_freearray(&node->array[j]);

			zfree(node->array);
			node->array = NULL;
		}
	}

	zfree(reader->array);
	reader->array = NULL;
}