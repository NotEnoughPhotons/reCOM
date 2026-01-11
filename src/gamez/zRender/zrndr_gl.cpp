#include "zrndr_gl.h"

#include "GL/glew.h"

s32 zgl_init()
{
	if (glewInit() != GLEW_OK)
		return ZGL_FAIL;

	return ZGL_SUCCESS;
}

void zgl_shutdown()
{
	SDL_GL_DestroyContext(context->ctx);
	SDL_DestroyRenderer(context->renderer);
	SDL_DestroyWindow(context->window);
}

void zgl_set_context(zgl_context* ctx)
{
	context = ctx;
}

zgl_mesh_packet zgl_read_packet(const _word128* chain)
{
	const u32 size = chain->u8[0]; // Size of the chain in quadwords
	const bool setup = chain->u8[2];

	for (u32 i = 1; i < size + 1; i++)
	{
		_word128 packet = chain[i];
		u32 qwc = packet.u8[0]; // Size of the packet in quadwords
		u8 type = packet.u8[2];
		_word128* meshpkt = (_word128*)packet.u32[1];

		if (type == 2)
			return zgl_read_mesh_packet(meshpkt);
	}
}

zgl_mesh_packet zgl_read_mesh_packet(const void* packet)
{
	const u8* addr = reinterpret_cast<const u8*>(packet);

	zgl_mesh_packet mesh;
	zgl_mesh_packet_hdr* header = (zgl_mesh_packet_hdr*)addr;
	zgl_vertex_packet* vertices = static_cast<zgl_vertex_packet*>(zcalloc(header->vertex_count, sizeof(zgl_vertex_packet)));

	memcpy(vertices, addr + sizeof(zgl_mesh_packet_hdr), header->vertex_count * sizeof(zgl_vertex_packet));

	mesh.header = *header;
	mesh.vertices = vertices;

	return mesh;
}

zgl_mesh zgl_convert_mesh_packet(const zgl_mesh_packet* packet)
{
	zgl_mesh mesh;
	mesh.index_count = packet->header.index_count;
	mesh.vertex_count = packet->header.vertex_count;

	mesh.vertices = static_cast<zgl_vertex*>(zcalloc(mesh.vertex_count, sizeof(zgl_vertex)));

	for (u32 i = 0; i < mesh.vertex_count; i++)
	{
		zgl_vertex_packet vp = packet->vertices[i];
		zgl_vertex v;

		v.x = vp.x / 16.0f;
		v.y = vp.y / 16.0f;
		v.z = vp.z / 16.0f;
		v.index = vp.index;
		v.u = vp.u / 4096.0f;
		v.v = vp.v / 4096.0f;
		v.flags = vp.flags;

		mesh.vertices[i] = v;
	}

	return mesh;
}