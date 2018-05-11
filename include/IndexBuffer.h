#ifndef CLASS_INDEXBUFFER
#define CLASS_INDEXBUFFER

class IndexBuffer {

  public:
    unsigned int id;
    unsigned int count;
    void * buffer;

    IndexBuffer(void * buffer, unsigned int count);
    ~IndexBuffer();

    void bind();
    void unbind();
};
#endif
