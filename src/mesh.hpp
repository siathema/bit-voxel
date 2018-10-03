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

    struct Mesh
    {
        u32 VAO;
        u32 VBO;
        u32 EBO;
        u32 Vertices;
        u32 Indices;
     };

    Mesh Gen_Mesh(Array<Vertex>& verts, Array<u32>& indices);
    void Destroy_Mesh(Mesh* mesh);
}
