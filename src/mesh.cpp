#include "mesh.hpp"
#include <GL/glew.h>

namespace SMOBA
{
  Mesh::Mesh() : diffuse(), specular(), normal(), vertices(), indices() {

  }

  Mesh::~Mesh() {}

  void Mesh::GenVertexObjects() {
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // vertex VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.Size * sizeof(Vertex),
                 &vertices[0],
                 GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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

  }
}
