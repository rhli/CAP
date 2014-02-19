CC = ./csim.gpp
OBJ_DIR = obj
SRC_DIR = src
#CFLAG = -O3
CFLAG = 

all: main

main: $(SRC_DIR)/main.cc $(OBJ_DIR)/tinyxml2.o $(OBJ_DIR)/config.o $(OBJ_DIR)/graph.o $(OBJ_DIR)/randomGen.o \
	$(OBJ_DIR)/layoutGen.o $(OBJ_DIR)/striping.o $(OBJ_DIR)/stripe.o $(OBJ_DIR)/stripeManager.o \
	$(OBJ_DIR)/trafficManager.o $(OBJ_DIR)/NodeTree.o $(OBJ_DIR)/Switch.o
	$(CC) -o $@ $^ $(CFLAG)

$(OBJ_DIR)/tinyxml2.o : $(SRC_DIR)/tinyxml2.cpp $(SRC_DIR)/tinyxml2.h
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/config.o : $(SRC_DIR)/config.cc $(SRC_DIR)/config.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/graph.o : $(SRC_DIR)/graph.cc $(SRC_DIR)/graph.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/randomGen.o : $(SRC_DIR)/randomGen.cc $(SRC_DIR)/randomGen.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/layoutGen.o : $(SRC_DIR)/layoutGen.cc $(SRC_DIR)/layoutGen.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/striping.o : $(SRC_DIR)/striping.cc $(SRC_DIR)/striping.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/stripe.o : $(SRC_DIR)/stripe.cc $(SRC_DIR)/stripe.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/stripeManager.o : $(SRC_DIR)/stripeManager.cc $(SRC_DIR)/stripeManager.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/trafficManager.o : $(SRC_DIR)/trafficManager.cc $(SRC_DIR)/trafficManager.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/NodeTree.o : $(SRC_DIR)/NodeTree.cc $(SRC_DIR)/NodeTree.hh
	$(CC) -o $@ -c $< $(CFLAG)

$(OBJ_DIR)/Switch.o : $(SRC_DIR)/Switch.cc $(SRC_DIR)/Switch.hh
	$(CC) -o $@ -c $< $(CFLAG)

clean:
	rm main $(OBJ_DIR)/*.o


