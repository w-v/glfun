#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
extern GLFWwindow* window; 
int init();
int load_models();
void init_load_models();
void loadTexture(const char * imagepath);
int main_loop();
void put_vertex(GLfloat* buffer, const glm::vec3& vertex, GLuint* index);
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
void compute_height_map(GLfloat** height_map, float t);
void put_vertex2(GLfloat* buffer, const glm::vec2& vertex, GLuint* index);
bool GLLogCall(const char* function, const char* file, int line);
void GLClearError();

#define ASSERT(x) if (!(x)) __builtin_trap()
#define GLCall(x) GLClearError();\
  x;\
  ASSERT(GLLogCall(#x, __FILE__, __LINE__))
