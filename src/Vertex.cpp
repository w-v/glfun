#include "Vertex.h"
#include <glm/glm.hpp>

Vertex::Vertex(glm::vec3 p)
  :pos(p)
{}
Vertex::~Vertex(){}
unsigned int Vertex::nbattrib = 1;


Vertexu::Vertexu(glm::vec3 p, glm::vec2 u)
  : Vertex(p), uv(u)
{}
Vertexu::~Vertexu(){}
unsigned int Vertexu::nbattrib = 2;

Vertexun::Vertexun(glm::vec3 p, glm::vec2 u, glm::vec3 n)
  : Vertex(p), uv(u), normal(n) 
{}
Vertexun::~Vertexun(){}
unsigned int Vertexun::nbattrib = 3;
