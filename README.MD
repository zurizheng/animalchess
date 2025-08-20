# 🦁 Animal Chess

A strategy board game with AI opponents powered by deep reinforcement learning.

![C++14](https://img.shields.io/badge/C%2B%2B-14-blue)
![AI](https://img.shields.io/badge/AI-Deep%20Q--Network-green)
![Build](https://img.shields.io/badge/build-passing-brightgreen)

## About

Animal Chess is based on the classic "Dou Shou Qi" (Fight of Animals). Players move animal pieces with different abilities across a board with water, traps, and dens. The goal: reach the opponent’s den or capture all their pieces.

## Features

- 8 animal pieces, each with unique strengths and different moves
- Text and graphical interfaces
- AI opponents using Deep Q-Networks
- Train your own AI models

## Quick Start

**Requirements:**  
- C++14 compiler  
- X11 libraries (for graphics)  
- Make

**Install & Run:**
```bash
git clone https://github.com/zurizheng/animalchess.git
cd animalchess
make all
./animalchess               # Play (human vs human)
./animalchess -ai           # Play vs AI
./animalchess -graphics     # Play with graphics
./aitrain                   # Train AI (1000 games)
```

## Game Options

| Flag         | Description                   |
|--------------|-------------------------------|
| `-graphics`  | Enable graphical interface    |
| `-ai`        | Play against AI               |
| `-pov`       | Point-of-view mode            |
| `-splitview` | Split view for multiplayer    |

## Project Structure

```
animalchess/
├── game/        # Core game logic
├── ai/          # AI implementation
├── animalchess  # Main game executable
├── aitrain      # AI trainer
└── board.txt    # Board config
```

## AI Overview

- Deep Q-Network with 4-layer neural net
- Learns by self-play and experience replay
- Board state as 396-feature vector
- 32 possible actions (8 pieces × 4 directions)

## Training AI

```bash
./aitrain -games 5000      # Train for 5000 games
./aitrain -graphics        # Watch training
```
Tips: Start with a few thousand games, test, then train more. Models save automatically.

## Game Rules

- **Animals:** Rat(1) < Cat(2) < Dog(3) < Wolf(4) < Leopard(5) < Tiger(6) < Lion(7) < Elephant(8). With the exception that Rat(1) wins against Elephant(8)
- **Terrain:** Water (rats only), traps, dens
- **Unique moves** Rats can go in water, tiger and lions can jump over water
- **Win:** Enter opponent’s den or capture all their pieces

**Ready to play?** Run `./animalchess -ai` and challenge the AI!