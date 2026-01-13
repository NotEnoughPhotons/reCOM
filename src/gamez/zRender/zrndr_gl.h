/// ----------------
/// PURPOSE:
/// OpenGL functions for preparing and rendering PS2 geometry.
/// 
/// GameZ uses packets and chains to do rendering on the PS2.
/// On PC, this not viable. However, we can take the raw packet data -
/// and prepare them for rendering through OpenGL.
/// 
/// Pre-processing has to be done on the information in order for it to be usable.
/// On the PS2, this is handled using VU0 and VU1 microcode.
/// 
/// [PACKET STRUCTURE]
///		[Position]
///			vx -> s16
///				Conversion: vx / 16
///			vy -> s16 
///				Conversion: vy / 16
///			vz -> s16
///				Conversion: vz / 16
///		[ID]
///			id -> u32
///		[Texture Coordinates]
///			vu -> s16
///				Conversion: vu / 4096
///			vv -> s16
///				Conversion: vv / 4096
///		[Flags]
///			flags -> u32
/// 

#ifndef ZRNDR_GL_H
#define ZRNDR_GL_H

#define ZGL_SUCCESS 0
#define ZGL_FAIL -1

#include <vector>

#include "SDL3/SDL.h"
#include "SDL3/SDL_video.h"

#include "gamez/zSystem/zsys.h"
#include "gamez/zSystem/zsys_primitive.h"

typedef struct {
	SDL_GLContext ctx;
	SDL_Window* window;
	SDL_Renderer* renderer;
} zgl_context;

typedef struct {
	u8 qwc;
	u8 type;
	void* data;
} zgl_packet;

typedef struct {
	u8 qwc;
	bool setup;
	void* data;
	std::vector<zgl_packet> packets;
} zgl_chain;

typedef struct {
	u8 unk01[48];
	u32 vertex_count;
	u32 index_count;
	u8 unk02[24];
} zgl_mesh_packet_hdr;

typedef struct {
	f32 x;
	f32 y;
	f32 z;
	f32 f;
	f32 u;
	f32 v;
	f32 xn;
	f32 yn;
	f32 zn;
	// u32 flags;
} zgl_vertex;

typedef struct {
	u8 ix;
	u8 iy;
	u8 iz;
} zgl_index;

typedef struct {
	u32 vertex_count;
	u32 index_count;
	std::vector<zgl_vertex> vertices;
	std::vector<u32> indices;
} zgl_mesh;

typedef struct {
	zgl_mesh mesh;
	u32 v_array;
	u32 v_buffer;
	u32 e_buffer;
} zgl_mesh_buffer;

static zgl_context context;

static u32 zgl_vao;
static u32 zgl_vbo;
static u32 zgl_ebo;

extern s32 zgl_init();
extern void zgl_shutdown();

extern void zgl_set_context(SDL_Window* window, SDL_Renderer* renderer, SDL_GLContext context);

extern void zgl_enable_ztest();
extern void zgl_disable_ztest();

extern void zgl_set_fog(u32 r, u32 g, u32 b);

extern void zgl_create_buffers();
extern void zgl_destroy_buffers();

extern zgl_chain zgl_read_chain(const _word128* chain);
extern size_t zgl_get_chain_size(const zgl_chain* chain);

extern zgl_mesh zgl_chain_read_meshes(const zgl_chain* chain);
extern void zgl_chain_mesh_process(const zgl_packet* packet, zgl_mesh* mesh);

extern void zgl_mesh_buffer_create(zgl_mesh_buffer* buffer);
extern void zgl_mesh_buffer_destroy(zgl_mesh_buffer* buffer);
#endif