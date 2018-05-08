CFLAGS = -Wall -g
LIBS = -lGLEW -lglfw -lGL 
SRC_DIR = src/
H_DIR = include/
OBJ_DIR = obj/

main: $(OBJ_DIR)main.o $(OBJ_DIR)inputs.o
	g++ $(CFLAGS) -o $@ $(OBJ_DIR)main.o $(OBJ_DIR)inputs.o $(LIBS)

$(OBJ_DIR)main.o: $(SRC_DIR)main.cpp $(H_DIR)main.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o $@ -c $(SRC_DIR)main.cpp 


$(OBJ_DIR)inputs.o: $(SRC_DIR)inputs.cpp $(H_DIR)inputs.h
	g++ $(CFLAGS) -I./$(H_DIR) $(LIBS) -o obj/inputs.o -c $(SRC_DIR)inputs.cpp 
	

	

	

	




	



	




	




	
