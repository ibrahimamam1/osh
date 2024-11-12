# Simple Shell Project

This project is a simple command-line shell in C that supports executing Linux commands, a command history, and some special commands like `!!` and `!n`.

## Features

- Execute basic Linux commands (e.g., `ls`, `pwd`, etc.)
- Maintain a command history in shared memory across shell sessions
- Special commands:
  - `!!` - Re-execute the most recent command
  - `! n` - Re-execute the nth command in history
  - `history` - Display up to the last 10 commands
  - `exit` - Close the shell

## Usage

1. **Compile the Program:**
   ```bash
   gcc osh.c -o osh -lrt
2. **Run the Program:**
   ```bash
  ./osh
