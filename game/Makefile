# Makefile for Animal Chess Game
CXX=g++
CXXFLAGS=-std=c++14 -g -MMD -Wall
EXEC=animalchess

# Source files
CCFILES=$(wildcard *.cc)
OBJECTS=${CCFILES:.cc=.o}
DEPENDS=${CCFILES:.cc=.d}

# AI objects from ai directory
AI_OBJECTS=../ai/aiplayer.o ../ai/ainetwork.o ../ai/training_visualizer.o

# All objects for the main game
ALL_OBJECTS=${OBJECTS} ${AI_OBJECTS}

${EXEC}: ${ALL_OBJECTS}
	${CXX} ${ALL_OBJECTS} -o ${EXEC} -lX11

# Build AI objects
../ai/%.o: ../ai/%.cc
	$(MAKE) -C ../ai $(notdir $@)

-include ${DEPENDS}

.PHONY: clean objects
clean:
	rm -f ${EXEC} ${OBJECTS} ${DEPENDS}

objects: ${OBJECTS}
