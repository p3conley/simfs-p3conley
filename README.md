# SimFS (Simulated File System)

## Overview
SimFS is a command-line file system simulator written in C. It demonstrates how an operating system manages disk blocks, tracks free space, stores file names, and stores file data.

The simulated disk has 100 blocks. Each block holds 512 bytes. Blocks 0 through 9 are reserved for the Free Map. Blocks 10 through 99 are used for files.

Each file is one block long. Inside each file block, bytes 0 through 31 store the file name, and bytes 32 through 511 store the file data.

## Team Members' Roles and Responsibilities
- Philip Conley: Completed the entire project individually, including coding, testing, documentation, slides, concept map, and video demonstration.

## Division of Work and Collaborative Experience
This project was completed individually. I did not work with teammates or collaborators.

## Concepts Demonstrated
- Simulated disk: shown in `char disk[TOTAL_BLOCKS][BLOCK_SIZE];`
- Free Map: shown in `int freeMap[TOTAL_BLOCKS];`
- File Table: shown in `char fileTable[TOTAL_BLOCKS][FILENAME_SIZE];`
- Reserved blocks 0-9: handled in `format_disk()`
- File blocks 10-99: handled using `FIRST_FILE_BLOCK`
- File name storage in bytes 0-31: handled in `create_file()`
- File data storage in bytes 32-511: handled in `write_file()` and `read_file()`
- Format command: implemented in `format_disk()`
- Create command: implemented in `create_file()`
- Read command: implemented in `read_file()`
- Write command: implemented in `write_file()`
- Delete command: implemented in `delete_file()`
- ls command: implemented in `list_files()`
- Command-line interface: implemented in `main()`
- Error handling: shown when a file is missing, already exists, has a name that is too long, or when no free blocks are available

## Build
```bash
gcc filesystem.c -o filesystem
