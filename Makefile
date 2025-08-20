# Top-level Makefile for Animal Chess with AI
CXX=g++
CXXFLAGS=-std=c++14 -g -MMD -Wall

# Directories
GAME_DIR=game
AI_DIR=ai

# Executables
EXEC=animalchess
AITRAIN=aitrain

.PHONY: all clean game ai

all: game ai

game:
	@echo "Building main game..."
	$(MAKE) -C $(GAME_DIR)
	cp $(GAME_DIR)/$(EXEC) .

ai:
	@echo "Building AI trainer..."
	$(MAKE) -C $(AI_DIR)
	cp $(AI_DIR)/$(AITRAIN) .

clean:
	@echo "Cleaning all directories..."
	$(MAKE) -C $(GAME_DIR) clean
	$(MAKE) -C $(AI_DIR) clean
	rm -f $(EXEC) $(AITRAIN)

.PHONY: help
help:
	@echo "Animal Chess Build System"
	@echo "========================"
	@echo "Available targets:"
	@echo "  all     - Build both game and AI trainer"
	@echo "  game    - Build main game only"
	@echo "  ai      - Build AI trainer only"
	@echo "  clean   - Clean all build files"
	@echo "  help    - Show this help message"