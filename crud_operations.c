#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
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
void closeFile(FILE *filePointer);
void createUser();
void displayUsers();
void updateUser();
void deleteUser();
int readInteger(const char *prompt, int *value, int minimum, int maximum);
int readName(const char *prompt, char *name);

FILE* openFile(const char *filename, const char *mode) {
    FILE *filePointer = fopen(filename, mode);
    if (filePointer == NULL) {
        printf("Unable to open file: %s\n", filename);
    }
    return filePointer;
}

void closeFile(FILE *filePointer) {
    if (filePointer != NULL) {
        fclose(filePointer);
    }
}

int readInteger(const char *prompt, int *value, int minimum, int maximum) {
    int extra;
    printf("%s", prompt);
    if (scanf("%d", value) != 1 || *value < minimum || *value > maximum) {
        while ((extra = getchar()) != '\n' && extra != EOF) {
        }
        printf("Invalid input. Please enter a number between %d and %d.\n",
               minimum, maximum);
        return 0;
    }

    extra = getchar();
    if (extra != '\n') {
        while ((extra = getchar()) != '\n' && extra != EOF) {
        }
        printf("Invalid input. Please enter one number only.\n");
        return 0;
    }
    return 1;
}

int readName(const char *prompt, char *name) {
    int extra;
    printf("%s", prompt);
    if (scanf("%99s", name) != 1) {
        while ((extra = getchar()) != '\n' && extra != EOF) {
        }
        printf("Invalid name.\n");
        return 0;
    }

    extra = getchar();
    if (extra != '\n') {
        while ((extra = getchar()) != '\n' && extra != EOF) {
        }
        printf("Invalid name. Use one word with at most 99 characters.\n");
        return 0;
    }
    return 1;
}

void createUser() {
    User newUser;
    if (!readInteger("Enter User ID: ", &newUser.id, 1, INT_MAX) ||
        !readName("Enter User Name: ", newUser.name) ||
        !readInteger("Enter User Age: ", &newUser.age, 1, 150)) {
        printf("User was not added.\n");
        return;
    }

    FILE *filePointer = openFile(userFile, "a");
    if (filePointer == NULL) return;

    fprintf(filePointer, "%d %s %d\n", newUser.id, newUser.name, newUser.age);
    closeFile(filePointer);

    printf("User added successfully.\n");
}

void displayUsers() {
    FILE *filePointer = openFile(userFile, "r");
    if (filePointer == NULL) return;

    User user;
    printf("\n------ User List ------\n");
    while (fscanf(filePointer, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        printf("ID: %d | Name: %s | Age: %d\n", user.id, user.name, user.age);
    }
    closeFile(filePointer);
}

void updateUser() {
    FILE *filePointer = openFile(userFile, "r");
    if (filePointer == NULL) return;

    FILE *tempPtr = openFile(tempFile, "w");
    if (tempPtr == NULL) {
        closeFile(filePointer);
        return;
    }

    int userId;
    bool found = false;
    if (!readInteger("Enter User ID to Update: ", &userId, 1, INT_MAX)) {
        closeFile(filePointer);
        closeFile(tempPtr);
        remove(tempFile);
        return;
    }

    User user;
    while (fscanf(filePointer, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == userId) {
            found = true;
            if (!readName("Enter New User Name: ", user.name) ||
                !readInteger("Enter New User Age: ", &user.age, 1, 150)) {
                closeFile(filePointer);
                closeFile(tempPtr);
                remove(tempFile);
                return;
            }
        }
        fprintf(tempPtr, "%d %s %d\n", user.id, user.name, user.age);
    }

    closeFile(filePointer);
    closeFile(tempPtr);
    remove(userFile);
    rename(tempFile, userFile);

    if (found)
        printf("User updated successfully.\n");
    else
        printf("User ID not found.\n");
}

void deleteUser() {
    FILE *filePointer = openFile(userFile, "r");
    if (filePointer == NULL) return;

    FILE *tempPtr = openFile(tempFile, "w");
    if (tempPtr == NULL) {
        closeFile(filePointer);
        return;
    }

    int userId;
    bool found = false;
    if (!readInteger("Enter User ID to Delete: ", &userId, 1, INT_MAX)) {
        closeFile(filePointer);
        closeFile(tempPtr);
        remove(tempFile);
        return;
    }

    User user;
    while (fscanf(filePointer, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == userId) {
            found = true;
            continue;
        }
        fprintf(tempPtr, "%d %s %d\n", user.id, user.name, user.age);
    }

    closeFile(filePointer);
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
        if (!readInteger("Enter your choice: ", &userChoice, ADD_USER, EXIT_PROGRAM)) {
            continue;
        }

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
}