#include "geometry.h"

Geometry::Geometry(float* vertices, size_t size){
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    bind();
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, size, &m_VBO, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    glEnableVertexAttribArray(0);

    unbind();
}


void Geometry::bind() const {
    glBindVertexArray(m_VAO);
}

void Geometry::unbind()const {
    glBindVertexArray(0);
}

void Geometry::draw() const{
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
}

Geometry::~Geometry() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}