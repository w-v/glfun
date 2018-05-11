#ifndef CLASS_VERTEXBUFFERLAYOUT
#define CLASS_VERTEXBUFFERLAYOUT
#include <vector>

struct VertexBufferElement{
  unsigned int count;
  unsigned int type;
  bool normalized;
  unsigned int offset;
};

class VertexBufferLayout{

  public:
    unsigned int offset;
    std::vector<VertexBufferElement> elements;
    VertexBufferLayout();
    ~VertexBufferLayout();
    void push(short unsigned int count, bool normalized);
};
#endif
