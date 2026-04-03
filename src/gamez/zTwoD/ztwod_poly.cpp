#include "ztwod.h"
#include "GL/glew.h"
#include "gamez/zRender/zrndr_gl.h"
#include "gamez/zMath/zmath_vu.h"
#include "gamez/zShader/zshader.h"
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

	m_shader = new CShader();

	char buf[256];
	sprintf_s(buf, 256, "data/shaders/%s.vertex", "z2d");
	m_shader->LoadVertexShader(buf);
	memset(buf, 0, 256);
	sprintf_s(buf, 256, "data/shaders/%s.fragment", "z2d");
	m_shader->LoadFragmentShader(buf);
	memset(buf, 0, 256);

	m_shader->Create();

	m_mesh = new zgl_mesh();

	m_buffer = new zgl_mesh_buffer();
	m_buffer->mesh.reserve(1);
}

void C2DPoly::SetColor(f32 r, f32 g, f32 b)
{
	m_RGB[0][0] = r;
	m_RGB[0][1] = g;
	m_RGB[0][2] = b;
	m_RGB[1][0] = r;
	m_RGB[1][1] = g;
	m_RGB[1][2] = b;
	m_RGB[2][0] = r;
	m_RGB[2][1] = g;
	m_RGB[2][2] = b;
}

void C2DPoly::Draw(zdb::CCamera* camera)
{
	MakePacket(camera);
	C2D::Draw(camera);

	if (!m_buffer || m_buffer->mesh.size() == 0)
	{
		glBindVertexArray(0);
		return;
	}

	m_shader->Use();

	for (auto i = m_buffer->mesh.begin(); i != m_buffer->mesh.end(); ++i)
	{
		zgl_mesh& mesh = *i;

		glBindVertexArray(mesh.v_array);
		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);
	}
}

void C2DPoly::MakePacket(zdb::CCamera* camera)
{

}

void C2DPoly::Load(f32 v0x, f32 v0y, f32 v1x, f32 v1y, f32 v2x, f32 v2y, zdb::CTexture* texture)
{
	m_texture = texture;

	m_x = static_cast<s32>(v0x);
	m_y = static_cast<s32>(v0y);

	m_vertex[0][0] = v0x * C2D::m_fXPixel;
	m_vertex[0][1] = v0y * C2D::m_fYPixel;
	m_vertex[1][0] = v1x * C2D::m_fXPixel;
	m_vertex[1][1] = v1y * C2D::m_fYPixel;
	m_vertex[2][0] = v2x * C2D::m_fXPixel;
	m_vertex[2][1] = v2y * C2D::m_fYPixel;

	m_mesh->vertices.clear();
	m_mesh->indices.clear();

	zgl_vertex v1;
	zgl_vertex v2;
	zgl_vertex v3;

	memset(&v1, 0, sizeof(zgl_vertex));
	memset(&v2, 0, sizeof(zgl_vertex));
	memset(&v3, 0, sizeof(zgl_vertex));

	v1.x = m_vertex[0][0] - 0.125f;
	v1.y = m_vertex[0][1] - 0.125f;
	v1.z = 0.0f;
	//v1.u = m_NewUV[0][1];
	//v1.v = m_NewUV[0][2];

	v2.x = m_vertex[1][0] - 0.125f;
	v2.y = m_vertex[1][1] - 0.125f;
	v2.z = 0.0f;
	//v2.u = m_NewUV[1][1];
	//v2.v = m_NewUV[1][2];

	v3.x = m_vertex[2][0] - 0.125f;
	v3.y = m_vertex[2][1] - 0.125f;
	v3.z = 0.0f;
	//v3.u = m_NewUV[2][1];
	//v3.v = m_NewUV[2][2];

	//v1.x = -0.5f;
	//v1.y = -0.5f;
	//v2.x = 0.5f;
	//v2.y = -0.5f;
	//v3.x = 0.0f;
	//v3.y = 0.5f;

	m_mesh->vertices.push_back(v1);
	m_mesh->vertices.push_back(v2);
	m_mesh->vertices.push_back(v3);

	m_mesh->indices.push_back(0);
	m_mesh->indices.push_back(1);
	m_mesh->indices.push_back(2);

	m_buffer->mesh.push_back(*m_mesh);

	PNT4D col;
	col.x = m_RGB[0][0];
	col.y = m_RGB[0][1];
	col.z = m_RGB[0][2];
	col.w = 1.0f;

	m_shader->SetVec4("ourColor", col);

	zgl_mesh_buffer_create(m_buffer);
}

void C2DBitmapPoly::SetColor(f32 r, f32 g, f32 b)
{
	m_tris[0].SetColor(r, g, b);
	m_tris[1].SetColor(r, g, b);
}

void C2DBitmapPoly::Load(f32 v0x, f32 v0y, f32 v1x, f32 v1y, f32 v2x, f32 v2y, f32 v3x, f32 v3y, zdb::CTexture* texture)
{
	m_tris[0].Load(v0x, v0y, v1x, v1y, v2x, v2y, texture);
	m_tris[1].Load(v0x, v0y, v2x, v2y, v3x, v3y, texture);
}

void C2DBitmapPoly::Draw(zdb::CCamera* camera)
{
	m_tris[0].Draw(camera);
	m_tris[1].Draw(camera);
}