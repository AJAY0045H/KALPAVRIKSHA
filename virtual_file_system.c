#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME_LENGTH 50
#define MAX_INPUT_LENGTH 8192

typedef struct freeBlock {
    int blockIndex;
    struct freeBlock *nextFreeBlock;
    struct freeBlock *previousFreeBlock;
} FreeBlock;

typedef struct fileNode {
    char name[MAX_NAME_LENGTH + 1];
    int isDirectory;
    struct fileNode *parentDirectory;
    struct fileNode *nextSibling;
    struct fileNode *previousSibling;
    struct fileNode *firstChild;
    int *blockPointers;
    int allocatedBlockCount;
    int contentSize;
} FileNode;

unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
FreeBlock *freeListHead = NULL;
FreeBlock *freeListTail = NULL;
int freeBlockCount = 0;
FileNode *rootDirectory = NULL;
FileNode *currentDirectory = NULL;

void appendFreeBlock(int blockIndex) {
    FreeBlock *newFreeBlock = malloc(sizeof(FreeBlock));
    newFreeBlock->blockIndex = blockIndex;
    newFreeBlock->nextFreeBlock = NULL;
    newFreeBlock->previousFreeBlock = freeListTail;
    if (freeListTail != NULL) {
        freeListTail->nextFreeBlock = newFreeBlock;
    } else {
        freeListHead = newFreeBlock;
    }
    freeListTail = newFreeBlock;
    freeBlockCount++;
}

int removeHeadFreeBlock(void) {
    FreeBlock *headBlock = freeListHead;
    int blockIndex = headBlock->blockIndex;
    freeListHead = headBlock->nextFreeBlock;
    if (freeListHead != NULL) {
        freeListHead->previousFreeBlock = NULL;
    } else {
        freeListTail = NULL;
    }
    free(headBlock);
    freeBlockCount--;
    return blockIndex;
}

void initializeFreeList(void) {
    for (int blockIndex = 0; blockIndex < NUM_BLOCKS; blockIndex++) {
        appendFreeBlock(blockIndex);
    }
}

void destroyFreeList(void) {
    while (freeListHead != NULL) {
        removeHeadFreeBlock();
    }
}

FileNode *createNode(const char *name, int isDirectory) {
    FileNode *newNode = calloc(1, sizeof(FileNode));
    strcpy(newNode->name, name);
    newNode->isDirectory = isDirectory;
    return newNode;
}

void insertChild(FileNode *directory, FileNode *newChild) {
    newChild->parentDirectory = directory;
    if (directory->firstChild == NULL) {
        directory->firstChild = newChild;
        newChild->nextSibling = newChild;
        newChild->previousSibling = newChild;
        return;
    }
    FileNode *firstChild = directory->firstChild;
    FileNode *lastChild = firstChild->previousSibling;
    lastChild->nextSibling = newChild;
    newChild->previousSibling = lastChild;
    newChild->nextSibling = firstChild;
    firstChild->previousSibling = newChild;
}

void removeChild(FileNode *directory, FileNode *child) {
    if (child->nextSibling == child) {
        directory->firstChild = NULL;
    } else {
        child->previousSibling->nextSibling = child->nextSibling;
        child->nextSibling->previousSibling = child->previousSibling;
        if (directory->firstChild == child) {
            directory->firstChild = child->nextSibling;
        }
    }
    child->nextSibling = NULL;
    child->previousSibling = NULL;
}

FileNode *findChild(FileNode *directory, const char *name) {
    if (directory->firstChild == NULL) {
        return NULL;
    }
    FileNode *currentChild = directory->firstChild;
    do {
        if (strcmp(currentChild->name, name) == 0) {
            return currentChild;
        }
        currentChild = currentChild->nextSibling;
    } while (currentChild != directory->firstChild);
    return NULL;
}

void releaseFileBlocks(FileNode *file) {
    for (int blockPosition = 0; blockPosition < file->allocatedBlockCount; blockPosition++) {
        appendFreeBlock(file->blockPointers[blockPosition]);
    }
    free(file->blockPointers);
    file->blockPointers = NULL;
    file->allocatedBlockCount = 0;
    file->contentSize = 0;
}

void freeTree(FileNode *node) {
    while (node->firstChild != NULL) {
        FileNode *child = node->firstChild;
        removeChild(node, child);
        freeTree(child);
    }
    free(node->blockPointers);
    free(node);
}

void printPathSegments(FileNode *node) {
    if (node->parentDirectory == NULL) {
        return;
    }
    printPathSegments(node->parentDirectory);
    printf("/%s", node->name);
}

void printFullPath(FileNode *node) {
    if (node == rootDirectory) {
        printf("/");
    } else {
        printPathSegments(node);
    }
}

void printPrompt(void) {
    if (currentDirectory == rootDirectory) {
        printf("/ > ");
    } else {
        printf("%s > ", currentDirectory->name);
    }
    fflush(stdout);
}

int isValidName(const char *name) {
    if (strlen(name) > MAX_NAME_LENGTH) {
        printf("Error: name must be at most %d characters.\n", MAX_NAME_LENGTH);
        return 0;
    }
    if (findChild(currentDirectory, name) != NULL) {
        printf("Error: '%s' already exists.\n", name);
        return 0;
    }
    return 1;
}

void handleMakeDirectory(const char *name) {
    if (!isValidName(name)) {
        return;
    }
    insertChild(currentDirectory, createNode(name, 1));
    printf("Directory '%s' created successfully.\n", name);
}

void handleCreateFile(const char *name) {
    if (!isValidName(name)) {
        return;
    }
    insertChild(currentDirectory, createNode(name, 0));
    printf("File '%s' created successfully.\n", name);
}

void handleList(void) {
    if (currentDirectory->firstChild == NULL) {
        printf("(empty)\n");
        return;
    }
    FileNode *currentChild = currentDirectory->firstChild;
    do {
        printf("%s%s\n", currentChild->name, currentChild->isDirectory ? "/" : "");
        currentChild = currentChild->nextSibling;
    } while (currentChild != currentDirectory->firstChild);
}

void handleChangeDirectory(const char *name) {
    if (strcmp(name, "..") == 0) {
        if (currentDirectory->parentDirectory != NULL) {
            currentDirectory = currentDirectory->parentDirectory;
        }
    } else {
        FileNode *target = findChild(currentDirectory, name);
        if (target == NULL) {
            printf("Error: directory '%s' not found.\n", name);
            return;
        }
        if (!target->isDirectory) {
            printf("Error: '%s' is not a directory.\n", name);
            return;
        }
        currentDirectory = target;
    }
    printf("Moved to ");
    printFullPath(currentDirectory);
    printf("\n");
}

void handlePrintWorkingDirectory(void) {
    printFullPath(currentDirectory);
    printf("\n");
}

void handleWrite(const char *fileName, const char *inputLine) {
    FileNode *file = findChild(currentDirectory, fileName);
    if (file == NULL) {
        printf("Error: file '%s' not found.\n", fileName);
        return;
    }
    if (file->isDirectory) {
        printf("Error: '%s' is a directory.\n", fileName);
        return;
    }
    const char *openingQuote = strchr(inputLine, '"');
    const char *closingQuote = strrchr(inputLine, '"');
    if (openingQuote == NULL || openingQuote == closingQuote) {
        printf("Error: content must be enclosed in double quotes.\n");
        return;
    }
    const char *content = openingQuote + 1;
    int contentLength = (int)(closingQuote - content);
    int blocksNeeded = (contentLength + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (blocksNeeded > freeBlockCount + file->allocatedBlockCount) {
        printf("Error: disk full, not enough free blocks.\n");
        return;
    }
    releaseFileBlocks(file);
    if (blocksNeeded > 0) {
        file->blockPointers = malloc(blocksNeeded * sizeof(int));
    }
    for (int blockPosition = 0; blockPosition < blocksNeeded; blockPosition++) {
        int diskBlockIndex = removeHeadFreeBlock();
        int bytesRemaining = contentLength - blockPosition * BLOCK_SIZE;
        int bytesToCopy = bytesRemaining < BLOCK_SIZE ? bytesRemaining : BLOCK_SIZE;
        memcpy(virtualDisk[diskBlockIndex], content + blockPosition * BLOCK_SIZE, bytesToCopy);
        file->blockPointers[blockPosition] = diskBlockIndex;
    }
    file->allocatedBlockCount = blocksNeeded;
    file->contentSize = contentLength;
    printf("Data written successfully (size=%d bytes).\n", contentLength);
}

void handleRead(const char *fileName) {
    FileNode *file = findChild(currentDirectory, fileName);
    if (file == NULL) {
        printf("Error: file '%s' not found.\n", fileName);
        return;
    }
    if (file->isDirectory) {
        printf("Error: '%s' is a directory.\n", fileName);
        return;
    }
    for (int blockPosition = 0; blockPosition < file->allocatedBlockCount; blockPosition++) {
        int bytesRemaining = file->contentSize - blockPosition * BLOCK_SIZE;
        int bytesToPrint = bytesRemaining < BLOCK_SIZE ? bytesRemaining : BLOCK_SIZE;
        fwrite(virtualDisk[file->blockPointers[blockPosition]], 1, bytesToPrint, stdout);
    }
    printf("\n");
}

void handleDelete(const char *fileName) {
    FileNode *file = findChild(currentDirectory, fileName);
    if (file == NULL) {
        printf("Error: file '%s' not found.\n", fileName);
        return;
    }
    if (file->isDirectory) {
        printf("Error: '%s' is a directory, use rmdir.\n", fileName);
        return;
    }
    removeChild(currentDirectory, file);
    releaseFileBlocks(file);
    free(file);
    printf("File deleted successfully.\n");
}

void handleRemoveDirectory(const char *directoryName) {
    FileNode *directory = findChild(currentDirectory, directoryName);
    if (directory == NULL) {
        printf("Error: directory '%s' not found.\n", directoryName);
        return;
    }
    if (!directory->isDirectory) {
        printf("Error: '%s' is a file, use delete.\n", directoryName);
        return;
    }
    if (directory->firstChild != NULL) {
        printf("Error: directory '%s' is not empty.\n", directoryName);
        return;
    }
    removeChild(currentDirectory, directory);
    free(directory);
    printf("Directory removed successfully.\n");
}

void handleDiskUsage(void) {
    int usedBlockCount = NUM_BLOCKS - freeBlockCount;
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedBlockCount);
    printf("Free Blocks: %d\n", freeBlockCount);
    printf("Disk Usage: %.2f%%\n", 100.0 * usedBlockCount / NUM_BLOCKS);
}

int main(void) {
    char inputLine[MAX_INPUT_LENGTH];
    char commandName[MAX_INPUT_LENGTH];
    char argumentName[MAX_INPUT_LENGTH];

    initializeFreeList();
    rootDirectory = createNode("/", 1);
    currentDirectory = rootDirectory;

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    while (1) {
        printPrompt();
        if (fgets(inputLine, sizeof(inputLine), stdin) == NULL) {
            break;
        }
        inputLine[strcspn(inputLine, "\r\n")] = '\0';

        commandName[0] = '\0';
        argumentName[0] = '\0';
        int tokenCount = sscanf(inputLine, "%8191s %8191s", commandName, argumentName);

        if (tokenCount < 1) {
            continue;
        }
        if (strcmp(commandName, "exit") == 0) {
            break;
        }

        int hasArgument = tokenCount == 2;

        if (strcmp(commandName, "ls") == 0) {
            handleList();
        } else if (strcmp(commandName, "pwd") == 0) {
            handlePrintWorkingDirectory();
        } else if (strcmp(commandName, "df") == 0) {
            handleDiskUsage();
        } else if (strcmp(commandName, "mkdir") == 0 && hasArgument) {
            handleMakeDirectory(argumentName);
        } else if (strcmp(commandName, "create") == 0 && hasArgument) {
            handleCreateFile(argumentName);
        } else if (strcmp(commandName, "cd") == 0 && hasArgument) {
            handleChangeDirectory(argumentName);
        } else if (strcmp(commandName, "write") == 0 && hasArgument) {
            handleWrite(argumentName, inputLine);
        } else if (strcmp(commandName, "read") == 0 && hasArgument) {
            handleRead(argumentName);
        } else if (strcmp(commandName, "delete") == 0 && hasArgument) {
            handleDelete(argumentName);
        } else if (strcmp(commandName, "rmdir") == 0 && hasArgument) {
            handleRemoveDirectory(argumentName);
        } else {
            printf("Error: invalid command or missing argument.\n");
        }
    }

    freeTree(rootDirectory);
    destroyFreeList();
    printf("Memory released. Exiting program...\n");
    return 0;
}                                                                                                                                                         