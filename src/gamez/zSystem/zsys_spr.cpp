#include "zsys_spr.h"

_word128* sprBeingWritten = NULL;
_word128* sprBeingKicked = NULL;

_word128* zsysSprGetPacket(_word128* packet = NULL)
{
	sprBeingWritten = (_word128*)zSys.sprPacketBegin;

	if (sprBeingWritten == sprBeingKicked)
	{
		sprBeingWritten = (_word128*)(zSys.sprPx ^ 1);
		zSys.sprPx ^= 1;
	}

	if (packet)
		*packet = (_word128&)zSys.sprPacketEnd[zSys.sprPx - 16];

	zSys.sprPx ^= 1;
	return sprBeingWritten;
}

long128* zsys_AllocScratchpad()
{
	long128* spr = (long128*)zcalloc(1024, sizeof(long128));
	zSys.sprPacketBegin = spr;
	return spr;
}

_word128* CSPR::Kick(_word128* packet, bool doKick)
{
	return NULL;
}