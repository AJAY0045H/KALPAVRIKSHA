#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
 
#define MAX_POOL_SIZE 1000000
 
typedef struct{
    int startOffset;
    int blockSize;
    int allocationId;
    int isFree;
} Block;
 
typedef enum{
    FIRST_FIT = 1, 
    BEST_FIT = 2, 
    WORST_FIT = 3 
} Strategy;
 
unsigned char *memoryPool = NULL;
Block *blocks = NULL;
int currentBlockLength = 0;
int blockCapacity = 4;
int poolSize = 0;
int nextId = 1;
Strategy strategy = FIRST_FIT;
 
void freeBlocks(){
    free(blocks);
    free(memoryPool);
    blocks = NULL;
    memoryPool = NULL;
}
 
int readInteger(const char *prompt, int *result){
    printf("%s", prompt);
    int value;
        if(scanf(" %d", &value) != 1){
            //freeBlocks();
            printf("Enter a valid integer\n");
            int ch;
            while((ch = getchar()) != '\n' && ch != EOF);
            return 0;
    }
    *result = value;
    return 1;
}
 
int ensureCapacity(int requiredBlocks){
    Block *temp;
    int newCapacity;
 
    if(requiredBlocks <= blockCapacity) return 1;
    newCapacity = blockCapacity * 2;
    if(newCapacity < requiredBlocks) newCapacity = requiredBlocks;
    temp = realloc(blocks, newCapacity * sizeof(Block));
    if(!temp) return 0;
    blocks = temp;
    blockCapacity = newCapacity;
    return 1;
}
 
void insertBlock(int position, Block newBlock){
    memmove(&blocks[position + 1], &blocks[position], (currentBlockLength - position) * sizeof(Block));
    blocks[position] = newBlock;
    currentBlockLength++;
}
 
void removeBlock(int position){
    memmove(&blocks[position], &blocks[position + 1], (currentBlockLength - position - 1) * sizeof(Block));
    currentBlockLength--;
}
 
int findBlockById(int allocationId){
    for(int blockIndex = 0; blockIndex < currentBlockLength; blockIndex++){
        if(!blocks[blockIndex].isFree && blocks[blockIndex].allocationId == allocationId) return blockIndex;
    }
    return -1;
}
 
int firstFit(int requestedSize){
    for(int blockIndex = 0; blockIndex < currentBlockLength; blockIndex++){
        if(blocks[blockIndex].isFree && blocks[blockIndex].blockSize >= requestedSize) return blockIndex;
    }        
    return -1;
}
 
int bestFit(int requestedSize){
    int bestIndex = -1;

    for(int blockIndex = 0; blockIndex < currentBlockLength; blockIndex++){
        if(blocks[blockIndex].isFree && blocks[blockIndex].blockSize >= requestedSize &&
            (bestIndex == -1 || blocks[blockIndex].blockSize < blocks[bestIndex].blockSize))
            bestIndex = blockIndex;
    }        

    return bestIndex;
}
 
int worstFit(int requestedSize){
    int worstIndex = -1;

    for(int blockIndex = 0; blockIndex < currentBlockLength; blockIndex++){
        if(blocks[blockIndex].isFree && blocks[blockIndex].blockSize >= requestedSize &&
            (worstIndex == -1 || blocks[blockIndex].blockSize > blocks[worstIndex].blockSize))
            worstIndex = blockIndex;
    }
    
    return worstIndex;
}
 
int findFreeBlock(int requestedSize){
    switch(strategy){
        case BEST_FIT: return bestFit(requestedSize);
        case WORST_FIT: return worstFit(requestedSize);
        default: return firstFit(requestedSize);
    }
}
 
void mergeFreeBlocks(){
    int writeIndex = 0;
    for(int readIndex = 1; readIndex < currentBlockLength; readIndex++){
        if(blocks[writeIndex].isFree && blocks[readIndex].isFree){
            blocks[writeIndex].blockSize += blocks[readIndex].blockSize;
        }
        else{
            blocks[++writeIndex] = blocks[readIndex];
        }
    }
    currentBlockLength = writeIndex + 1;
}
 
int totalFreeBytes(){
    int freeBytes = 0;

    for(int blockIndex = 0; blockIndex < currentBlockLength; blockIndex++){
        if(blocks[blockIndex].isFree) freeBytes += blocks[blockIndex].blockSize;
    }

    return freeBytes;
}
 
void reportFailure(const char *operationName, int requestedSize){
    int freeBytes = totalFreeBytes();
    printf("%s failed: no contiguous free block is large enough.\n", operationName);
    
    if(freeBytes >= requestedSize){
        printf("External fragmentation: %d bytes are free in total\n", freeBytes);
        
    }
}
 
void splitBlock(int blockIndex, int newSize){
    if(blocks[blockIndex].blockSize > newSize){
        Block remainder ={
            blocks[blockIndex].startOffset + newSize,
            blocks[blockIndex].blockSize - newSize,
            0,
            1
        };
        insertBlock(blockIndex + 1, remainder);
        blocks[blockIndex].blockSize = newSize;
    }
}
 
void allocateBlock(int requestedSize){
    int blockIndex;
 
    if(requestedSize <= 0 || requestedSize > poolSize){
        printf("Invalid size. Must be between 1 and %d.\n", poolSize);
        return;
    }
    blockIndex = findFreeBlock(requestedSize);
    if(blockIndex < 0){
        reportFailure("Allocation", requestedSize);
        return;
    }
    if(blocks[blockIndex].blockSize > requestedSize && !ensureCapacity(currentBlockLength + 1)){
        puts("Allocation failed.");
        return;
    }
    splitBlock(blockIndex, requestedSize);
    blocks[blockIndex].isFree = 0;
    blocks[blockIndex].allocationId = nextId++;
    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n",
           blocks[blockIndex].allocationId,
           blocks[blockIndex].startOffset,
           blocks[blockIndex].blockSize);
}
 
void releaseBlock(int allocationId){
    int blockIndex;
 
    if(allocationId <= 0 || allocationId >= nextId){
        puts("Unknown allocation ID.");
        return;
    }
    blockIndex = findBlockById(allocationId);
    if(blockIndex < 0){
        puts("Allocation is already released.");
        return;
    }
    blocks[blockIndex].isFree = 1;
    blocks[blockIndex].allocationId = 0;
    mergeFreeBlocks();
    puts("Allocation released.");
}
 
void shrinkBlock(int blockIndex, int newSize){
    int releasedBytes = blocks[blockIndex].blockSize - newSize;
 
    if(blockIndex + 1 < currentBlockLength && blocks[blockIndex + 1].isFree){
        blocks[blockIndex + 1].startOffset -= releasedBytes;
        blocks[blockIndex + 1].blockSize += releasedBytes;
    } else{
        Block releasedBlock ={ blocks[blockIndex].startOffset + newSize, releasedBytes, 0, 1 };
        insertBlock(blockIndex + 1, releasedBlock);
    }
    blocks[blockIndex].blockSize = newSize;
}
 
int growInPlace(int blockIndex, int newSize){
    int extraBytes = newSize - blocks[blockIndex].blockSize;
    int nextIndex = blockIndex + 1;
 
    if(nextIndex >= currentBlockLength || !blocks[nextIndex].isFree ||
        blocks[nextIndex].blockSize < extraBytes)
        return 0;
    blocks[blockIndex].blockSize = newSize;
    blocks[nextIndex].startOffset += extraBytes;
    blocks[nextIndex].blockSize -= extraBytes;
    if(blocks[nextIndex].blockSize == 0) removeBlock(nextIndex);
    return 1;
}
 
void resizeBlock(int allocationId, int newSize){
    int oldIndex, targetIndex, oldSize;
 
    if(newSize <= 0 || newSize > poolSize){
        printf("Invalid size. Must be between 1 and %d.\n", poolSize);
        return;
    }
    if(allocationId <= 0 || allocationId >= nextId){
        puts("Unknown allocation ID.");
        return;
    }
    oldIndex = findBlockById(allocationId);
    if(oldIndex < 0){
        puts("Allocation is already released.");
        return;
    }
    oldSize = blocks[oldIndex].blockSize;
    if(newSize == oldSize){
        puts("Allocation already has that size.");
        return;
    }
 
    if(newSize < oldSize){
        int nextBlockIsFree = oldIndex + 1 < currentBlockLength && blocks[oldIndex + 1].isFree;
        if(!nextBlockIsFree && !ensureCapacity(currentBlockLength + 1)){
            puts("Resize failed: could not grow block metadata. Allocation unchanged.");
            return;
        }
        shrinkBlock(oldIndex, newSize);
        printf("Allocation %d shrunk to %d bytes.\n", allocationId, newSize);
        return;
    }
 
    if(growInPlace(oldIndex, newSize)){
        printf("Allocation %d expanded in place to %d bytes.\n", allocationId, newSize);
        return;
    }
 
    targetIndex = findFreeBlock(newSize);
    if(targetIndex < 0){
        reportFailure("Resize", newSize);
        puts("Original allocation is unchanged.");
        return;
    }
    if(blocks[targetIndex].blockSize > newSize && !ensureCapacity(currentBlockLength + 1)){
        puts("Resize failed: could not grow block metadata. Allocation unchanged.");
        return;
    }
    memcpy(memoryPool + blocks[targetIndex].startOffset,
           memoryPool + blocks[oldIndex].startOffset,
           oldSize);
    splitBlock(targetIndex, newSize);
    if(oldIndex > targetIndex) oldIndex++;
    blocks[targetIndex].isFree = 0;
    blocks[targetIndex].allocationId = allocationId;
    blocks[oldIndex].isFree = 1;
    blocks[oldIndex].allocationId = 0;
    printf("Allocation %d moved to Start: %d, Size: %d\n", allocationId, blocks[targetIndex].startOffset, newSize);
    mergeFreeBlocks();
}
 
void printMemoryMap(){
    printf("Memory Map (%d bytes):\n", poolSize);
    for(int blockIndex = 0; blockIndex < currentBlockLength; blockIndex++){
        if(blocks[blockIndex].isFree){
            printf("[Free: %d bytes, Start: %d]\n",
                blocks[blockIndex].blockSize, blocks[blockIndex].startOffset);
            }
        else{
            printf("[Allocated #%d: %d bytes, Start: %d]\n",
                   blocks[blockIndex].allocationId,
                   blocks[blockIndex].blockSize,
                   blocks[blockIndex].startOffset);
            }
    }
}
 
void printStatistics(){
    int allocatedBytes = 0, freeBytes = 0;
    int allocatedBlocks = 0, freeBlocks = 0;
    int largestFreeBlock = 0;
 
    for(int blockIndex = 0; blockIndex < currentBlockLength; blockIndex++){
        if(blocks[blockIndex].isFree){
            freeBytes += blocks[blockIndex].blockSize;
            freeBlocks++;
            if(blocks[blockIndex].blockSize > largestFreeBlock)
                largestFreeBlock = blocks[blockIndex].blockSize;
        } else{
            allocatedBytes += blocks[blockIndex].blockSize;
            allocatedBlocks++;
        }
    }
    printf("Total memory: %d bytes\n", poolSize);
    printf("Allocated: %d bytes\n", allocatedBytes);
    printf("Free: %d bytes\n", freeBytes);
    printf("Allocated blocks: %d\n", allocatedBlocks);
    printf("Free blocks: %d\n", freeBlocks);
    printf("Largest free block: %d bytes\n", largestFreeBlock);
}
 
void chooseStrategy(){
    int strategyChoice;
 
    while(!readInteger("Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ",&strategyChoice) || strategyChoice < 1 || strategyChoice > 3)
        puts("Invalid choice.");
    strategy = (Strategy)strategyChoice;
    printf(strategy == FIRST_FIT ? "Strategy set to First Fit.": strategy == BEST_FIT ? "Strategy set to Best Fit.": "Strategy set to Worst Fit.");
}
 
int initializePool(){
    int requestedPoolSize;
 
    while(!readInteger("Enter memory pool size: ", &requestedPoolSize)|| requestedPoolSize < 1 || requestedPoolSize > MAX_POOL_SIZE)
        printf("Invalid pool size. Must be between 1 and %d.\n", MAX_POOL_SIZE);
 
    memoryPool = malloc(requestedPoolSize);
    blocks = malloc(blockCapacity * sizeof(Block));
    if(!memoryPool || !blocks){
        puts("Memory allocation failed.");
        freeBlocks();
        return 0;
    }
    poolSize = requestedPoolSize;
    currentBlockLength = 1;
    blocks[0].startOffset = 0;
    blocks[0].blockSize = requestedPoolSize;
    blocks[0].allocationId = 0;
    blocks[0].isFree = 1;
    return 1;
}
 
int main(){
    int menuChoice, firstInput, size;
 
    if(!initializePool()) return 1;
    chooseStrategy();
 
    while(1){
        puts("\nMenu:");
        puts("1. Allocate memory");
        puts("2. Release memory");
        puts("3. Resize allocation");
        puts("4. Display memory map");
        puts("5. Display memory statistics");
        puts("6. Change allocation strategy");
        puts("7. Exit");
        if(!readInteger("Choice: ", &menuChoice)){
            puts("Invalid input.");
            continue;
        }
        switch(menuChoice){
            case 1:
                if(readInteger("Requested size: ", &firstInput)) allocateBlock(firstInput);
                else puts("Invalid input.");
                break;
            case 2:
                if(readInteger("Allocation ID: ", &firstInput)) releaseBlock(firstInput);
                else puts("Invalid input.");
                break;
            case 3:
                if(readInteger("Allocation ID: ", &firstInput) && readInteger("New size: ", &size))
                    resizeBlock(firstInput, size);
                else
                    puts("Invalid input.");
                break;
            case 4: printMemoryMap(); break;
            case 5: printStatistics(); break;
            case 6: chooseStrategy(); break;
            case 7:
                freeBlocks();
                printf("Exiting...\n");
                return 0;
            default:
                puts("Invalid choice.");
        }
    }
}