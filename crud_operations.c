#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define userFile "users.txt"
#define tempFile "temp.txt"

typedef struct User {
    int id;
    char name[100];
    int age;
} User;

typedef enum CrudOperation {
    ADD_USER = 1,
    DISPLAY_USERS,
    UPDATE_USER,
    DELETE_USER,
    EXIT_PROGRAM
} CrudOperation;

FILE* openFile(const char *filename, const char *mode);
void closeFile(FILE *filePtr);
void createUser();
void displayUsers();
void updateUser();
void deleteUser();

FILE* openFile(const char *filename, const char *mode) {
    FILE *filePtr = fopen(filename, mode);
    if (filePtr == NULL) {
        printf("Unable to open file: %s\n", filename);
    }
    return filePtr;
}

void closeFile(FILE *filePtr) {
    if (filePtr != NULL) {
        fclose(filePtr);
    }
}

void createUser() {
    FILE *filePtr = openFile(userFile, "a");
    if (filePtr == NULL) return;

    User newUser;
    printf("Enter User ID: ");
    scanf("%d", &newUser.id);
    printf("Enter User Name: ");
    scanf("%s", newUser.name);
    printf("Enter User Age: ");
    scanf("%d", &newUser.age);

    fprintf(filePtr, "%d %s %d\n", newUser.id, newUser.name, newUser.age);
    closeFile(filePtr);

    printf("User added successfully.\n");
}

void displayUsers() {
    FILE *filePtr = openFile(userFile, "r");
    if (filePtr == NULL) return;

    User user;
    printf("\n------ User List ------\n");
    while (fscanf(filePtr, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        printf("ID: %d | Name: %s | Age: %d\n", user.id, user.name, user.age);
    }
    closeFile(filePtr);
}

void updateUser() {
    FILE *filePtr = openFile(userFile, "r");
    if (filePtr == NULL) return;

    FILE *tempPtr = openFile(tempFile, "w");
    if (tempPtr == NULL) {
        closeFile(filePtr);
        return;
    }

    int userId;
    bool found = false;
    printf("Enter User ID to Update: ");
    scanf("%d", &userId);

    User user;
    while (fscanf(filePtr, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == userId) {
            found = true;
            printf("Enter New User Name: ");
            scanf("%s", user.name);
            printf("Enter New User Age: ");
            scanf("%d", &user.age);
        }
        fprintf(tempPtr, "%d %s %d\n", user.id, user.name, user.age);
    }

    closeFile(filePtr);
    closeFile(tempPtr);
    remove(userFile);
    rename(tempFile, userFile);

    if (found)
        printf("User updated successfully.\n");
    else
        printf("User ID not found.\n");
}

void deleteUser() {
    FILE *filePtr = openFile(userFile, "r");
    if (filePtr == NULL) return;

    FILE *tempPtr = openFile(tempFile, "w");
    if (tempPtr == NULL) {
        closeFile(filePtr);
        return;
    }

    int userId;
    bool found = false;
    printf("Enter User ID to Delete: ");
    scanf("%d", &userId);

    User user;
    while (fscanf(filePtr, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == userId) {
            found = true;
            continue;
        }
        fprintf(tempPtr, "%d %s %d\n", user.id, user.name, user.age);
    }

    closeFile(filePtr);
    closeFile(tempPtr);
    remove(userFile);
    rename(tempFile, userFile);

    if (found)
        printf("User deleted successfully.\n");
    else
        printf("User not found.\n");
}

int main() {
    bool start = true;
    int userChoice;

    FILE *initFile = openFile(userFile, "a");
    closeFile(initFile);

    while (start) {
        printf("\n====== USER MANAGEMENT SYSTEM ======\n");
        printf("%d. Add User\n", ADD_USER);
        printf("%d. Display Users\n", DISPLAY_USERS);
        printf("%d. Update User by ID\n", UPDATE_USER);
        printf("%d. Delete User by ID\n", DELETE_USER);
        printf("%d. Exit\n", EXIT_PROGRAM);
        printf("Enter your choice: ");
        scanf("%d", &userChoice);

        CrudOperation operation = (CrudOperation)userChoice;

        switch (operation) {
            case ADD_USER:
                createUser();
                break;
            case DISPLAY_USERS:
                displayUsers();
                break;
            case UPDATE_USER:
                updateUser();
                break;
            case DELETE_USER:
                deleteUser();
                break;
            case EXIT_PROGRAM:
                start = false;
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;