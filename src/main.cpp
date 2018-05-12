#include <stdio.h>
#include <iostream>
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

#include "main.h"
#include "inputs.h"
#include "PerlinNoise.h"
#include "texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Vertex.h"

using namespace glm;

GLFWwindow* window; 
GLuint programID;
GLuint MatrixID;
glm::mat4 mvp;
GLuint w = 301;
GLuint nb_vertices = w*w;
GLuint nb_indices = nb_vertices*2-w; // normally (nb-w)*2 but +w for restart indices
GLfloat scl = 0.1;
float t = 0.0f;
GLuint Texture;
GLuint TextureID; 
bool fill = true;

int main(){
  init();

  programID = LoadShaders( "src/shaders/vshader.txt", "src/shaders/fshader.txt" );
  GLCall(glUseProgram(programID));
  load_models();
  main_loop();

}

void GLClearError(){
  while(glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line){

  while(GLenum error = glGetError()){
    std::cout << "[OpenGL Error] (" << error << ") " << function << " " << file <<  ":" << line << std::endl;
    return false;
  }
  return true;

}

void loadTexture(const char * imagepath){
  GLCall(glGenTextures(1, &Texture));

  GLCall(glBindTexture(GL_TEXTURE_2D, Texture));
  Texture = loadBMP_custom(imagepath);
  if(Texture == 0)
    printf("could not load texture");
}

int init(){
  glewExperimental = true; // Needed for core profile
  if( !glfwInit())
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
    GLCall(glfwTerminate());
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
  
  loadTexture("texture/ff.bmp");
  GLfloat** height_map = (GLfloat**) malloc(w*sizeof(GLfloat*));
  for(unsigned int i = 0;i<w;i++){
    height_map[i] = (GLfloat*) malloc(w*sizeof(GLfloat));
  }

  compute_height_map(height_map,t);
  // Get a handle for our "MVP" uniform
  // Only during the initialisation
  GLCall(MatrixID = glGetUniformLocation(programID, "MVP"));
  GLCall(TextureID = glGetUniformLocation(programID, "myTextureSampler"));

  // Enable depth test
  GLCall(glEnable(GL_DEPTH_TEST));
  // Accept fragment if it closer to the camera than the former one
  GLCall(glDepthFunc(GL_LESS));

  // Enable restarting triangle strip
  GLCall(glEnable(GL_PRIMITIVE_RESTART));


  Vertexu* terrain = (Vertexu*) malloc(nb_vertices*sizeof(Vertexu));
  //Vertexun* terrain = (Vertexun*) malloc(nb_vertices*sizeof(Vertexun));
  //GLfloat* uv = (GLfloat*) malloc((nb_vertices/3)*2*sizeof(GLfloat));
  unsigned int* indices = (unsigned int*) malloc( nb_indices*sizeof(GLuint) );
  
  // making the grid
  GLfloat step = 1.0f/ (float) w*w;
  //GLfloat step = 0.1f;
  GLuint ind = 0;
  int j;
  unsigned int tex_span = 6; // nb of squares the texture is mapped to
  float a,b,c;
  for(unsigned int i=0;i<w;i++){
    for(j=0;j<w;j++){
      a = (float)(j%tex_span)/(float)tex_span;
      b = (float)(i%tex_span)/(float)tex_span;
      /*terrain[ind++] = Vertexun(
          vec3(j*scl,height_map[j][i],i*scl),
          vec2(a,b),
          vec3(0.0f,0.0f,0.0f)
          );*/
      terrain[ind++] = Vertexu(
          vec3(j*scl,height_map[j][i],i*scl),
          vec2(a,b)
          );
    }
  }
  ind = 0;
  for(unsigned int i=0;i<w-1;i++){
    for(j=0;j<w;j++){
      indices[ind++] = j+w*i;
      indices[ind++] = j+w*(i+1);
    }
    indices[ind++] = nb_vertices;
  }

  //for(unsigned int i=0;i<nb_indices;i++){
    //vec3 v1 = indices[]
  //}
  GLCall(glPrimitiveRestartIndex(nb_vertices));

  VertexArray* va = new VertexArray();
  va->bind();
  VertexBuffer * vb = new VertexBuffer(terrain, nb_vertices*(3+2));
  VertexBufferLayout* layout = new VertexBufferLayout;
  layout->push(3,false);
  layout->push(2,false);
  //layout->push(3,false);
  va->addBuffer(*vb, *layout);
  IndexBuffer* ib = new IndexBuffer(indices, nb_indices);
  ib->bind();
  for(unsigned int i = 0;i<w;i++){
    free(height_map[i] );
  }
  free(height_map);
  free(terrain);
}

void put_vertex(GLfloat* buffer, const glm::vec3& vertex, GLuint* index){

  buffer[(*index)++] = vertex.x;
  buffer[(*index)++] = vertex.y;
  buffer[(*index)++] = vertex.z;
}

void put_vertex2(GLfloat* buffer, const glm::vec2& vertex, GLuint* index){

  buffer[(*index)++] = vertex.x;
  buffer[(*index)++] = vertex.y;
}

void compute_height_map(GLfloat** height_map,float t){
  PerlinNoise p(69); 
  float ter_scl1 = 0.5;
  float ter_scl2 = 0.1;
  for(unsigned int i=0;i<w;i++){
    for(unsigned int j=0;j<w;j++){
      //height_map[i][j] = sin(i*scl)*sin(j*scl);
      height_map[i][j] = p.noise(i*scl*ter_scl1,j*scl*ter_scl1,0)+p.noise(i*scl*ter_scl2,j*scl*ter_scl2,69)*10;
    }
  }
}

int main_loop(){
  do{
    // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
    GLCall(glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    t += 0.1f;
    compute_mvp(mvp);
    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
    GLCall(glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]));

    if (glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS){
      fill=!fill;
      if(fill){
        GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
      }
      else{
        GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
      }
    }
    
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, Texture));
    GLCall(glUniform1i(TextureID, 0));
    // Draw the triangle !
    GLCall(glDrawElements(GL_TRIANGLE_STRIP, nb_indices, GL_UNSIGNED_INT, (GLvoid*) 0)); // Starting from vertex 0; 3 vertices total -> 1 triangle

    //glDrawArrays(GL_TRIANGLE_STRIP, 0, nb_vertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
      glfwWindowShouldClose(window) == 0 );

  GLCall(glDisableVertexAttribArray(0));  
  return 0;
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

  // Create the shaders
  GLCall(GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER));
  GLCall(GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER));

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
  GLCall(glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL));
  GLCall(glCompileShader(VertexShaderID));

  // Check Vertex Shader
  GLCall(glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result));
  GLCall(glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength));
  if ( InfoLogLength > 0 ){
    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
    GLCall(glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]));
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }



  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragment_file_path);
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  GLCall(glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL));
  GLCall(glCompileShader(FragmentShaderID));

  // Check Fragment Shader
  GLCall(glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result));
  GLCall(glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength));
  if ( InfoLogLength > 0 ){
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
    GLCall(glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]));
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }



  // Link the program
  printf("Linking program\n");
  GLCall(GLuint ProgramID = glCreateProgram());
  GLCall(glAttachShader(ProgramID, VertexShaderID));
  GLCall(glAttachShader(ProgramID, FragmentShaderID));
  GLCall(glLinkProgram(ProgramID));

  // Check the program
  GLCall(glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result));
  GLCall(glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength));
  if ( InfoLogLength > 0 ){
    std::vector<char> ProgramErrorMessage(InfoLogLength+1);
    GLCall(glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]));
    printf("%s\n", &ProgramErrorMessage[0]);
  }

  
  GLCall(glDetachShader(ProgramID, VertexShaderID));
  GLCall(glDetachShader(ProgramID, FragmentShaderID));
  
  GLCall(glDeleteShader(VertexShaderID));
  GLCall(glDeleteShader(FragmentShaderID));

  return ProgramID;
}
