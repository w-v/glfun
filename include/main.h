#include <glm/glm.hpp>
extern GLFWwindow* window; 
int init();
int load_models();
int main_loop();
void put_vertex(GLfloat* buffer, const glm::vec3& vertex, GLuint* index);
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
void compute_height_map(GLfloat** height_map, float t);
