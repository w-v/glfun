#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>

#include <inputs.h>
#include <main.h>

double currentTime ;
float deltaTime ;
double lastTime ;

// position
glm::vec3 position = glm::vec3( 0, 10, 0 );
// horizontal angle : toward -Z
float horizontalAngle = 0;
// vertical angle : 0, look at the horizon
float verticalAngle = -M_PI/2;
// Initial Field of View
float initialFoV = 45.0f;

void compute_mvp(glm::mat4& mvp, glm::mat4& v, glm::mat4& m){


 float speed = 10.0f; // 3 units / second
 float mouseSpeed = 0.05f;  

 // Get mouse position
 double xpos, ypos;
 glfwGetCursorPos(window, &xpos, &ypos);
 glfwSetCursorPos(window, 1024/2, 768/2);

 // Compute new orientation
 horizontalAngle += mouseSpeed * deltaTime * float(1024/2 - xpos );
 verticalAngle   += mouseSpeed * deltaTime * float( 768/2 - ypos );

 glm::vec3 direction(
     cos(verticalAngle) * sin(horizontalAngle),
     sin(verticalAngle),
     cos(verticalAngle) * cos(horizontalAngle)
     );

 // Right vector
 glm::vec3 right = glm::vec3(
     sin(horizontalAngle - 3.14f/2.0f),
     0,
     cos(horizontalAngle - 3.14f/2.0f)
     );
 // Up vector : perpendicular to both direction and right
 glm::vec3 up = glm::cross( right, direction );

 // Move forward
 if (glfwGetKey(window, GLFW_KEY_UP ) == GLFW_PRESS){
   position += direction * deltaTime * speed;
 }
 // Move backward
 if (glfwGetKey(window, GLFW_KEY_DOWN ) == GLFW_PRESS){
   position -= direction * deltaTime * speed;
 }
 // Strafe right
 if (glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
   position += right * deltaTime * speed;
 }
 // Strafe left
 if (glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS){
   position -= right * deltaTime * speed;
 }

 int width = 4;
 int height = 3;
 GLfloat rotation = 0;

 glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

 glm::mat4 View = glm::lookAt(
     position,
     position+direction,
     up
     );
 
 glm::mat4 sc = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
 glm::mat4 rot = glm::rotate(rotation, glm::vec3(1.0f,0.0f,0.0f));
 glm::mat4 tr = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));

 // Model matrix : an identity matrix (model will be at the origin)
 //glm::mat4 Model = glm::mat4(1.0f);
 glm::mat4 Model = tr * rot * sc;
 m = Model;
 v = View;
 // Our ModelViewProjection : multiplication of our 3 matrices
 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

 currentTime = glfwGetTime();
 deltaTime = float(currentTime - lastTime);
 lastTime = currentTime;
}
