#include <GL/glew.h>
#include "IndexBuffer.h"


IndexBuffer::IndexBuffer(void * buffer, unsigned int count){
  glGenBuffers(1, &(this->id) );
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), buffer, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer(){
  glDeleteBuffers(1, &(this->id) );
}

void IndexBuffer::bind(){
  glBindBuffer(GL_ARRAY_BUFFER,this->id);
}

void IndexBuffer::unbind(){
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
