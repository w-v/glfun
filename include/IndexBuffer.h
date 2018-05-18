#ifndef CLASS_INDEXBUFFER
#define CLASS_INDEXBUFFER

class IndexBuffer {

  public:
    unsigned int id;
    unsigned int count;
    void * buffer;

    IndexBuffer(void * buffer, unsigned int count);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;
    void update() const;
};
#endif
