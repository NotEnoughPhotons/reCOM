#include "zrndr_gl.h"

#include "GL/glew.h"

s32 zgl_init()
{
	if (glewInit() != GLEW_OK)
		return ZGL_FAIL;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	return ZGL_SUCCESS;
}

void zgl_shutdown()
{
	SDL_GL_DestroyContext(context.ctx);
	SDL_DestroyRenderer(context.renderer);
	SDL_DestroyWindow(context.window);
}

void zgl_set_context(SDL_Window* window, SDL_Renderer* renderer, SDL_GLContext glContext)
{
	context.ctx = glContext;
	context.window = window;
	context.renderer = renderer;

	if (!SDL_GL_MakeCurrent(context.window, context.ctx))
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[GameZ] - ERROR! %s", SDL_GetError());
}

void zgl_create_buffers()
{
	glGenVertexArrays(1, &zgl_vao);
	glGenBuffers(1, &zgl_vbo);
	glGenBuffers(1, &zgl_ebo);
}

void zgl_destroy_buffers()
{
	glDeleteBuffers(1, &zgl_ebo);
	glDeleteBuffers(1, &zgl_vbo);
	glDeleteVertexArrays(1, &zgl_vao);
}

zgl_chain zgl_read_chain(const _word128* chain)
{
	const _word128* addr = chain;

	const u32 size = chain->u8[0];
	const bool setup = chain->u8[2];

	zgl_chain chn;
	chn.qwc = size;
	chn.setup = false;
	chn.data = (void*)chain->u32[2];
	chn.packets.reserve(chn.qwc);

	addr++;

	for (u32 i = 1; i <= size; i++)
	{
		_word128 packet = addr[i];
		zgl_packet chainp;
		
		chainp.qwc = packet.u8[0];
		chainp.type = packet.u8[2];
		chainp.data = (void*)packet.u32[1];

		chn.packets.push_back(chainp);
	}

	return chn;
}

size_t zgl_get_chain_size(const zgl_chain* chain)
{
	return chain->qwc * sizeof(_word128);
}

std::vector<zgl_mesh> zgl_chain_read_meshes(const zgl_chain* chain)
{
	zgl_mesh mesh;
	mesh.index_count = 0;
	mesh.vertex_count = 0;

	std::vector<zgl_mesh> meshes;

	for (u32 i = 0; i < chain->qwc; i++)
	{
		zgl_packet packet = chain->packets[i];
		zgl_mesh m;

		if (packet.type == 2)
		{
			zgl_chain_mesh_process(&packet, &m);
			meshes.push_back(m);
			mesh = m;
		}
	}

	return meshes;
}

void zgl_chain_mesh_process(const zgl_packet* packet, zgl_mesh* mesh)
{
	const _word128* addr = reinterpret_cast<const _word128*>(packet->data);

	addr += 3;

	u32 vertex_count = addr->u32[0];
	u32 index_count = addr->u32[1];

	mesh->vertices.reserve(vertex_count);
	mesh->indices.reserve(index_count);

	mesh->base_vertex = 0;
	mesh->base_index = 0;

	mesh->vertex_count = vertex_count;
	mesh->index_count = index_count;

	addr += 2;

	for (u32 i = 0; i < vertex_count; i++)
	{
		const _word128* word = &addr[i];
		zgl_vertex v;

		v.x = static_cast<s16>(word->u16[0]) / 2.0f;
		v.y = static_cast<s16>(word->u16[1]) / 2.0f;
		v.z = static_cast<s16>(word->u16[2]) / 2.0f;
		v.f = static_cast<s16>(word->u16[3]) / 2.0f;
		v.u = static_cast<s16>(word->u16[4]) / 4096.0f;
		v.v = static_cast<s16>(word->u16[5]) / 4096.0f;

		mesh->vertices.push_back(v);
	}

	// Calculate face normals
	// This is based off of taking the cross product
	// of three points.
	for (u32 i = 0; i < mesh->vertices.size() - 3; i += 3)
	{
		zgl_vertex& p1 = mesh->vertices[i];
		zgl_vertex& p2 = mesh->vertices[i + 1];
		zgl_vertex& p3 = mesh->vertices[i + 2];

		zgl_vertex v1;
		zgl_vertex v2;
		zgl_vertex result;

		// Differences
		v1.x = p2.x - p1.x;
		v1.y = p2.y - p1.y;
		v1.z = p2.z - p1.z;

		v2.x = p3.x - p1.x;
		v2.y = p3.y - p1.y;
		v2.z = p3.z - p1.z;

		// Cross product
		result.x = (v1.y * v2.z) - (v1.z * v2.y);
		result.y = (v1.z * v1.x) - (v1.x * v2.z);
		result.z = (v1.x * v2.y) - (v1.y * v2.x);

		// Then normalize
		f32 length = sqrtf(result.x * result.x + result.y * result.y + result.z * result.z);

		result.x /= length;
		result.y /= length;
		result.z /= length;

		// Store the result in the vertices
		p1.xn = result.x;
		p1.yn = result.y;
		p1.zn = result.z;

		p2.xn = result.x;
		p2.yn = result.y;
		p2.zn = result.z;

		p3.xn = result.x;
		p3.yn = result.y;
		p3.zn = result.z;
	}

	addr += vertex_count;
	addr = (_word128*)(&addr->u64[1]);

	const u8* ptr = reinterpret_cast<const u8*>(addr);

	for (u32 i = 0; i < index_count * 4; i += 4)
	{
		u8 x = ptr[i];
		u8 y = ptr[i + 1];
		u8 z = ptr[i + 2];

		mesh->indices.push_back(x / 3);
		mesh->indices.push_back(y / 3);
		mesh->indices.push_back(z / 3);
	}
}

void zgl_mesh_buffer_create(zgl_mesh_buffer* buffer)
{
	

	for (auto i = buffer->mesh.begin(); i != buffer->mesh.end(); ++i)
	{
		zgl_mesh& mesh = *i;

		glGenVertexArrays(1, &mesh.v_array);

		glGenBuffers(1, &mesh.v_buffer);
		glGenBuffers(1, &mesh.e_buffer);

		glBindVertexArray(mesh.v_array);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.v_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.e_buffer);

		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(zgl_vertex), mesh.vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(u32), mesh.indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(zgl_vertex), (void*)(offsetof(zgl_vertex, x)));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(zgl_vertex), (void*)(offsetof(zgl_vertex, u)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(zgl_vertex), (void*)(offsetof(zgl_vertex, xn)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	

	
}

void zgl_enable_ztest()
{
	glEnable(GL_DEPTH_TEST);
}

void zgl_disable_ztest()
{
	glDisable(GL_DEPTH_TEST);
}