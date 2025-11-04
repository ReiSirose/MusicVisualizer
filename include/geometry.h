#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <glad/glad.h>
#include <cstddef> 


class Geometry {
public:
    Geometry(float* vertices, size_t size);
    ~Geometry();

    void bind() const;
    void unbind() const;
    void draw() const;

private:
    GLuint m_VAO;
    GLuint m_VBO;
    GLsizei m_vertexCount;
};

#endif