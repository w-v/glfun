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
#include <iostream>

#include "main.h"
#include "inputs.h"
#include "FastNoise.h"
#include "PerlinNoise.h"
#include "texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Vertex.h"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"

using namespace glm;

GLFWwindow* window; 
GLuint programID;
GLuint MatrixID;
GLuint MID;
GLuint VID;
glm::mat4 mvp;
glm::mat4 v;
glm::mat4 m;
GLuint w = 2000;
GLuint nb_vertices = w*w;
GLuint nb_indices = nb_vertices*2-w; // normally (nb-w)*2 but +w for restart indices
GLfloat scl = 0.1f;
float t = 0.0f;
GLuint Texture;
GLuint TextureID; 
bool fill = true;

//global variables for debuging window
int nbOctave = 5;
int tempOctave = nbOctave;
float zoom = 5.0f;
float tempzoom = zoom;
int neg = 1;
int tempneg = neg;
int seed = 15;
int tempseed = seed;
float lacunarity = 2.0f;
float templacunarity = lacunarity;
float persistence = 0.5f; 
float temppersistence = persistence;
float flatcoef = 5.0f;
float tempflatcoef = flatcoef;
float modfreq = 0.01;
float tempmodfreq = modfreq;
int choosePerlin = 0;
int tempchoose = choosePerlin;

Vertexun* terrain ;
//GLfloat* uv = (GLfloat*) malloc((nb_vertices/3)*2*sizeof(GLfloat));
unsigned int* indices ;
GLfloat**  height_map;
VertexArray* va;
VertexBuffer * vb;
VertexBufferLayout* layout;
IndexBuffer* ib;

int main(){
  init();

  programID = LoadShaders( "src/shaders/vshader.txt", "src/shaders/fshader.txt" );
  GLCall(glUseProgram(programID));

  init_load_models();
  load_models();
  main_loop();
  for(unsigned int i = 0;i<w;i++){
    free(height_map[i] );
  }
  free(height_map);
  free(terrain);
  free(indices);

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
  window = glfwCreateWindow( 1920, 1080, "Tutorial 01", NULL, NULL);
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

void init_load_models(){
  loadTexture("texture/ff.bmp");
  height_map = (GLfloat**) malloc(w*sizeof(GLfloat*));
  for(unsigned int i = 0;i<w;i++){
    height_map[i] = (GLfloat*) malloc(w*sizeof(GLfloat));
  }

  // Get a handle for our "MVP" uniform
  // Only during the initialisation
  GLCall(MatrixID = glGetUniformLocation(programID, "MVP"));
  GLCall(TextureID = glGetUniformLocation(programID, "myTextureSampler"));
  GLCall(VID = glGetUniformLocation(programID, "V"));
  GLCall(MID = glGetUniformLocation(programID, "M"));

  // Enable depth test
  GLCall(glEnable(GL_DEPTH_TEST));
  // Accept fragment if it closer to the camera than the former one
  GLCall(glDepthFunc(GL_LESS));

  // Enable restarting triangle strip
  GLCall(glEnable(GL_PRIMITIVE_RESTART));


  //Vertexu* terrain = (Vertexu*) malloc(nb_vertices*sizeof(Vertexu));
  terrain = (Vertexun*) malloc(nb_vertices*sizeof(Vertexun));
  //GLfloat* uv = (GLfloat*) malloc((nb_vertices/3)*2*sizeof(GLfloat));
  indices = (unsigned int*) malloc( nb_indices*sizeof(GLuint) );
  load_models();
  va = new VertexArray();
  va->bind();
  vb = new VertexBuffer(terrain, nb_vertices*sizeof(Vertexun));
  layout = new VertexBufferLayout;
  layout->push(3,false);
  layout->push(2,false);
  layout->push(3,true);
  //layout->push(3,false);
  va->addBuffer(*vb, *layout);
  ib = new IndexBuffer(indices, nb_indices);
  ib->bind();
  vb->bind();

}
int load_models(){
  
  compute_height_map(height_map,t);
  // making the grid
  GLfloat step = 1.0f/ (float) w*w;
  //GLfloat step = 0.1f;
  GLuint ind = 0;
  int j;
  unsigned int tex_span = 6; // nb of squares the texture is mapped to
  float a,b,c;
  for(unsigned int i=0;i<w;i++){
    for(j=0;j<w;j++){
      a = j/(float)tex_span;
      b = i/(float)tex_span;
      terrain[ind++] = Vertexun(
          vec3(j*scl*6,height_map[j][i]*6,i*scl*6),
          vec2(a,b),
          vec3(0.0f,0.0f,0.0f)
          );
      /*terrain[ind++] = Vertexu(
          vec3(j*scl,height_map[j][i],i*scl),
          vec2(a,b)
          );*/
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

  unsigned int i1 = indices[0];
  unsigned int i2 = indices[1];
  unsigned int i3;
  vec3 triangle_normal;
  bool order = true;
  for(unsigned int i=2;i<nb_indices;i++){
    if(indices[i] == nb_vertices){
      // reached end of line (index == restart index)
      i1 = indices[++i];
      i2 = indices[++i];
      i++;
      order = true;
    }
    i3 = indices[i];
    // compute cross product in right order
    if(order)
      triangle_normal = normalize(cross(terrain[i2].pos-terrain[i1].pos,terrain[i3].pos-terrain[i1].pos));
    else
      triangle_normal = normalize(cross(terrain[i2].pos-terrain[i3].pos,terrain[i1].pos-terrain[i3].pos));
    terrain[i1].normal += triangle_normal;
    terrain[i2].normal += triangle_normal;
    terrain[i3].normal += triangle_normal;
    i1 = i2;
    i2 = i3;
    order = !order;
  }
  for(unsigned int i = 0; i<nb_vertices;i++){
    terrain[i].normal = normalize(terrain[i].normal);
  } 
  GLCall(glPrimitiveRestartIndex(nb_vertices));

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

  FastNoise* myNoise = (FastNoise*) malloc(nbOctave*sizeof(FastNoise));
  FastNoise valueNoise;
  FastNoise perlin(seed);
  perlin.SetNoiseType(FastNoise::Perlin); // Set the desired noise type
  perlin.SetFrequency( modfreq);
  valueNoise.SetNoiseType(FastNoise::Cubic); // Set the desired noise type
  for(unsigned int k=0;k<nbOctave;k++){
    myNoise[k] = FastNoise(seed);
    myNoise[k].SetNoiseType(FastNoise::Simplex); // Set the desired noise type
    myNoise[k].SetFrequency(pow(lacunarity, k) * 0.01f * (1.0f/zoom));
  }

  float a,b,c;
  for(unsigned int i=0;i<w;i++){
    for(unsigned int j=0;j<w;j++){
      a = 0;
      for(unsigned int k=0;k<nbOctave;k++){
        myNoise[k].SetFrequency(pow(lacunarity, k) * 0.01f * (1.0f/zoom));
        a += myNoise[k].GetNoise(i,j) * pow(persistence, k); 
      }
      b = valueNoise.GetNoise(i,j)*0.2f ;
      c = (perlin.GetNoise(i,j)+1)/2.0f;
      if (choosePerlin == 0)
        height_map[i][j] = a*(3*c*c) + b*(1-c);
      else if (choosePerlin == 1)
        height_map[i][j] = a;
      else if (choosePerlin == 2)
        height_map[i][j] = b;
      else
        height_map[i][j] = 3*c*c;
      height_map[i][j] *= zoom;
    }
  }
  free(myNoise);
}

int main_loop(){

  ImGui::CreateContext();
  ImGui_ImplGlfwGL3_Init(window, true);
  ImGui::StyleColorsDark();

  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  do{
    // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
    GLCall(glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    ImGui_ImplGlfwGL3_NewFrame();

    t += 0.1f;
    compute_mvp(mvp,v,m);
    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
    GLCall(glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]));
    GLCall(glUniformMatrix4fv(VID, 1, GL_FALSE, &v[0][0]));
    GLCall(glUniformMatrix4fv(MID, 1, GL_FALSE, &m[0][0]));

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

    if ((tempOctave != nbOctave) || (tempzoom != zoom) || (tempneg != neg) || (temppersistence != persistence)||
        (tempseed != seed) || (tempmodfreq != modfreq) ||(templacunarity != lacunarity) || (tempflatcoef != flatcoef) || (choosePerlin != tempchoose)){
      nbOctave = tempOctave;
      if (tempneg != neg)
        tempzoom = tempzoom * (- 1);
      zoom = tempzoom;
      neg = tempneg;
      seed = tempseed;
      lacunarity = templacunarity;
      persistence = temppersistence;
      flatcoef = tempflatcoef;
      modfreq = tempmodfreq;
      choosePerlin = tempchoose;
      load_models();
      ib->update();
      vb->update();
    }
    {
      ImGui::SliderInt("nbOctave", &tempOctave, 1, 10);
      if (((tempzoom > 0.5f) && (tempzoom < 9.5f)) ||
          ((tempzoom < -0.5f) && (tempzoom > -9.5f)))
        ImGui::SliderFloat("zoom_precision", &tempzoom, tempzoom - 0.5f, tempzoom + 0.5f);
      else if ((tempzoom <= 0.5f) && (tempzoom >= 0.0f))
        ImGui::SliderFloat("zoom_precision", &tempzoom, 0.0f, tempzoom + 0.5f);
      else if ((tempzoom >= -0.5f) && (tempzoom <= 0.0f))
        ImGui::SliderFloat("zoom_precision", &tempzoom, tempzoom - 0.5f, 0.0f);
      else if (tempzoom >= 9.5f)
        ImGui::SliderFloat("zoom_precision", &tempzoom, tempzoom - 0.5f, 10.0f);
      else if (tempzoom <= -9.5f)
        ImGui::SliderFloat("zoom_precision", &tempzoom, -10.0f, tempzoom + 0.5f);
      if (neg == 1)
        ImGui::SliderFloat("zoom", &tempzoom, 0.0f, 10.0f);
      else
        ImGui::SliderFloat("zoom", &tempzoom, -10.0f, 0.0f);      
      ImGui::SliderInt("0 is <0, 1 is >0", &tempneg, 0, 1);
      ImGui::SliderInt("seed", &tempseed, tempseed - 15, tempseed + 15);
      ImGui::SliderFloat("lacunarity", &templacunarity, 1.0f, 10.0f);      
      ImGui::SliderFloat("persistence", &temppersistence, -1.0f, 1.0f);      
      ImGui::SliderFloat("flatcoef", &tempflatcoef, 0.0f, 5.0f);      
      ImGui::SliderFloat("mod freq", &tempmodfreq, 0.0f, 0.01f);
      ImGui::SliderInt("a=1, b=2, c=3", &tempchoose, 0, 3);
      ImGui::Text("Combined persistence changement with");
      ImGui::Text("lacunarity changement to have fun");
      ImGui::Text("");
      ImGui::Text("Press SPACE while you need to move");
      ImGui::Text("the camera");
      ImGui::Text("");
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    
    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

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
