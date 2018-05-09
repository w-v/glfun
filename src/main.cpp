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

#include <main.h>
#include <inputs.h>
#include <PerlinNoise.h>
#include <texture.h>

using namespace glm;

GLFWwindow* window; 
GLuint programID;
GLuint MatrixID;
glm::mat4 mvp;
GLuint w = 101;
GLuint nb_vertices = 3*((w-1)*((w)*2+2));
GLfloat scl = 0.1;
float t = 0.0f;
GLuint Texture;
GLuint TextureID; 


int main(){
  init();

  programID = LoadShaders( "src/shaders/vshader.txt", "src/shaders/fshader.txt" );
  load_models();
  main_loop();

}

void loadTexture(const char * imagepath){
  glGenTextures(1, &Texture);

  glBindTexture(GL_TEXTURE_2D, Texture);
  Texture = loadBMP_custom(imagepath);
  if(Texture == 0)
    printf("could not load texture");
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
  

  GLfloat** height_map = (GLfloat**) malloc(w*sizeof(GLfloat*));
  for(unsigned int i = 0;i<w;i++){
    height_map[i] = (GLfloat*) malloc(w*sizeof(GLfloat));
  }

  compute_height_map(height_map,t);
  // Get a handle for our "MVP" uniform
  // Only during the initialisation
  MatrixID = glGetUniformLocation(programID, "MVP");
  TextureID = glGetUniformLocation(programID, "myTextureSampler");

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
  int j;
  for(unsigned int i=0;i<w-1;){
    for(j=0;j<w;j++){
      put_vertex(g_vertex_buffer_data, vec3(j*scl,height_map[j][i],i*scl),&ind);
      put_vertex(g_vertex_buffer_data, vec3(j*scl,height_map[j][i+1],(i+1)*scl),&ind);
    }
    j--;
    put_vertex(g_vertex_buffer_data, vec3(j*scl,height_map[j][i+1],(i+1)*scl),&ind);
    put_vertex(g_vertex_buffer_data, vec3(j*scl,height_map[j][i+1],(i+1)*scl),&ind);
    i++;
    for(;0<=j;j--){
      put_vertex(g_vertex_buffer_data, vec3(j*scl,height_map[j][i],i*scl),&ind);
      put_vertex(g_vertex_buffer_data, vec3(j*scl,height_map[j][i+1],(i+1)*scl),&ind);
    }
    j++;
    put_vertex(g_vertex_buffer_data, vec3(j*scl,height_map[j][i+1],(i+1)*scl),&ind);
    put_vertex(g_vertex_buffer_data, vec3(j*scl,height_map[j][i+1],(i+1)*scl),&ind);
    i++;
  }
  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, nb_vertices*sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);

  loadTexture("texture/ff.bmp");

  GLfloat* uv = (GLfloat*) malloc((nb_vertices/3)*2*sizeof(GLfloat));
  ind = 0;
  vec2 uv_coords[3] = {
    vec2(0.0f,0.0f),
    vec2(0.1f,0.0f),
    vec2(0.0f,0.1f)
  };
  for(unsigned int i = 0;i < nb_vertices/3;i++){
    put_vertex2(uv,uv_coords[i%3],&ind);
  }/*
  float uv[100] = {
    0.0f, 0.0f,
    0.1f, 0.0f,
    0.0f, 0.1f,
    0.0f, 0.0f,
    0.1f, 0.0f,
    0.0f, 0.1f,
    0.0f, 0.0f,
    0.1f, 0.0f,
    0.0f, 0.1f,
    0.0f, 0.0f,
    0.1f, 0.0f,
    0.0f, 0.1f,
    0.0f, 0.0f,
    0.1f, 0.0f,
    0.0f, 0.1f,
    0.0f, 0.0f,
    0.1f, 0.0f,
    0.0f, 0.1f,
    0.0f, 0.0f,
    0.1f, 0.0f,
    0.0f, 0.1f,
    0.0f, 0.0f,
    0.1f, 0.0f,
    0.0f, 0.1f
  };*/
  
  GLuint uv_buffer;
  glGenBuffers(1,&uv_buffer);
  glBindBuffer(GL_ARRAY_BUFFER,uv_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(nb_vertices/3)*2, uv, GL_STATIC_DRAW);

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
  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
  glVertexAttribPointer(
      1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
      2,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
      );

  for(unsigned int i = 0;i<w;i++){
    free(height_map[i] );
  }
  free(height_map);
  free(g_vertex_buffer_data);
  //free(uv);

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
  for(unsigned int i=0;i<w;i++){
    for(unsigned int j=0;j<w;j++){
      //height_map[i][j] = sin(i*scl)*sin(j*scl);
      height_map[i][j] = p.noise(i*scl,j*scl,0);
    }
  }
}

int main_loop(){
  do{
    // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    t += 0.1f;
    compute_mvp(mvp);
    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glUniform1i(TextureID, 0);
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLE_STRIP, 0, nb_vertices); // Starting from vertex 0; 3 vertices total -> 1 triangle

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
