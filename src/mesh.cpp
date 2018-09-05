#include "mesh.hpp"
#include <GL/glew.h>

namespace SMOBA
{
    Mesh Gen_Mesh(Array<Vertex>& verts, Array<u32>& indices)
    {
        Mesh result = {};

        glGenBuffers(1, &result.VBO);
        glGenBuffers(1, &result.EBO);

        // vertex VBO
        glBindBuffer(GL_ARRAY_BUFFER, result.VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     verts.Size * sizeof(Vertex),
                     &verts[0],
                     GL_STATIC_DRAW);

        glGenVertexArrays(1, &result.VAO);
        glBindVertexArray(result.VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.Size * sizeof(u32),
                     &indices[0],
                     GL_STATIC_DRAW);

        // vertex positions
        glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              11 * sizeof(r32),
                              (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // texture coordinates
        glVertexAttribPointer(1,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              11 * sizeof(r32),
                              (GLvoid*)offsetof(Vertex, u));
        glEnableVertexAttribArray(1);
        //normal vectors
        glVertexAttribPointer(2,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              11 * sizeof(r32),
                              (GLvoid*)offsetof(Vertex, nx));
        glEnableVertexAttribArray(2);
        //tangent vectors
        glVertexAttribPointer(3,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              11 * sizeof(r32),
                              (GLvoid*)offsetof(Vertex, tx));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);

        result.Vertices = verts.Size;
        result.Indices = indices.Size;

        verts.Free_Data();
        indices.Free_Data();

        return result;

    }
}
