# Santase (66) Card Game Emulator

A C++ terminal-based implementation of the traditional two-player card game **Santase** (also known as 66).

## Overview

This project implements a fully functional Santase game emulator with comprehensive game logic, customizable rules, and save/load functionality. The game is played entirely through a command-line interface with colored suit symbols.

## Features

### Core Gameplay
  - **Full game logic** including stock-closing, card play, and scoring
- **Trump suit mechanics** with strict rule enforcement when stock closes
- **Marriage declarations** (K+Q of same suit) with configurable point values
- **Round-based gameplay** with match points accumulation

### Player Features
- **Command-line interface** with indexed card selection and colored output
- **Customizable settings** for target points, marriage bonuses, and optional rules
- **Save/Load functionality** to persist game progress
- **Game history tracking** across multiple rounds and games
- **Real-time status display** showing hands, trump suit, deck info, and points

### Game Commands
- `start` — Begin a new game
- `play <index>` — Play a card from your hand
- `marriage <suit>` — Declare a K+Q marriage (H/D/C/S)
- `close` — Close the stock (activate strict rules)
- `stop` — End the current round
- `surrender` — Give up the current round
- `surrender-forever` — Give up the entire match
- `save <name>` — Save game to file
- `load <name>` — Load game from file
- `hand`, `trump`, `status`, `history`, `rules` — Display game information

## Technical Details

- **Language:** C++
- **Compiler:** GCC

