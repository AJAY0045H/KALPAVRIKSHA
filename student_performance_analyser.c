#include <stdio.h>
#include <string.h>

#define subjects 3
#define maxStudents 100

typedef struct Student {
    int rollNumber;
    char name[100];
    int marks[subjects];
} student;

int totalMarks(student student);
float averageMarks(int totalMark);
char gradeOfStudent(int average);
void printStars(char grade);
void printRollNumbers(int rollNumber, student *students);

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

void trimName(char *name) {
    int start = 0;
    int end = strlen(name) - 1;

    while (name[start] == ' ' && start < end) {
        start++;
    }

    while (name[end] == ' ' && end > start) {
        end--;
    }

    int index = 0;

    for (int i = start; i <= end; i++) {
        name[index++] = name[i];
    }
}

int main() {
    int numberOfStudents;

    scanf("%d", &numberOfStudents);

    if (numberOfStudents <= 0 || numberOfStudents > maxStudents) {
        printf("Number of students should be greater than 0 and less than or equal to %d",maxStudents);
        return 0;
    }

    student *students = malloc(numberOfStudents * sizeof(student));

    if (students == NULL) {
        printf("Error in allocating memory");
        return 0;
    }

    for (int student = 0; student < numberOfStudents; student++) {

        int rollNumber;

        while (1) {
            scanf("%d", &rollNumber);

            if (!checkRollNumberExists(rollNumber, students, student)) {
                break;
            }

            printf("Roll Number already exists\nEnter again\n");
        }

        students[student].rollNumber = rollNumber;

        char name[200];
        while(1){
            scanf("%199[^0-9]", name);
            trimName(name);
            if(strlen(name) == 0) {
                printf("Name cannot be empty\nEnter again\n");
                continue;
            }
            break;
        }
        strcpy(students[student].name, name);

        for (int subject = 0; subject < subjects; subject++) {
            int mark;

            while (1) {
                scanf("%d", &mark);

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

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(numberOfStudents, students);

    free(students);
    return 0;
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