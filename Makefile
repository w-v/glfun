CFLAGS = -Wall -g
LIBS = -lGLEW -lglfw -lGL 
SRC_DIR = src/
H_DIR = include/
OBJ_DIR = obj/

main: $(OBJ_DIR)main.o $(OBJ_DIR)inputs.o $(OBJ_DIR)FastNoise.o obj/texture.o obj/VertexBufferLayout.o obj/VertexBuffer.o obj/VertexArray.o obj/IndexBuffer.o obj/Vertex.o obj/Imgui.o obj/Imgui_demo.o obj/Imgui_draw.o obj/Imgui_impl_glfw_gl3.o
	g++ $(CFLAGS) -o $@ $(OBJ_DIR)main.o $(OBJ_DIR)inputs.o $(OBJ_DIR)FastNoise.o obj/texture.o obj/VertexBufferLayout.o obj/VertexBuffer.o obj/VertexArray.o obj/IndexBuffer.o obj/Vertex.o obj/Imgui.o obj/Imgui_demo.o obj/Imgui_draw.o obj/Imgui_impl_glfw_gl3.o $(LIBS)

$(OBJ_DIR)main.o: $(SRC_DIR)main.cpp $(H_DIR)main.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o $@ -c $(SRC_DIR)main.cpp 


$(OBJ_DIR)inputs.o: $(SRC_DIR)inputs.cpp $(H_DIR)inputs.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/inputs.o -c $(SRC_DIR)inputs.cpp 
	

$(OBJ_DIR)FastNoise.o: $(SRC_DIR)FastNoise.cpp $(H_DIR)FastNoise.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/FastNoise.o -c $(SRC_DIR)FastNoise.cpp 
	
$(OBJ_DIR)texture.o: $(SRC_DIR)texture.cpp $(H_DIR)texture.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/texture.o -c $(SRC_DIR)texture.cpp 
	
$(OBJ_DIR)VertexBuffer.o: $(SRC_DIR)VertexBuffer.cpp $(H_DIR)VertexBuffer.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/VertexBuffer.o -c $(SRC_DIR)VertexBuffer.cpp 
	
$(OBJ_DIR)VertexBufferLayout.o: $(SRC_DIR)VertexBufferLayout.cpp $(H_DIR)VertexBufferLayout.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/VertexBufferLayout.o -c $(SRC_DIR)VertexBufferLayout.cpp 
	
$(OBJ_DIR)VertexArray.o: $(SRC_DIR)VertexArray.cpp $(H_DIR)VertexArray.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/VertexArray.o -c $(SRC_DIR)VertexArray.cpp 
	
$(OBJ_DIR)IndexBuffer.o: $(SRC_DIR)IndexBuffer.cpp $(H_DIR)IndexBuffer.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/IndexBuffer.o -c $(SRC_DIR)IndexBuffer.cpp 

$(OBJ_DIR)Vertex.o: $(SRC_DIR)Vertex.cpp $(H_DIR)Vertex.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/Vertex.o -c $(SRC_DIR)Vertex.cpp 

$(OBJ_DIR)Imgui.o: src/vendor/imgui/imgui.cpp
	g++ $(CFLAGS) -I./src/vendor/imgui $(LIBS) -o obj/Imgui.o -c src/vendor/imgui/imgui.cpp 

$(OBJ_DIR)Imgui_demo.o: src/vendor/imgui/imgui_demo.cpp
	g++ $(CFLAGS) -I./src/vendor/imgui_demo $(LIBS) -o obj/Imgui_demo.o -c src/vendor/imgui/imgui_demo.cpp 
	
$(OBJ_DIR)Imgui_draw.o: src/vendor/imgui/imgui_draw.cpp
	g++ $(CFLAGS) -I./src/vendor/imgui_draw $(LIBS) -o obj/Imgui_draw.o -c src/vendor/imgui/imgui_draw.cpp 
	
$(OBJ_DIR)Imgui_impl_glfw_gl3.o: src/vendor/imgui/imgui_impl_glfw_gl3.cpp
	g++ $(CFLAGS) -I./src/vendor/imgui_impl_glfw_gl3 $(LIBS) -o obj/Imgui_impl_glfw_gl3.o -c src/vendor/imgui/imgui_impl_glfw_gl3.cpp 




	



	




	




	
