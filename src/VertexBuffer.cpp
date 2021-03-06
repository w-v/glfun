
#include <stdio.h>
#include <GL/glew.h>
#include "VertexBuffer.h"
#include "main.h"


VertexBuffer::VertexBuffer(void * b, unsigned int s)
  : buffer(b), size(s){
  GLCall(glGenBuffers(1, &id ));
  bind();
  GLCall(glBufferData(GL_ARRAY_BUFFER, s, b, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer(){
  GLCall(glDeleteBuffers(1, &id ));
}
void VertexBuffer::bind() const{
  GLCall(glBindBuffer(GL_ARRAY_BUFFER,id));
}

void VertexBuffer::update() const{
//  printf("%d\n",size);
  bind();
  GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, this->buffer));
}

void VertexBuffer::unbind() {
  GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
}
