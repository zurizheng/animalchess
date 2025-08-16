#Generalized Makefile (C++ specific)
CXX=g++ #special Makefile variable indicating which compiler to use
CXXFLAGS= -std=c++14 -g -MMD #-Wall #MMD auto generates dependencies 
EXEC=animalchess
CCFILES=$(wildcard *.cc)
OBJECTS=${CCFILES:.cc=.o}
DEPENDS=${CCFILES:.cc=.d}

${EXEC}:${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC} -lX11

-include ${DEPENDS}

.PHONY: clean
clean:
	rm -f ${EXEC} ${OBJECTS} ${DEPENDS}

.PHONY: tidy
tidy:
	rm -f ${OBJECTS} ${DEPENDS}