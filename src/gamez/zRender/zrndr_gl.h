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

#include "SDL3/SDL.h"
#include "SDL3/SDL_video.h"

#include "gamez/zSystem/zsys.h"
#include "gamez/zSystem/zsys_primitive.h"

typedef struct {
	SDL_GLContext& ctx;
	SDL_Window* window;
	SDL_Renderer* renderer;
} zgl_context;

typedef struct {
	u8 unk01[48];
	u32 vertex_count;
	u32 index_count;
	u8 unk02[24];
} zgl_mesh_packet_hdr;

typedef struct {
	s16 x;
	s16 y;
	s16 z;
	u16 index;
	s16 u;
	s16 v;
	u32 flags;
} zgl_vertex_packet;

typedef struct {
	u8 ix;
	u8 iy;
	u8 iz;
	u8 mark;
} zgl_index_packet;

typedef struct {
	zgl_mesh_packet_hdr header;
	zgl_vertex_packet* vertices;
	zgl_index_packet* indices;
} zgl_mesh_packet;

typedef struct {
	f32 x;
	f32 y;
	f32 z;
	u32 index;
	f32 u;
	f32 v;
	u32 flags;
} zgl_vertex;

typedef struct {
	u8 ix;
	u8 iy;
	u8 iz;
} zgl_index;

typedef struct {
	u32 vertex_count;
	u32 index_count;
	zgl_vertex* vertices;
	zgl_index* indices;
} zgl_mesh;

static zgl_context* context;

extern s32 zgl_init();
extern void zgl_shutdown();

extern void zgl_set_context(zgl_context* ctx);

extern void zgl_enable_ztest();
extern void zgl_disable_ztest();

extern void zgl_set_fog(u32 r, u32 g, u32 b);

extern zgl_mesh_packet zgl_read_packet(const _word128* chain);
extern zgl_mesh_packet zgl_read_mesh_packet(const void* packet);

extern zgl_mesh zgl_convert_mesh_packet(const zgl_mesh_packet* packet);
#endif