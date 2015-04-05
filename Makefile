CC = ./csim.gpp
OBJ_DIR = obj
SRC_DIR = src
CFLAG = -Wall

all: directories main

directories: $(OBJ_DIR)

${OBJ_DIR}:
	mkdir -p $(OBJ_DIR)

main: $(SRC_DIR)/main.cc $(OBJ_DIR)/tinyxml2.o $(OBJ_DIR)/config.o $(OBJ_DIR)/FlowGraph.o $(OBJ_DIR)/randomGen.o \
	$(OBJ_DIR)/PlacementManager.o $(OBJ_DIR)/TrafficManager.o $(OBJ_DIR)/Topology.o $(OBJ_DIR)/Switch.o
	$(CC) -o $@ $^ $(CFLAG)

$(OBJ_DIR)/tinyxml2.o : $(SRC_DIR)/tinyxml2.cpp $(SRC_DIR)/tinyxml2.h
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/config.o : $(SRC_DIR)/config.cc $(SRC_DIR)/config.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/FlowGraph.o : $(SRC_DIR)/FlowGraph.cc $(SRC_DIR)/FlowGraph.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/randomGen.o : $(SRC_DIR)/randomGen.cc $(SRC_DIR)/randomGen.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/PlacementManager.o : $(SRC_DIR)/PlacementManager.cc $(SRC_DIR)/PlacementManager.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/stripe.o : $(SRC_DIR)/stripe.cc $(SRC_DIR)/stripe.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/TrafficManager.o : $(SRC_DIR)/TrafficManager.cc $(SRC_DIR)/TrafficManager.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/Topology.o : $(SRC_DIR)/Topology.cc $(SRC_DIR)/Topology.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/Switch.o : $(SRC_DIR)/Switch.cc $(SRC_DIR)/Switch.hh
	$(CC) -o $@ -c $< $(CFLAG)

clean:
	rm main $(OBJ_DIR)/*.o


