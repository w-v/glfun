CFLAGS = -Wall -g
LIBS = -lGLEW -lglfw -lGL 
SRC_DIR = src/
H_DIR = include/
OBJ_DIR = obj/

main: $(OBJ_DIR)main.o $(OBJ_DIR)inputs.o $(OBJ_DIR)PerlinNoise.o obj/texture.o obj/VertexBufferLayout.o obj/VertexBuffer.o obj/VertexArray.o obj/IndexBuffer.o
	g++ $(CFLAGS) -o $@ $(OBJ_DIR)main.o $(OBJ_DIR)inputs.o $(OBJ_DIR)PerlinNoise.o obj/texture.o obj/VertexBufferLayout.o obj/VertexBuffer.o obj/VertexArray.o obj/IndexBuffer.o $(LIBS)

$(OBJ_DIR)main.o: $(SRC_DIR)main.cpp $(H_DIR)main.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o $@ -c $(SRC_DIR)main.cpp 


$(OBJ_DIR)inputs.o: $(SRC_DIR)inputs.cpp $(H_DIR)inputs.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/inputs.o -c $(SRC_DIR)inputs.cpp 
	

$(OBJ_DIR)PerlinNoise.o: $(SRC_DIR)PerlinNoise.cpp $(H_DIR)PerlinNoise.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/PerlinNoise.o -c $(SRC_DIR)PerlinNoise.cpp 
	
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

	

	




	



	




	




	
