//time complexity O(n^2)
/*hierarchy of the employees
CEO
  |->Engineering manager
        |->Team Lead
           |->Developer
           |->Tester
  |->HR manager
        |->Team Lead
           |-> Hr executive
           |->Tester
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define MAX 100
#define LINE_SIZE 256
#define DESIGNATION_COUNT 7

typedef struct employee{
    int employeeId;
    int reportingEmployeesLength;
    int capacity;
    char *name;
    char *designation;
    struct employee **reportingEmployees;
    struct employee *reportingManager;
} employee;

int length = 0, capacity = 16;
employee **employees = NULL;

const char *designationList[DESIGNATION_COUNT] = {
    "CEO",
    "Engineering Manager",
    "HR Manager",
    "Developer",
    "HR Executive",
    "Team Lead",
    "Tester"
};

const int designationLevel[DESIGNATION_COUNT] = {1, 2, 2, 4, 4, 3, 4};

int readInteger(int *value){
    int number = 0;
    int character;
    int result = scanf("%d", &number);
    if(result == EOF) return -1;
    while((character = getchar()) != '\n' && character != EOF);
    if(result != 1){
        printf("Enter a valid Integer\n");
        return 0;
    }
    *value = number;
    return 1;
}

int matchDesignation(const char *start, size_t len){
    for(int designationIndex = 0; designationIndex < DESIGNATION_COUNT; designationIndex++){
        if(strlen(designationList[designationIndex]) != len) continue;
        size_t charIndex = 0;
        while(charIndex < len && tolower((unsigned char)start[charIndex]) == tolower((unsigned char)designationList[designationIndex][charIndex])){
            charIndex++;
        }
        if(charIndex == len) return designationIndex;
    }
    return -1;
}

int getLevel(const char *designation){
    return designationLevel[matchDesignation(designation, strlen(designation))];
}

int parseLine(char *line, int *empId, char *name, char *designation, int *managerId){
    char *end;
    long id, mId = 0;

    while(isspace((unsigned char)*line)) line++;
    if(!isdigit((unsigned char)*line)) return -1;

    id = strtol(line, &end, 10);
    if(id <= 0 || id > INT_MAX || !isspace((unsigned char)*end)) return -1;

    line = end;
    while(isspace((unsigned char)*line)) line++;

    char *nameStart = line;
    while(*line && !isspace((unsigned char)*line)){
        if(!isalpha((unsigned char)*line)) return -1;
        line++;
    }
    size_t nameLength = line - nameStart;
    if(nameLength == 0 || nameLength >= MAX) return -1;

    while(isspace((unsigned char)*line)) line++;

    char *arrow = strstr(line, "->");
    char *desEnd = arrow ? arrow : line + strlen(line);
    while(desEnd > line && isspace((unsigned char)desEnd[-1])) desEnd--;

    int desIndex = matchDesignation(line, desEnd - line);
    if(desIndex == -1) return -1;

    if(arrow){
        char *managerStart = arrow + 2;
        while(isspace((unsigned char)*managerStart)) managerStart++;
        if(!isdigit((unsigned char)*managerStart)) return -1;

        mId = strtol(managerStart, &end, 10);
        if(mId <= 0 || mId > INT_MAX) return -1;
        if(mId == id){
            printf("Employee cannot report to themselves\n");
            return -1;
        }
        while(isspace((unsigned char)*end)) end++;
        if(*end != '\0') return -1;
    }
    if(mId == 0 && strcasecmp(designationList[desIndex],"ceo") != 0){
        printf("only ceo should have no reporting manager\n");
        return -1;
    }
    if(mId != 0 && strcasecmp(designationList[desIndex] ,"ceo") == 0){
        printf("The ceo should not have a reporting manager\n");
        return -1;
    }
    *empId = (int)id;
    memcpy(name, nameStart, nameLength);
    name[nameLength] = '\0';
    strcpy(designation, designationList[desIndex]);
    *managerId = (int)mId;
    return 1;
}

employee *findEmployee(int empId, int empIndex){
    if(empIndex < 0) return NULL;
    if(employees[empIndex]->employeeId == empId) return employees[empIndex];
    return findEmployee(empId, empIndex - 1);
}

void removeFromList(employee **list, int *count, employee *target){
    for(int position = 0; position < *count; position++){
        if(list[position] == target){
            for(int shiftIndex = position; shiftIndex < *count - 1; shiftIndex++){
                list[shiftIndex] = list[shiftIndex + 1];
            }
            (*count)--;
            return;
        }
    }
}

int addReport(employee *manager, employee *e){
    if(manager->reportingEmployeesLength == manager->capacity){
        employee **temp = realloc(manager->reportingEmployees, manager->capacity * 2 * sizeof(employee *));
        if(temp == NULL) return -1;
        manager->reportingEmployees = temp;
        manager->capacity *= 2;
    }
    manager->reportingEmployees[manager->reportingEmployeesLength++] = e;
    return 0;
}

int resizeEmployees(){
    if(length == capacity){
        employee **temp = realloc(employees, capacity * 2 * sizeof(employee *));
        if(temp == NULL) return -1;
        employees = temp;
        capacity *= 2;
    }
    return 0;
}

void freeEmployee(employee *e){
    free(e->name);
    free(e->designation);
    free(e->reportingEmployees);
    free(e);
}

int countUnder(employee *e){
    int total = 0;
    for(int reportIndex = 0; reportIndex < e->reportingEmployeesLength; reportIndex++){
        total += 1 + countUnder(e->reportingEmployees[reportIndex]);
    }
    return total;
}

void displayTree(employee *e, int level){
    printf("%*s%d - %s - %s\n", level * 4, "", e->employeeId, e->name, e->designation);
    for(int reportIndex = 0; reportIndex < e->reportingEmployeesLength; reportIndex++){
        displayTree(e->reportingEmployees[reportIndex], level + 1);
    }
}

void displayEmployeeHierarchy(int id){
    employee *emp = findEmployee(id, length - 1);
    if(emp == NULL){
        printf("Employee not found\n");
        return;
    }
    displayTree(emp, 0);
    printf("Total Employees Under %d: %d\n", id, countUnder(emp));
}

void displayAllEmployeesHierarchy(){
    if(length == 0){
        printf("No employees in the organization\n");
        return;
    }
    printf("===== ORGANIZATION HIERARCHY =====\n");
    int total = 0;
    for(int employeeIndex = 0; employeeIndex < length; employeeIndex++){
        if(employees[employeeIndex]->reportingManager == NULL){
            displayTree(employees[employeeIndex], 0);
            total += 1 + countUnder(employees[employeeIndex]);
        }
    }
    printf("Total Employees: %d\n", total);
}

int addEmployee(){
    int employeeId;
    int managerId;
    char name[MAX];
    char designation[MAX];
    char line[LINE_SIZE];

    if(fgets(line, sizeof(line), stdin) == NULL) return -1;
    if(strchr(line, '\n') == NULL){
        int character;
        while((character = getchar()) != '\n' && character != EOF);
        printf("Input too long\n");
        return -1;
    }
    if(parseLine(line, &employeeId, name, designation, &managerId) == -1){
        printf("Invalid input format\n");
        return -1;
    }
    if(findEmployee(employeeId, length - 1) != NULL){
        printf("Employee Id already exists\n");
        return -1;
    }

    employee *manager = NULL;
    if(managerId != 0){
        manager = findEmployee(managerId, length - 1);
        if(manager == NULL){
            printf("No employee found. Enter a valid manager Id\n");
            return -1;
        }
        if(getLevel(manager->designation) >= getLevel(designation)){
            printf("%s cannot be the manager of %s\n", manager->designation, designation);
            return -1;
        }
    }

    employee *newEmployee = calloc(1, sizeof(employee));
    if(newEmployee == NULL){
        printf("Memory allocation failed\n");
        return -1;
    }
    newEmployee->employeeId = employeeId;
    newEmployee->capacity = 4;
    newEmployee->reportingManager = manager;
    newEmployee->name = malloc(strlen(name) + 1);
    newEmployee->designation = malloc(strlen(designation) + 1);
    newEmployee->reportingEmployees = malloc(newEmployee->capacity * sizeof(employee *));

    if(newEmployee->name == NULL || newEmployee->designation == NULL || newEmployee->reportingEmployees == NULL
       || resizeEmployees() == -1 || (manager != NULL && addReport(manager, newEmployee) == -1)){
        freeEmployee(newEmployee);
        printf("Memory allocation failed\n");
        return -1;
    }
    strcpy(newEmployee->name, name);
    strcpy(newEmployee->designation, designation);

    employees[length++] = newEmployee;
    printf("Employee added successfully\n");
    return 0;
}

employee *findBestSibling(employee *removed, int childLevel){
    employee *best = NULL;
    employee *manager = removed->reportingManager;

    int candidateCount = (manager != NULL) ? manager->reportingEmployeesLength : length;

    for(int candidateIndex = 0; candidateIndex < candidateCount; candidateIndex++){
        employee *candidate = (manager != NULL) ? manager->reportingEmployees[candidateIndex] : employees[candidateIndex];

        if(candidate == removed) continue;
        if(manager == NULL && candidate->reportingManager != NULL) continue;
        if(getLevel(candidate->designation) >= childLevel) continue;

        if(best == NULL || candidate->reportingEmployeesLength < best->reportingEmployeesLength){
            best = candidate;
        }
    }
    return best;
}

void removeEmployee(){
    int id;
    printf("Enter Employee ID to remove: ");
    if(readInteger(&id) != 1) return;

    employee *e = findEmployee(id, length - 1);
    if(e == NULL){
        printf("Employee not found\n");
        return;
    }
    if(strcmp(e->designation, designationList[0]) == 0){
        printf("CEO cannot be deleted\n");
        return;
    }

    for(int reportIndex = 0; reportIndex < e->reportingEmployeesLength; reportIndex++){
        employee *child = e->reportingEmployees[reportIndex];
        employee *target = findBestSibling(e, getLevel(child->designation));
        if(target == NULL) target = e->reportingManager;

        child->reportingManager = target;
        if(target != NULL && addReport(target, child) == -1){
            printf("Memory allocation failed\n");
            exit(1);
        }
    }

    if(e->reportingManager != NULL){
        removeFromList(e->reportingManager->reportingEmployees, &e->reportingManager->reportingEmployeesLength, e);
    }
    removeFromList(employees, &length, e);

    freeEmployee(e);
    printf("Employee removed successfully\n");
}

int main(){
    employees = malloc(capacity * sizeof(employee *));
    if(employees == NULL){
        printf("Memory allocation failed.\n");
        return 1;
    }

    int running = 1;
    while(running){
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        int choice;
        int status = readInteger(&choice);
        if(status == -1) break;
        if(status == 0) continue;

        switch(choice){
            case 1:
                addEmployee();
                break;
            case 2:
                removeEmployee();
                break;
            case 3: {
                int id;
                printf("Enter Employee ID: ");
                if(readInteger(&id) == 1) displayEmployeeHierarchy(id);
                break;
            }
            case 4:
                displayAllEmployeesHierarchy();
                break;
            case 5:
                printf("Exiting...\n");
                running = 0;
                break;
            default:
                printf("Enter a valid option\n");
        }
    }

    for(int employeeIndex = 0; employeeIndex < length; employeeIndex++){
        freeEmployee(employees[employeeIndex]);
    }
    free(employees);
    return 0;
}