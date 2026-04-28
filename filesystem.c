#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_BLOCKS 100
#define BLOCK_SIZE 512
#define FREE_MAP_BLOCKS 10
#define FIRST_FILE_BLOCK 10
#define FILENAME_SIZE 32
#define DATA_START 32
#define DATA_SIZE 480
#define MAX_INPUT 1024

/*
 * This is my simulated disk.
 * It has 100 blocks, and each block is 512 bytes.
 */
char disk[TOTAL_BLOCKS][BLOCK_SIZE];

/*
 * This is the free map.
 * 0 means the block is free.
 * 1 means the block is already being used.
 */
int freeMap[TOTAL_BLOCKS];

/*
 * This is the file table kept in memory.
 * The index matches the disk block number.
 * If a file is stored in block 10, its name is stored in fileTable[10].
 */
char fileTable[TOTAL_BLOCKS][FILENAME_SIZE];

/*
 * Removes the newline at the end of user input.
 * fgets keeps the newline, so this cleans it up.
 */
void trim_newline(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

/*
 * This formats the disk.
 * It clears all blocks, clears the file table,
 * and marks blocks 0 through 9 as used because they are reserved for the free map.
 */
void format_disk() {
    printf("Formatting disk...\n");

    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        freeMap[i] = 0;
        memset(disk[i], 0, BLOCK_SIZE);
        memset(fileTable[i], 0, FILENAME_SIZE);
    }

    for (int i = 0; i < FREE_MAP_BLOCKS; i++) {
        freeMap[i] = 1;
    }

    printf("Disk formatted successfully. FreeMap blocks 0-9 are now allocated.\n");
}

/*
 * This loads the file table from the disk.
 * In this simple version, the filename is stored in the first 32 bytes of each file block.
 */
void load_file_table() {
    printf("Loading file table from disk...\n");

    for (int i = FIRST_FILE_BLOCK; i < TOTAL_BLOCKS; i++) {
        if (freeMap[i] == 1 && disk[i][0] != '\0') {
            strncpy(fileTable[i], disk[i], FILENAME_SIZE - 1);
            fileTable[i][FILENAME_SIZE - 1] = '\0';
        }
    }

    printf("File table loaded successfully.\n");
}

/*
 * Searches the file table to find which block a file is stored in.
 * Returns the block number if found, or -1 if the file does not exist.
 */
int find_file_block(const char *filename) {
    for (int i = FIRST_FILE_BLOCK; i < TOTAL_BLOCKS; i++) {
        if (freeMap[i] == 1 && strcmp(fileTable[i], filename) == 0) {
            return i;
        }
    }
    return -1;
}

/*
 * Finds the first available block for a new file.
 * It starts at block 10 because blocks 0-9 are reserved.
 */
int find_free_block() {
    for (int i = FIRST_FILE_BLOCK; i < TOTAL_BLOCKS; i++) {
        if (freeMap[i] == 0) {
            return i;
        }
    }
    return -1;
}

/*
 * Creates a new file.
 * It finds a free block, marks that block as used,
 * and stores the filename in both the disk block and the file table.
 */
void create_file(const char *filename) {
    if (strlen(filename) >= FILENAME_SIZE) {
        printf("Error: Filename is too long. Max is 31 characters.\n");
        return;
    }

    if (find_file_block(filename) != -1) {
        printf("Error: File '%s' already exists.\n", filename);
        return;
    }

    int block = find_free_block();

    if (block == -1) {
        printf("Error: No free blocks available.\n");
        return;
    }

    freeMap[block] = 1;

    /* Store filename in bytes 0-31 of the block */
    strncpy(disk[block], filename, FILENAME_SIZE - 1);
    disk[block][FILENAME_SIZE - 1] = '\0';

    /* Also store filename in the in-memory file table */
    strncpy(fileTable[block], filename, FILENAME_SIZE - 1);
    fileTable[block][FILENAME_SIZE - 1] = '\0';

    printf("File '%s' created successfully at block %d.\n", filename, block);
}

/*
 * Reads a file.
 * The filename is used to find the block,
 * and then the data section of that block is printed.
 */
void read_file(const char *filename) {
    int block = find_file_block(filename);

    if (block == -1) {
        printf("Error: File '%s' not found.\n", filename);
        return;
    }

    printf("--- Content of '%s' ---\n", filename);
    printf("%s\n", &disk[block][DATA_START]);
}

/*
 * Writes data to a file.
 * The first 32 bytes of the block are for the file name,
 * so file data starts at byte 32.
 */
void write_file(const char *filename) {
    int block = find_file_block(filename);

    if (block == -1) {
        printf("Error: File '%s' not found.\n", filename);
        return;
    }

    char content[DATA_SIZE];

    printf("Enter content to write (max 255 bytes): ");

    if (fgets(content, sizeof(content), stdin) == NULL) {
        printf("Error: Could not read input.\n");
        return;
    }

    trim_newline(content);

    /* Clear old file data before writing new data */
    memset(&disk[block][DATA_START], 0, DATA_SIZE);

    /* Store the content starting at byte 32 of the block */
    strncpy(&disk[block][DATA_START], content, DATA_SIZE - 1);
}

/*
 * Deletes a file.
 * It frees the block in the free map,
 * clears the disk block, and removes the filename from the file table.
 */
void delete_file(const char *filename) {
    int block = find_file_block(filename);

    if (block == -1) {
        printf("Error: File '%s' not found.\n", filename);
        return;
    }

    freeMap[block] = 0;
    memset(disk[block], 0, BLOCK_SIZE);
    memset(fileTable[block], 0, FILENAME_SIZE);

    printf("File '%s' deleted successfully.\n", filename);
}

/*
 * Lists all files currently stored in the root directory.
 * Since this project only has one directory, this is the whole file list.
 */
void list_files() {
    printf("--- Files on disk ---\n");

    for (int i = FIRST_FILE_BLOCK; i < TOTAL_BLOCKS; i++) {
        if (freeMap[i] == 1 && fileTable[i][0] != '\0') {
            printf("%s\n", fileTable[i]);
        }
    }
}

/*
 * Prints the commands the user can type.
 */
void print_menu() {
    printf("\nWelcome to the simple file system simulator.\n");
    printf("Available commands:\n");
    printf("--> format\n");
    printf("--> create <filename>\n");
    printf("--> read <filename>\n");
    printf("--> write <filename>\n");
    printf("--> delete <filename>\n");
    printf("--> ls\n");
    printf("--> exit\n");
}

/*
 * Main command loop.
 * This keeps asking the user for commands until they type exit.
 */
int main() {
    char input[MAX_INPUT];

    format_disk();
    load_file_table();
    print_menu();

    while (1) {
        printf("\n> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        trim_newline(input);

        if (strcmp(input, "format") == 0) {
            format_disk();
            load_file_table();
        } else if (strncmp(input, "create ", 7) == 0) {
            create_file(input + 7);
        } else if (strncmp(input, "read ", 5) == 0) {
            read_file(input + 5);
        } else if (strncmp(input, "write ", 6) == 0) {
            write_file(input + 6);
        } else if (strncmp(input, "delete ", 7) == 0) {
            delete_file(input + 7);
        } else if (strcmp(input, "ls") == 0) {
            list_files();
        } else if (strcmp(input, "exit") == 0) {
            printf("Exiting file system simulator.\n");
            break;
        } else {
            printf("Error: Unknown command.\n");
        }
    }

    return 0;
}
