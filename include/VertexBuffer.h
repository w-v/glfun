#ifndef CLASS_VERTEXBUFFER
#define CLASS_VERTEXBUFFER

class VertexBuffer {

  public:
    unsigned int id;
    unsigned int size;
    void * buffer;

    VertexBuffer(void * buffer, unsigned int size);
    ~VertexBuffer();

    void bind() const;
    void unbind();
};
#endif
