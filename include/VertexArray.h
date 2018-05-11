#ifndef CLASS_VERTEXARRAY
#define CLASS_VERTEXARRAY

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray {

  public:

    unsigned int id;

    VertexArray();
    ~VertexArray();

    void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const;

    void bind();
    void unbind();
};
#endif
