#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>
using namespace glm;

GLFWwindow* window; 
GLuint programID;
GLuint MatrixID;
glm::mat4 mvp;
GLuint w = 100;
GLuint nb_vertices = (w-1)*(w-1)*2*3*3;
GLfloat scl = 0.1;

int compute_mvp();
int init();
int load_models();
int main_loop();
void put_vertex(GLfloat* buffer, const glm::vec3& vertex, GLuint* index);
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

int main(){
  init();

  programID = LoadShaders( "vshader.txt", "fshader.txt" );
  load_models();
  main_loop();

}

int init(){
  glewExperimental = true; // Needed for core profile
  if( !glfwInit() )
  {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    return -1;
  }
  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

  // Open a window and create its OpenGL context
  window = glfwCreateWindow( 1024, 768, "Tutorial 01", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window); // Initialize GLEW
  glewExperimental=true; // Needed in core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }
  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

}

int load_models(){
  // Get a handle for our "MVP" uniform
  // Only during the initialisation
  MatrixID = glGetUniformLocation(programID, "MVP");
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  // Use our shader
  glUseProgram(programID);

  // Draw nothing, see you in tutorial 2 !
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  GLfloat* g_vertex_buffer_data = (GLfloat*) malloc(nb_vertices*sizeof(GLfloat));
  
  // making the grid
  GLfloat step = 1.0f/ (float) w*w;
  //GLfloat step = 0.1f;
  GLuint ind = 0;
  glm::vec2 c = vec2(0.0f,0.0f);
  for(unsigned int i=0;i<w-1;i++){
    for(unsigned int j=0;j<w-1;j++){
      //making a square

      // first triangle
      put_vertex(g_vertex_buffer_data, vec3(c.x,c.y,glm::sin(c.x*M_PI*2)),&ind);
      put_vertex(g_vertex_buffer_data, vec3(c.x+step,c.y,glm::sin((c.x+step)*M_PI*2)),&ind);
      put_vertex(g_vertex_buffer_data, vec3(c.x,c.y+step,glm::sin(c.x*M_PI*2)),&ind);

      // second triangle
      put_vertex(g_vertex_buffer_data, vec3(c.x,c.y+step,glm::sin(c.x*M_PI*2)),&ind);
      put_vertex(g_vertex_buffer_data, vec3(c.x+step,c.y,glm::sin((c.x+step)*M_PI*2)),&ind);
      put_vertex(g_vertex_buffer_data, vec3(c.x+step,c.y+step,glm::sin((c.x+step)*M_PI*2)),&ind);
      c.x += step;
    }
    c.x = 0.0f;
    c.y += step;
  }
  GLuint vertexbuffer;
  // Generate 1 buffer, put the resulting identifier in vertexbuffer
  glGenBuffers(1, &vertexbuffer);
  // The following commands will talk about our 'vertexbuffer' buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  // Give our vertices to OpenGL.
  glBufferData(GL_ARRAY_BUFFER, nb_vertices*sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);

  GLfloat* vertex_color = (GLfloat*) malloc(nb_vertices*sizeof(GLfloat));

  for(unsigned int i=0;i<(w-1)*(w-1)*2*3;i++){
    vertex_color[i*3] = 1.0f;
    vertex_color[i*3+1] = 1.0f;
    vertex_color[i*3+2] = 1.0f;
  }
  
  GLuint color_buffer;
  glGenBuffers(1,&color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER,color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_color), vertex_color, GL_STATIC_DRAW);

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(
      0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
      );

  // 2nd attribute buffer : vertices
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  glVertexAttribPointer(
      1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
      );

}

void put_vertex(GLfloat* buffer, const glm::vec3& vertex, GLuint* index){

  buffer[(*index)++] = vertex.x;
  buffer[(*index)++] = vertex.z;
  buffer[(*index)++] = vertex.y;
}

void compute_height_map(GLfloat** height_map){
  for(unsigned int i=0;i<w;i++){
    for(unsigned int j=0;j<w;j++){
      height_map[i][j] = sin(i*scl)*sin(j*scl);
    }
  }
}
int compute_mvp() {
  int width = 4;
  int height = 3;
  GLfloat rotation = 0;
  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);

  // Or, for an ortho camera :
  //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

  // Camera matrix
  glm::mat4 View = glm::lookAt(
      glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
      glm::vec3(0,0,0), // and looks at the origin
      glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
      );

    
    glm::mat4 sc = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 rot = glm::rotate(rotation, glm::vec3(1.0f,0.0f,0.0f));
    glm::mat4 tr = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));

    // Model matrix : an identity matrix (model will be at the origin)
    //glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 Model = tr * rot * sc;
    // Our ModelViewProjection : multiplication of our 3 matrices
    mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around
}

int main_loop(){
  do{
    // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    compute_mvp();
    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices); // Starting from vertex 0; 3 vertices total -> 1 triangle

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
      glfwWindowShouldClose(window) == 0 );

  glDisableVertexAttribArray(0);  
  return 0;
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
  if(VertexShaderStream.is_open()){
    std::stringstream sstr;
    sstr << VertexShaderStream.rdbuf();
    VertexShaderCode = sstr.str();
    VertexShaderStream.close();
  }else{
    printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
    getchar();
    return 0;
  }

  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
  if(FragmentShaderStream.is_open()){
    std::stringstream sstr;
    sstr << FragmentShaderStream.rdbuf();
    FragmentShaderCode = sstr.str();
    FragmentShaderStream.close();
  }

  GLint Result = GL_FALSE;
  int InfoLogLength;


  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertex_file_path);
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }



  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragment_file_path);
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }



  // Link the program
  printf("Linking program\n");
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> ProgramErrorMessage(InfoLogLength+1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }

  
  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);
  
  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}
