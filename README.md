# TicTacToe-ConsoleAI

A feature-rich console-based Tic-Tac-Toe game in C++ with multiple gameplay modes and customizable settings. The game offers multiple gameplay modes, customizable settings, and supports different grid sizes beyond the traditional 3x3 board.

# Key Features
## Game Modes
Player vs Player (PVP): Two human players can compete against each other

Player vs Computer (PVC): Play against an AI with three difficulty levels

## Board Sizes
3x3 (classic Tic-Tac-Toe)

5x5 (requires 4 in a row to win)

7x7 (requires 5 in a row to win)

## AI Difficulty Levels
Easy: Makes random moves

Medium: Mixes random and strategic moves

Hard: Uses advanced strategy to block and win

# Additional Features
Colorful console interface with ANSI color codes

Sound effects for moves, wins, and draws

High score tracking

Customizable settings

Detailed game rules

Credits screen

# Technical Implementation
## Data Structures
2D array for game board (up to 7x7)

Structures for game settings and high scores

Enums for game state, difficulty, and grid size

## Key Functions
checkWin(): Implements win condition checking for all board sizes

getComputerMove(): Contains AI logic for all difficulty levels

displayBoard(): Handles colorful board rendering

drawBoxedMenu(): Creates consistent menu interfaces

Dependencies
Windows API for sound effects (windows.h)

Standard C++ libraries (iostream, cstdlib, ctime)

Console input handling (conio.h)
## How to Play

1. Compile the program using a C++ compiler
2. Run the executable
3. Use the menu to select game mode and options
4. During gameplay, press number/letter keys to make moves
5. Press Esc during game to return to main menu

## Controls

- Number keys (1-9) for first 9 positions
- Letter keys (A-Z) for larger boards
- Backspace to return to menu from info screens
- Esc to exit current game

## Requirements

- Windows OS (for sound effects)
- C++ compiler
- Console supporting ANSI color codes

## Future Enhancements

- Network multiplayer support
- Save/load game functionality
- More AI difficulty levels
- Graphical interface version
