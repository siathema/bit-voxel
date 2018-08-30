#pragma once

#include "game.hpp"

#define IQM_MAGIC "INTERQUAKEMODEL"
#define IQM_VERSION 2

namespace SMOBA {

	struct iqmheader
	{
		char magic[16];
		u32 version;
		u32 filesize;
		u32 flags;
		u32 num_text, ofs_text;
		u32 num_meshes, ofs_meshes;
		u32 num_vertexarrays, num_vertexes, ofs_vertexarrays;
		u32 num_triangles, ofs_triangles, ofs_adjacency;
		u32 num_joints, ofs_joints;
		u32 num_poses, ofs_poses;
		u32 num_anims, ofs_anims;
		u32 num_frames, num_framechannels, ofs_frames, ofs_bounds;
		u32 num_comment, ofs_comment;
		u32 num_extensions, ofs_extensions;
	};

	struct iqmmesh
	{
		u32 name;
		u32 material;
		u32 first_vertex, num_vertexes;
		u32 first_triangle, num_triangles;
	};

	enum
	{
		IQM_POSITION = 0,
		IQM_TEXCOORD = 1,
		IQM_NORMAL = 2,
		IQM_TANGENT = 3,
		IQM_BLENDINDEXES = 4,
		IQM_BLENDWEIGHTS = 5,
		IQM_COLOR = 6,
		IQM_CUSTOM = 0x10
	};

	enum
	{
		IQM_BYTE = 0,
		IQM_UBYTE = 1,
		IQM_SHORT = 2,
		IQM_USHORT = 3,
		IQM_INT = 4,
		IQM_UINT = 5,
		IQM_HALF = 6,
		IQM_FLOAT = 7,
		IQM_DOUBLE = 8,
	};

	struct iqmtriangle
	{
		u32 vertex[3];
	};

	struct iqmadjacency
	{
		u32 triangle[3];
	};

	struct iqmjoint
	{
		u32 name;
		i32 parent;
		r32 translate[3], rotate[4], scale[3];
	};

	struct iqmpose
	{
		i32 parent;
		u32 mask;
		r32 channeloffset[10];
		r32 channelscale[10];
	};

	struct iqmanim
	{
		u32 name;
		u32 first_frame, num_frames;
		r32 framerate;
		u32 flags;
	};

	enum
	{
		IQM_LOOP = 1 << 0
	};

	struct iqmvertexarray
	{
		u32 type;
		u32 flags;
		u32 format;
		u32 size;
		u32 offset;
	};

	struct iqmbounds
	{
		r32 bbmin[3], bbmax[3];
		r32 xyradius, radius;
	};

}

