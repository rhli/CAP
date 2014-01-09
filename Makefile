CC = g++
OBJ_DIR = obj
SRC_DIR = src

all: main

main: $(SRC_DIR)/main.cc $(OBJ_DIR)/tinyxml2.o $(OBJ_DIR)/config.o $(OBJ_DIR)/graph.o 
	$(CC) -o $@ $^

$(OBJ_DIR)/tinyxml2.o : $(SRC_DIR)/tinyxml2.cpp $(SRC_DIR)/tinyxml2.h
	$(CC) -o $@ -c $<

$(OBJ_DIR)/config.o : $(SRC_DIR)/config.cc $(SRC_DIR)/config.hh
	$(CC) -o $@ -c $<

$(OBJ_DIR)/graph.o : $(SRC_DIR)/graph.cc $(SRC_DIR)/graph.hh
	$(CC) -o $@ -c $<

