#include <GL/glew.h>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "main.h"

VertexArray::VertexArray(){
  GLCall(glGenVertexArrays(1, &id));
}

VertexArray::~VertexArray(){
  GLCall(glDeleteVertexArrays(1, &id));
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const{
  
  vb.bind();
  unsigned int stride = layout.offset * sizeof(GLfloat);
  for(unsigned int i = 0; i < layout.elements.size(); i++){
    const VertexBufferElement& e = layout.elements.at(i);
    GLCall(glEnableVertexAttribArray(i));
    GLCall(glVertexAttribPointer(
        i,
        e.count,
        e.type,
        GL_FALSE, 
        stride,
        (const void*)(e.offset)
        ));
  }

}


void VertexArray::bind(){
  GLCall(glBindVertexArray(id));
}

void VertexArray::unbind(){
  GLCall(glBindVertexArray(0));
}
