#include <stdio.h>
#include <string.h>

#define subjects 3

typedef struct Student{
    int rollNumber;
    char name[100];
    int marks[subjects];
} student;

int totalMarks(student student);
float averageMarks(int totalMark);
char gradeOfStudent(int average);
void printStars(char grade);
void printRollNumbers(int rollNumber, student *students);

int checkRollNumberExists(int rollNumber, student *students, int numberOfStudents){
    for(int i = 0 ; i < numberOfStudents ; i++){
        if(rollNumber == students[i].rollNumber) return 1;
    }
    return 0;
}
int markIsInRange(int mark){
    return (mark >= 0 && mark <= 100)? 1 : 0;
}

int main(){
    int numberOfStudents;
    scanf("%d",&numberOfStudents);

    student students[numberOfStudents];
    for(int i = 0 ; i < numberOfStudents ; i++){

        int rollNumber;
        while(1){
            scanf("%d",&rollNumber);
            if(!checkRollNumberExists(rollNumber, students, i)) break;
            printf("Roll Number already exists\n");

        }
        students[i].rollNumber = rollNumber;

        char name[100];
        scanf("%99[^0-9]", name);
        strcpy(students[i].name, name);

        for(int j = 0; j < subjects; j++){
            int mark;
            while(1){
                scanf("%d", &mark);
                if(markIsInRange(mark)) break;
                printf("Entered mark should be in range of 0 to 100:\n");
            }
            students[i].marks[j] = mark;
        }

    }
    //printf("\n");
    for(int i = 0 ; i < numberOfStudents ; i++){
        printf("Roll: %d\nName: %s\n", students[i].rollNumber, students[i].name);
        
        int totalMark = totalMarks(students[i]);
        printf("Total: %d\n", totalMark);
        
        float averageMark = averageMarks(totalMark);
        printf("Average: %0.2f\n", averageMark);
        
        char grade = gradeOfStudent((int)averageMark);
        printf("Grade: %c\n", grade);
        
        if(grade == 'F'){
            printf("\n");
            continue;
        }
        printStars(grade);
        printf("\n");
    }

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(numberOfStudents, students);
    
    return 0;
}

void printStars(char grade){
    printf("Performance:");
    
    switch(grade){
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
int totalMarks(student student){
    int totalMark = 0;
    for(int i = 0 ; i < subjects ; i++){
        totalMark += student.marks[i];
    }

    return totalMark;
}

float averageMarks(int totalMark){
    return (float)totalMark / subjects;
}

char gradeOfStudent(int average){
    
    char grade;

    if(average >= 85 && average <= 100) grade = 'A';
    else if(average >= 70) grade = 'B';
    else if(average >= 50) grade = 'C';
    else if(average >= 35) grade = 'D';
    else if(average < 35 && average >= 0) grade = 'F';
   
    return grade;
}

void printRollNumbers(int rollNumber, student *students){

    if(rollNumber == 0){
        return;
    }
    printRollNumbers(rollNumber - 1, students);
    printf("%d ",students[rollNumber - 1].rollNumber);

}