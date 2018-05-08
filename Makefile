CFLAGS = -Wall -g
LIBS = -lGLEW -lglfw -lGL 
SRC_DIR = src/
H_DIR = include/
OBJ_DIR = obj/

main: $(OBJ_DIR)main.o $(OBJ_DIR)inputs.o $(OBJ_DIR)PerlinNoise.o
	g++ $(CFLAGS) -o $@ $(OBJ_DIR)main.o $(OBJ_DIR)inputs.o $(OBJ_DIR)PerlinNoise.o $(LIBS)

$(OBJ_DIR)main.o: $(SRC_DIR)main.cpp $(H_DIR)main.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o $@ -c $(SRC_DIR)main.cpp 


$(OBJ_DIR)inputs.o: $(SRC_DIR)inputs.cpp $(H_DIR)inputs.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/inputs.o -c $(SRC_DIR)inputs.cpp 
	

$(OBJ_DIR)PerlinNoise.o: $(SRC_DIR)PerlinNoise.cpp $(H_DIR)PerlinNoise.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/PerlinNoise.o -c $(SRC_DIR)PerlinNoise.cpp 
	

	

	




	



	




	




	
