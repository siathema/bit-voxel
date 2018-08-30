#pragma once

#include "game.hpp"
#include "sMath.hpp"
#include "array.hpp"

namespace SMOBA
{

    struct Vertex
    {
      r32 x,y,z,u,v,nx,ny,nz,tx,ty,tz;
    };

    struct Material
    {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        r32 shininess;
    };

    struct Mesh
    {
        u32 VAO;
        u32 VBO;
        u32 EBO;
        Material material;
        ID diffuse;
        ID specular;
        ID normal;
        Array<Vertex> vertices;
        Array<u32> indices;

        Mesh();
        ~Mesh();
        void GenVertexObjects();
    };

    struct Model
    {
        u32 NumMeshes;
        Array<ID> Meshes;
        Model():Meshes() {}
    };

}
