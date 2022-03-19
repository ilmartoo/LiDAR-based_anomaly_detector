# Compiler options
CC	  := g++
FLAGS := -g -Wall -O0

SRC_DIR  := src/main
TEST_DIR := src/test
INC_DIR  := include
LIB_DIR  := lib

SOURCES	 := $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/*/*.cpp)
LIBS	 := 

# Program name
PROGRAM	= anomaly_detection

$(PROGRAM):
# @echo $(CC) $(FLAGS) -o $@ $(SOURCES) -I $(INCLUDES) $(LIBS)
	$(CC) $(FLAGS) -o $@ $(SOURCES) -I $(INC_DIR) $(LIBS)