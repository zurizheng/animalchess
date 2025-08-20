# Makefile for Animal Chess AI
CXX=g++
CXXFLAGS=-std=c++14 -g -MMD -Wall
AITRAIN=aitrain

# AI source files
CCFILES=$(wildcard *.cc)
OBJECTS=${CCFILES:.cc=.o}
DEPENDS=${CCFILES:.cc=.d}

# Game objects from game directory (filter out main.o for AI trainer)
GAME_OBJECTS=$(filter-out ../game/main.o, $(wildcard ../game/*.o))

# If game objects don't exist, we need to build them first
${AITRAIN}: ${OBJECTS} check-game-objects
	${CXX} ${OBJECTS} ${GAME_OBJECTS} -o ${AITRAIN} -lX11

.PHONY: check-game-objects
check-game-objects:
	@if [ ! -f ../game/controller.o ]; then \
		echo "Building game objects first..."; \
		$(MAKE) -C ../game objects; \
	fi
	$(eval GAME_OBJECTS := $(filter-out ../game/main.o, $(wildcard ../game/*.o)))

-include ${DEPENDS}

.PHONY: clean objects
clean:
	rm -f ${AITRAIN} ${OBJECTS} ${DEPENDS}

objects: ${OBJECTS}
