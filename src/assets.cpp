#include "assets.hpp"
//#include "../assets/meshes.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "renderer.hpp"
#include "IQM.hpp"
#include "voxel.hpp"
#include "array.hpp"

#include <string.h>

namespace SMOBA
{
	Array<Texture2D> ASSETS::Textures;
	Array<Mesh> ASSETS::Meshes;
	Array<Model> ASSETS::Models;
	//static Array<ID> ASSETS::Meshes::ChunkMeshes;

	void Load_Texture(const char *path)
	{
		i32 w, h, channels;
		u8* pixels = stbi_load(path, &w, &h, &channels, 4);
		if (pixels == 0) printf("%s\n", stbi_failure_reason());
		Texture2D tex(pixels, w, h);
		ASSETS::Textures.Add(tex);
	}

	void loadFromIQM(const char *path)
	{
		FILE* inStream = fopen(path, "rb");
		s_assert(inStream != 0, "Could not open file\n");

		iqmheader header = {};
		iqmmesh* iqm_mesh = 0;
		iqmvertexarray* iqm_vertex_array = 0;
		Array<iqmtriangle> triangles;

		fread(&header, sizeof(iqmheader), 1, inStream);
		u8* buffer = (u8*)malloc(header.filesize * sizeof(u8));
		fseek(inStream, 0, SEEK_SET);
		fread(buffer, header.filesize*sizeof(u8), 1, inStream);
		fclose(inStream);

		iqm_mesh = (iqmmesh*)malloc(sizeof(iqmmesh)*header.num_meshes);
		iqm_vertex_array = (iqmvertexarray*)malloc(header.num_vertexarrays * sizeof(iqmvertexarray));
		Array<r32> vert_pos, vert_tex, vert_norm, vert_tan;


		u8* mesh_location = buffer + header.ofs_meshes;
		for(i32 imesh=0; imesh<header.num_meshes; imesh++)
		{
			mesh_location += imesh * sizeof(iqmmesh);
			memcpy(&iqm_mesh[imesh], mesh_location, sizeof(iqmmesh));
		}

		mesh_location = buffer + header.ofs_vertexarrays;
		iqmvertexarray* varray_location = (iqmvertexarray*)mesh_location;
		for (i32 imesh = 0; imesh<header.num_vertexarrays; imesh++)
		{
			iqm_vertex_array[imesh] = varray_location[imesh];
		}

		//NOTE(matthias): pulling out vertex data;
		for (i32 varray = 0; varray < header.num_vertexarrays; varray++)
		{
			iqmvertexarray current_array = iqm_vertex_array[varray];
			if (current_array.type != IQM_POSITION &&
				current_array.type != IQM_TEXCOORD &&
				current_array.type != IQM_NORMAL &&
				current_array.type != IQM_TANGENT)
				continue;

			u8* vertexArrayAddress = buffer + current_array.offset;
			r32* vertices = (r32*)vertexArrayAddress;

			for (i32 vert = 0; vert < header.num_vertexes; vert++)
			{
				for (i32 attribute = 0; attribute < current_array.size; attribute++)
				{
					i32 totalV = vert * current_array.size + attribute;
					switch (current_array.type) {
					case IQM_POSITION:
						vert_pos.Add(vertices[totalV]);
						break;
					case IQM_TEXCOORD:
						vert_tex.Add(vertices[totalV]);
						break;
					case IQM_NORMAL:
						vert_norm.Add(vertices[totalV]);
						break;
					case IQM_TANGENT:
						vert_tan.Add(vertices[totalV]);
						break;
					}
				}
			}
		}

		// NOTE(matthias): Getting triangles
		u8* triAddress = buffer + header.ofs_triangles;
		iqmtriangle* tris = (iqmtriangle*)triAddress;
		for (i32 tri = 0; tri < header.num_triangles; tri++)
		{
			triangles.Add(tris[tri]);
		}

		// NOTE(matthias): building meshes
		Model model;

		for (i32 m = 0; m < header.num_meshes; m++)
		{
			Mesh mesh;
			u32 firstTriangle = iqm_mesh[m].first_triangle;
			u32 firstVertex = iqm_mesh[m].first_vertex;

			//mesh.vertices.resize(iqm_mesh[m].num_vertexes);
			//mesh.indices.resize(iqm_mesh[m].num_triangles * 3);
			for (i32 tri = firstTriangle; tri < iqm_mesh[m].num_triangles+firstTriangle; tri++)
			{
				for(i32 i=0; i<3; i++)
					mesh.indices.Add(triangles[tri].vertex[i]);
			}
			for (i32 vert = firstVertex; vert < iqm_mesh[m].num_vertexes+firstVertex; vert++)
			{
				Vertex v = {};
				i32 posIndex = vert*3;
				i32 texIndex = vert*2;

				v.x = vert_pos[posIndex];
				v.y = vert_pos[posIndex+1];
				v.z = vert_pos[posIndex+2];
				v.u = vert_tex[texIndex];
				v.v = vert_tex[texIndex+1];
				v.nx = vert_norm[posIndex];
				v.ny = vert_norm[posIndex+1];
				v.nz = vert_norm[posIndex+2];
				v.tx = vert_tan[posIndex];
				v.ty = vert_tan[posIndex+1];
				v.tz = vert_tan[posIndex+2];

				mesh.vertices.Add(v);
			}

			//	__builtin_debugtrap();
#if 0
			for(i32 i=0; i<mesh.vertices.size; i++)
			{
				printf("x: %f y:%f z:%f u:%f v%f nx:%f ny:%f nz:%f\n",
					   mesh.vertices[i].x,
					   mesh.vertices[i].y,
					   mesh.vertices[i].z,
					   mesh.vertices[i].u,
					   mesh.vertices[i].v,
					   mesh.vertices[i].nx,
					   mesh.vertices[i].ny,
					   mesh.vertices[i].nz);
			}
			for(i32 i=0; i<mesh.indices.size; i++)
			{
				printf("%d\n" ,mesh.indices[i]);
			}
#endif
			Material mat = {};
			mat.ambient = vec3(1.0f, 0.5f, 0.31f);
			mat.diffuse = vec3(1.0f, 0.5f, 0.31f);
			mat.specular = vec3(0.5f, 0.5f, 0.5f);
			mat.shininess = 32.0f;

			mesh.material = mat;
			mesh.GenVertexObjects();
			ASSETS::Meshes.Add(mesh);
		}

		ASSETS::Models.Add(model);
		u8* text = buffer + header.ofs_text;


		free(buffer);
		free(iqm_mesh);
		free(iqm_vertex_array);
	}

	void GenDebugCube()
	{
		Mesh* cube = new Mesh();
		r32 vertices[] = {
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f, 1.0f, 1.0f,  1.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f, 1.0f, 1.0f,  1.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

			0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f
		};

		for(u32 i=0; i<36; ++i)
		{
			Vertex v = {};
			v.x = vertices[i*8];
			v.y = vertices[i*8+1];
			v.z = vertices[i*8+2];
			v.u = vertices[i*8+3];
			v.v = vertices[i*8+4];
			v.nx = vertices[i*8+5];
			v.ny = vertices[i*8+6];
			v.nz = vertices[i*8+7];
			cube->vertices.Add(v);
			cube->indices.Add(i);
		}
		Material mat = {};
		mat.ambient = vec3(1.0f, 0.5f, 0.31f);
		mat.diffuse = vec3(1.0f, 0.5f, 0.31f);
		mat.specular = vec3(0.5f, 0.5f, 0.5f);
		mat.shininess = 32.0f;
		cube->material = mat;
		cube->GenVertexObjects();
		ASSETS::Meshes.Add(*cube);
	}

	Model* ASSETS::Get_Model(ID assetID)
	{
		return &(Models[assetID]);
	}

	Mesh* ASSETS::Get_Mesh(ID assetID)
	{
		return &(Meshes[assetID]);
	}

	Texture2D* ASSETS::Get_Texture(ID assetID)
	{
		return &(Textures[assetID]);
	}

	//TODO(matthias): OMFG change this!
	void ASSETS::Load_Assets(ViewportInfo& viewport, Voxel_World* voxelWorld)
    {
		{
			Load_Texture("assets/img/test.png");
			Load_Texture("assets/fonts/DebugFont.png");
			Load_Texture("assets/img/tile.png");
			Load_Texture("assets/img/tex.png");
			Load_Texture("assets/img/Turret-Diffuse.jpg");
			Load_Texture("assets/img/SPECULAR_Character.jpg");
			Load_Texture("assets/img/Turret-Normal.jpg");
		}

        GenDebugCube();
        loadFromIQM("assets/models/TestBot.iqm");

        for (i32 chunk = 0; chunk < voxelWorld->GeneratedChunks.Size; chunk++)
        {
            Voxel_Chunk* CurrentChunk = &voxelWorld->Chunks[voxelWorld->GeneratedChunks[chunk]];
            ASSETS::Meshes.Add(*Generate_Voxel_Chunk_Mesh(voxelWorld, CurrentChunk->ChunkID));
            //ASSETS::Meshes::ChunkMeshes.add(ASSETS::meshes.size - 1);
            CurrentChunk->MeshID = ASSETS::Meshes.Size - 1;
        }


		{
			ASSETS::Models[0].Meshes.Add(2);
			ASSETS::Models[0].NumMeshes = ASSETS::Models[0].Meshes.Size;
			ASSETS::Meshes[2].diffuse = ASSETS::TEXTURES::TurretDiffuse;
			ASSETS::Meshes[2].specular = ASSETS::TEXTURES::TurretSpecular;
			ASSETS::Meshes[2].normal = ASSETS::TEXTURES::TurretNormal;
		}
	}

	void ASSETS::Unload_Assets() {

	}
}
