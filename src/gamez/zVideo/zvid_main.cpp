#include <GL/glew.h>
#include <SDL3/SDL.h>

#include "zvid.h"
#include "Apps/FTS/gamever.h"

#include "gamez/zRender/zrndr_gl.h"
#include "gamez/zReader/zrdr.h"
#include "gamez/zValve/zvalve.h"

_zvid_public zVid;
CValve* lodLevel = NULL;
CWindow* theWindow = NULL;

s32 CVideo::m_defaultFrameRate = 0;

void zVid_Init(_zvid_mode mode)
{
	zVid.runTime = 0.0f;
	zVid.renderBuf = NULL;
	zVid.ztest_on = GL_DEPTH_TEST;
	zVid.ztest_off = GL_DEPTH_TEST;
	zVid.doMpeg224 = false;
	zVid.minZ = 10.0f;
	zVid.pcrtcDo = false;
	zVid.maxZ = 65535.0f;
	zVid.aspect[0] = 1.0f;
	zVid.aspect[1] = 1.0f;
	zVid.displayBuf = NULL;
	zVid.frameRate = 144.0f;
	zVid.hblnkRate = 15750.0f;
	zvid_SetVideoMode(mode);

	zVid_CreateWindow();

	if (lodLevel == NULL)
	{
		lodLevel = CValve::Create("lodLevel", VALVE_TYPE::VTYPE_PERM);
	}

	if ((SDL_GetWindowFlags(theWindow->GetWindow()) & SDL_WINDOW_OPENGL) != 0)
	{
		zVid_Assert(glewInit() == GLEW_OK, LONG_MAX, __FILE__, __LINE__);
	}

	zgl_enable_ztest();

}

void zVid_Open()
{
	
}

void zVid_Sync0()
{

}

void zvid_SetVideoMode(_zvid_mode mode)
{
	zVid.vidMode = mode;
	zVid.videoMode = mode;
}

f32 CVideo::MaxFrameRate(s32 fr)
{
	return 0.0f;
}