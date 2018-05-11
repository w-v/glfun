#include <GL/glew.h>
#include "VertexBufferLayout.h"
#include "main.h"

VertexBufferLayout::VertexBufferLayout()
  : offset(0)
{}

VertexBufferLayout::~VertexBufferLayout(){
}

void VertexBufferLayout::push(short unsigned int count, bool normalized){
  unsigned int a = offset*sizeof(GLfloat);
  elements.push_back((VertexBufferElement){count,GL_FLOAT,normalized,a});
  offset += count;
}


