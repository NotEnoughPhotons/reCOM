#include "ztwod.h"
#include "GL/glew.h"
#include "gamez/zRender/zrndr_gl.h"
#include "gamez/zMath/zmath_vu.h"
#include "gamez/zVideo/zvid.h"

C2DPoly::C2DPoly()
{
	m_vertex[0][1] = -0.000006f;
	m_vertex[0][2] = -0.000004f;
	m_vertex[0][3] = 0.0001f;
	m_vertex[1][0] = 1.0f;
	m_vertex[1][1] = -0.000003f;
	m_vertex[1][2] = -0.000002f;
	m_vertex[1][3] = 0.0001f;
	m_vertex[2][0] = 1.0f;
	m_vertex[2][1] = -0.000003f;
	m_vertex[2][2] = -0.000002f;
	m_vertex[2][3] = 0.0001f;

	m_x = 0;
	m_y = 0;

	m_NewUV[0][1] = 0.0f;
	m_NewUV[0][2] = 0.0f;
	m_NewUV[0][3] = 1.0f;
	m_NewUV[1][1] = 1.0f;
	m_NewUV[1][2] = 1.0f;
	m_NewUV[1][3] = 1.0f;
	m_NewUV[2][0] = 1.0f;
	m_NewUV[2][1] = 1.0f;
	m_NewUV[2][2] = 1.0f;
	m_NewUV[2][3] = 1.0f;

	m_vertex[0][0] = 0.0f;

	m_RGB[0][1] = 255.0;
	m_RGB[0][2] = 255.0;
	m_RGB[0][3] = 255.0;
	m_RGB[1][0] = 100.0;
	m_RGB[1][1] = 100.0;
	m_RGB[1][2] = 100.0;
	m_RGB[1][3] = 100.0;
	m_RGB[2][0] = 100.0;
	m_RGB[2][1] = 100.0;
	m_RGB[2][2] = 100.0;
	m_RGB[2][3] = 100.0;

	m_NewUV[0][0] = 100.0f;

	m_texture = NULL;

	m_on = false;

	m_RGB[0][1] = 128.0f;
	m_RGB[0][2] = 128.0f;
	m_RGB[0][3] = 128.0f;
	m_RGB[1][1] = 128.0f;
	m_RGB[1][2] = 128.0f;
	m_RGB[1][3] = 128.0f;
	m_RGB[2][1] = 128.0f;
	m_RGB[2][2] = 128.0f;
	m_RGB[2][3] = 128.0f;

	SetTrans(128.0f);
}

void C2DPoly::Draw(zdb::CCamera* camera)
{
	MakePacket(camera);
	C2D::Draw(camera);
}

void C2DPoly::MakePacket(zdb::CCamera* camera)
{
	//glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, theWindow->GetWidth(), theWindow->GetHeight(), 0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glEnd();
	glMatrixMode(GL_MODELVIEW);
	//glPopMatrix();
}

void C2DPoly::Load(f32 v0x, f32 v0y, f32 v1x, f32 v1y, f32 v2x, f32 v2y, zdb::CTexture* texture)
{
	m_texture = texture;
	m_x = static_cast<s32>(v0x);
	m_y = static_cast<s32>(v0y);

	m_vertex[0][1] = C2D::m_topx + v0x * C2D::m_fXPixel;
	m_vertex[0][2] = C2D::m_topy + v0y * C2D::m_fYPixel;
	m_vertex[1][1] = C2D::m_topx + v1x * C2D::m_fXPixel;
	m_vertex[1][2] = C2D::m_topy + v1y * C2D::m_fYPixel;
	m_vertex[2][1] = C2D::m_topx + v2x * C2D::m_fXPixel;
	m_vertex[2][2] = C2D::m_topy + v2y * C2D::m_fYPixel;
}