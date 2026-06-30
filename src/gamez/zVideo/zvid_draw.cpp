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

	// zVid.frameTime = (curRenderTime - lastRenderTime) * zSys.timerScale;
	// zVid.frameRate = 1.0f / zVid.frameTime + 0.5f;
	// zVid.runTime += zVid.frameTime;
	// zVid.frameNumber++;
}

void zVid_ClearColor(f32 red, f32 green, f32 blue)
{
	
}

void CVideo::RestoreImage(const char* img, bool raw)
{
	if (!raw)
	{
		return;
	}
	
	void* buf = NULL;
	CFileIO file;
	if (file.Open(img))
	{
		buf = zmalloc(file.m_filesize);
		file.fread(buf, file.m_filesize);
		file.Close();
		zfree(buf);
	}
}
