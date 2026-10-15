/*
time complexity: O(n^2+nL)+O(nL)+O(n)
O(n^2) for checking roll number exists already or not
O(nL) for trimming name(number of students * length of name)
O(n) for printing roll numbers
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#define subjects 3
#define maxStudents 100

typedef struct Student {
    int rollNumber;
    char *name;
    int marks[subjects];
} student;

int readInteger(int *value);
int readName(char **name);
void freeStudents(student *students, int numberOfStudents);
int totalMarks(student student);
float averageMarks(int totalMark);
char gradeOfStudent(int average);
void printStars(char grade);
void printRollNumbers(int rollNumber, student *students);

//O(n)
int checkRollNumberExists(int rollNumber,student *students,int numberOfStudents) {
    for (int index = 0; index < numberOfStudents; index++) {
        if (rollNumber == students[index].rollNumber) {
            return 1;
        }
    }

    return 0;
}

int markIsInRange(int mark) {
    return (mark >= 0 && mark <= 100) ? 1 : 0;
}

int readInteger(int *value) {
    char input[100];
    char *end;
    long number;
    int character;
    size_t length = 0;
    int tooLong = 0;

    do {
        character = getchar();
        if (character == EOF) {
            return -1;
        }
    } while (isspace((unsigned char)character));

    do {
        if (length + 1 < sizeof(input)) {
            input[length++] = (char)character;
        } else {
            tooLong = 1;
        }
        character = getchar();
    } while (character != EOF && !isspace((unsigned char)character));

    if (character == EOF && length == 0) {
        return -1;
    }

    input[length] = '\0';

    if (tooLong) {
        return 0;
    }

    number = strtol(input, &end, 10);

    while (isspace((unsigned char)*end)) {
        end++;
    }

    if (input == end || *end != '\0' || number < INT_MIN || number > INT_MAX) {
        return 0;
    }

    *value = (int)number;
    return 1;
}

int readName(char **result) {
    size_t length = 0;
    size_t capacity = 64;
    char *name = malloc(capacity);
    int character;
    int hasAlphabet = 0;

    if (name == NULL) {
        return -1;
    }

    character = getchar();

    while (character == ' ' || character == '\t') {
        character = getchar();
    }

    if (character == EOF) {
        free(name);
        return -1;
    }

    while (character != '\n' && character != EOF) {
        if (isalpha((unsigned char)character)) {
            hasAlphabet = 1;
        } else if (character == ' ' || character == '\t') {
            int nextCharacter;

            while ((nextCharacter = getchar()) == ' ' || nextCharacter == '\t') {
            }

            if (nextCharacter == '\n' || nextCharacter == EOF) {
                break;
            }

            if (isalpha((unsigned char)nextCharacter)) {
                character = nextCharacter;
                if (length + 1 >= capacity) {
                    capacity *= 2;
                    char *newName = realloc(name, capacity);

                    if (newName == NULL) {
                        free(name);
                        return -1;
                    }

                    name = newName;
                }

                name[length++] = ' ';
                continue;
            }

            if (isdigit((unsigned char)nextCharacter) ||
                nextCharacter == '+' || nextCharacter == '-') {
                ungetc(nextCharacter, stdin);
                break;
            }

            while ((character = getchar()) != '\n' && character != EOF) {
            }
            free(name);
            return 0;
        } else {
            while ((character = getchar()) != '\n' && character != EOF) {
            }
            free(name);
            return 0;
        }

        if (length + 1 >= capacity) {
            capacity *= 2;
            char *newName = realloc(name, capacity);

            if (newName == NULL) {
                free(name);
                return -1;
            }

            name = newName;
        }

        name[length++] = (char)character;
        character = getchar();
    }

    while (length > 0 && name[length - 1] == ' ') {
        length--;
    }

    name[length] = '\0';

    if (length == 0 || !hasAlphabet) {
        free(name);
        return 0;
    }

    *result = name;
    return 1;
}

int main() {
    int numberOfStudents;

    while (readInteger(&numberOfStudents) != 1) {
        if (feof(stdin)) {
            printf("Input ended before the number of students was provided.\n");
            return 1;
        }
        printf("Please enter an integer value.\n");
    }

    if (numberOfStudents <= 0 || numberOfStudents > maxStudents) {
        printf("Number of students should be greater than 0 and less than or equal to %d",maxStudents);
        return 0;
    }

    student *students = malloc(numberOfStudents * sizeof(student));

    if (students == NULL) {
        printf("Error allocating memory for students.\n");
        return 1;
    }

    for (int student = 0; student < numberOfStudents; student++) {

        int rollNumber;

        while (1) {
            int inputResult = readInteger(&rollNumber);

            if (inputResult == -1) {
                printf("Input ended before all roll numbers were provided.\n");
                freeStudents(students, student);
                return 1;
            }

            if (inputResult == 0) {
                printf("Please enter an integer value.\n");
                continue;
            }

            if (checkRollNumberExists(rollNumber, students, student)) {
                printf("Roll Number already exists. Enter a different roll number.\n");
            } else {
                students[student].rollNumber = rollNumber;
                break;
            }
        }

        while(1){
            int inputResult = readName(&students[student].name);

            if (inputResult == -1) {
                printf("Input ended before all names were provided.\n");
                freeStudents(students, student);
                return 1;
            }

            if (inputResult == 0) {
                printf("Name must be non-empty and contain alphabets only.\n");
                continue;
            }

            break;
        }

        for (int subject = 0; subject < subjects; subject++) {
            int mark;

            while (1) {
                int inputResult = readInteger(&mark);

                if (inputResult == -1) {
                    printf("Input ended before all marks were provided.\n");
                    freeStudents(students, student + 1);
                    return 1;
                }

                if (inputResult == 0) {
                    printf("Please enter an integer value.\n");
                    continue;
                }

                if (markIsInRange(mark)) {
                    break;
                }

                printf("Entered mark should be in range of 0 to 100:\nEnter again\n");
            }

            students[student].marks[subject] = mark;
        }
    }

    for (int student = 0; student < numberOfStudents; student++) {
        printf("Roll: %d\nName: %s\n",students[student].rollNumber,students[student].name);

        int totalMark = totalMarks(students[student]);
        printf("Total: %d\n", totalMark);

        float averageMark = averageMarks(totalMark);
        printf("Average: %0.2f\n", averageMark);

        char grade = gradeOfStudent((int)averageMark);
        printf("Grade: %c\n", grade);

        if (grade == 'F') {
            printf("\n");
            continue;
        }

        printStars(grade);
        printf("\n");
    }
    //O(N) where N is the number of students
    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(numberOfStudents, students);

    freeStudents(students, numberOfStudents);
    return 0;
}

void freeStudents(student *students, int numberOfStudents) {
    for (int index = 0; index < numberOfStudents; index++) {
        free(students[index].name);
    }

    free(students);
}

void printStars(char grade) {
    printf("Performance:");

    switch (grade) {
        case 'A':
            printf("*****");
            break;
        case 'B':
            printf("****");
            break;
        case 'C':
            printf("***");
            break;
        case 'D':
            printf("**");
            break;
    }

    printf("\n");
}

int totalMarks(student student) {
    int totalMark = 0;

    for (int studentMark = 0; studentMark < subjects; studentMark++) {
        totalMark += student.marks[studentMark];
    }

    return totalMark;
}

float averageMarks(int totalMark) {
    return (float)totalMark / subjects;
}

char gradeOfStudent(int average) {
    char grade;

    if (average >= 85 && average <= 100) {
        grade = 'A';
    } else if (average >= 70) {
        grade = 'B';
    } else if (average >= 50) {
        grade = 'C';
    } else if (average >= 35) {
        grade = 'D';
    } else if (average < 35 && average >= 0) {
        grade = 'F';
    }

    return grade;
}

void printRollNumbers(int rollNumber, student *students) {
    if (rollNumber == 0) {
        return;
    }

    printRollNumbers(rollNumber - 1, students);
    printf("%d ", students[rollNumber - 1].rollNumber);
}