#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "Players_data.h"

#define MAX_LINE 256

char *roles[] = {
    "",
    "Batsman",
    "Bowler",
    "All-rounder"
};

typedef struct playerNode{
    int playerId;
    int totalRuns;  
    int wickets;

    float performanceIndex;
    float battingAverage;
    float strikeRate;
    float economyRate;

    char *teamName;
    char *role;
    char *name;

    struct playerNode *nextPlayer;
}players;

typedef struct teamNode{
int teamId;
int totalPlayers;

float averageBattingStrikeRate;
char *teamName;

struct teamNode *nextTeam;
players *playerHead;

}teams;

teams *teamHead = NULL;

void freeList(void) {
    while (teamHead) {
        players *Player = teamHead->playerHead;
        while (Player) {
            players *next = Player->nextPlayer;
            free(Player->name);
            free(Player);
            Player = next;
        }
        teams *nextteam = teamHead->nextTeam;
        free(teamHead);
        teamHead = nextteam;
    }
}

teams *createTeamNode(int teamId, const char *teamName){
    teams *newNode = malloc(sizeof(teams));
    if(newNode == NULL){
        printf("Memory allocation failed\n");
        freeList();
        exit(1);
    }
    //initialising to default values
    newNode->teamId = teamId;
    newNode->totalPlayers = 0;
    newNode->averageBattingStrikeRate = 0.0f;
    newNode->teamName = (char *)teamName;
    newNode->playerHead = NULL;
    newNode->nextTeam = NULL;

    return newNode;
}

int roleRank(const char *role){
    if(strcmp(role, "Batsman") == 0) return 0;
    if(strcmp(role, "Bowler") == 0) return 1;
    return 2;
}

float findAvgBattingStrikeRate(teams *data){
    float sum = 0.0f;
    int count = 0;
    players *curr = data->playerHead;
    while(curr != NULL){
        if(strcmp(curr->role,"Batsman") == 0 || strcmp(curr->role,"All-rounder") == 0){
            sum += curr->strikeRate;
            count++;
        }
        curr = curr->nextPlayer;
    }
    return count == 0 ? 0.0f : sum / count;
}

void insertPlayer(teams *playerTeam, players *player){
    int rank = roleRank(player->role);
    players **cur = &playerTeam->playerHead;
    while(*cur && roleRank((*cur)->role) < rank){
        cur = &(*cur)->nextPlayer;
    }
    while(*cur && roleRank((*cur)->role) == rank &&
        (*cur)->performanceIndex >= player->performanceIndex){
        cur = &(*cur)->nextPlayer;
    }
    player->nextPlayer = *cur;
    *cur = player;
    playerTeam->totalPlayers++;
    playerTeam->averageBattingStrikeRate = findAvgBattingStrikeRate(playerTeam);
}

void initialiseTeams(){
    teams *tail = NULL;
    for(size_t teamIndex = 0; teamIndex < (size_t)teamCount; teamIndex++){
        teams *node = createTeamNode((int)teamIndex + 1, teamNames[teamIndex]);
        if(teamHead == NULL){
            teamHead = node;
        }else{
            tail->nextTeam = node;
        }
        tail = node;
    }
}

teams *findTeamByName(char *name){
    teams *dummyHead = teamHead;
    while(dummyHead){
        if(strcmp(dummyHead->teamName,name) == 0){
            return dummyHead;
        }
        dummyHead = dummyHead->nextTeam;
    }
    return NULL;
}

float calculatePerformanceIndex(players data){
    float performanceIndex = 0.0f;
    if(strcmp(data.role,"Batsman") == 0){
        performanceIndex = (data.battingAverage * data.strikeRate) / 100;
    }else if(strcmp(data.role,"Bowler") == 0){
        performanceIndex = (data.wickets * 2) + (100 - data.economyRate);
    }else{
        performanceIndex = ((data.battingAverage * data.strikeRate) / 100) + (data.wickets * 2);
    }
    return performanceIndex;
}

players *createPlayerNode(const Player *data){
    players *newNode = malloc(sizeof(players));
    if(newNode == NULL){
        printf("Memory allocation failed\n");
        freeList();
        exit(1);
    }
    size_t nameLength = strlen(data->name);
    newNode->playerId = data->id;
    newNode->teamName = (char *)data->team;
    newNode->name = malloc(nameLength + 1);
    if(newNode->name == NULL){
        printf("Memory allocation failed\n");
        free(newNode);
        freeList();
        exit(1);
    }
    strcpy(newNode->name, data->name);
    newNode->battingAverage = data->battingAverage;
    newNode->economyRate = data->economyRate;
    newNode->nextPlayer = NULL;
    newNode->role = (char *)data->role;
    newNode->totalRuns = data->totalRuns;
    newNode->strikeRate = data->strikeRate;
    newNode->wickets = data->wickets;
    newNode->performanceIndex = calculatePerformanceIndex(*newNode);

    return newNode;
}

void initialisePlayers(){
    for(size_t playerIndex = 0; playerIndex < (size_t)playerCount; playerIndex++){
        players *node = createPlayerNode(&playerData[playerIndex]);
        teams *playerTeam = findTeamByName(node->teamName);
        if(playerTeam == NULL){
            printf("Team not found\n");
            free(node->name);
            free(node);
            continue;
        }
        insertPlayer(playerTeam, node);
    }
}

teams *findTeamById(int id) {
    for (teams *t = teamHead; t; t = t->nextTeam)
        if (t->teamId == id) return t;
    return NULL;
}

int check(int playerId, int teamId) {
    teams *team = findTeamById(teamId);
    if(team == NULL){
        return 1;
    }
    for (players *player = team->playerHead; player; player = player->nextPlayer)
        if (player->playerId == playerId) return 0;
    return 1;
}

void clearInput(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

void readInteger(int *value){
    int number;
    while(scanf(" %d",&number) != 1 || number < 0){
        printf("Enter a Valid Integer and it should be a positive number\n");
        clearInput();
    
    }
    clearInput();
    *value = number;
}

void readFloat(float *value){
    float number;
    while(scanf(" %f",&number) != 1 || number < 0){
        printf("Enter a valid floating point value\n");
        clearInput();
    }
    clearInput();
    *value = number;
}

int validName(char *name){
    if(strlen(name) > 50) return 0;
    for(int index = 0 ; name[index] != '\0' ;index++){
        if(!isspace((unsigned char)name[index]) && !isalpha((unsigned char)name[index]) && (unsigned char)name[index] != '-') return 0;
    }
    return 1;
}

int addPlayer(){
    int teamId, playerId, Role, totalRuns, wickets;
    float battingAverage, strikeRate, economyRate;
    char playerName[MAX_LINE];
    
    players *newNode = malloc(sizeof(players));
    if(newNode == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }

    //read team id
    teams *currTeam = NULL;
    while(1){
        printf("Enter Team ID to add player: ");
        readInteger(&teamId);
        currTeam = findTeamById(teamId);
        if(currTeam != NULL) break;
        else printf("Team Id does not exists.\n");
    }
    if(currTeam->totalPlayers >= 50){
        printf("Max players reached for this team\n");
        free(newNode);
        return -1;
    }
    newNode->teamName = NULL;
    printf("\nEnter Player Details:\n");

    //read player id
    while(1){
        printf("Player ID: ");
        readInteger(&playerId);
        if(playerId < 1 || playerId > 1000){
            printf("Player ID must be in the range of 1 to 1000\n");
            continue;
        }
        if(check(playerId,teamId) == 1) break;
        else printf("\nPlayer ID already present in the team\n");
    }

    //read name
    while(1){
        printf("Name: ");
        if(fgets(playerName,MAX_LINE,stdin) == NULL){
            printf("Error in reading the name\n");
            free(newNode);
            return -1;
        }
        else{
            playerName[strcspn(playerName,"\n")] = '\0';
            if(strlen(playerName) == 0 || strlen(playerName) > 50){
                printf("Name cannot be empty and name length should be within 50 characters\n");
                continue;
            }
            if(validName(playerName) != 1){
                printf("Enter a valid name that contains only alphabets and spaces\n");
                continue;
            }
            break;
        }
    }

    //read role
    while(1){
        printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
        readInteger(&Role);
        if(Role >= 1 && Role <= 3) break;
        else printf("\nInvalid option\n");
    }

    //read totalruns
    printf("Total Runs: ");
    readInteger(&totalRuns);

    //read Batting Average
    printf("Batting Average: ");
    readFloat(&battingAverage);

    //read strike Rate
    printf("Strike Rate: ");
    readFloat(&strikeRate);

    //read wickets
    printf("Wickets: ");
    readInteger(&wickets);

    //read economy rate
    printf("Economy Rate: ");
    readFloat(&economyRate);

    newNode->playerId = playerId;
    newNode->name = strdup(playerName);
    newNode->teamName = currTeam->teamName; 
    newNode->economyRate = economyRate;
    newNode->role = roles[Role];
    newNode->battingAverage = battingAverage;
    newNode->strikeRate = strikeRate;
    newNode->totalRuns = totalRuns;
    newNode->wickets = wickets;
    newNode->performanceIndex = calculatePerformanceIndex(*newNode);
    newNode->nextPlayer = NULL;

    insertPlayer(currTeam,newNode);
    printf("\nPlayer added successfully to Team %s!\n",currTeam->teamName);
    return 1;
}

void printSeperator(){
    printf("\n");
    for(int index = 0 ; index < 110 ; index++){
        printf("=");
    }
    printf("\n\n");
} 

void printPlayerHeader(void) {
    printSeperator();
    printf("%-6s %-24s %-12s %8s %7s %7s %6s %6s %12s\n",
        "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printSeperator();
}

void printPlayerRow(const players *p) {
    printf("%-6d %-24s %-12s %8d %7.1f %7.1f %6d %6.1f %12.2f\n",
        p->playerId, p->name, p->role, p->totalRuns,
        p->battingAverage, p->strikeRate, p->wickets,
        p->economyRate, p->performanceIndex);
}

int displaySpecificTeam(int id) {

    teams *currTeam = findTeamById(id);
    if(currTeam == NULL){
        return -1;
    }
    printf("Players of Team %s\n",currTeam->teamName);
    printPlayerHeader();
    for (const players *p = currTeam->playerHead; p; p = p->nextPlayer)
        printPlayerRow(p);
    printSeperator();

    printf("Total players: %d\n",currTeam->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n",currTeam->averageBattingStrikeRate);

    return 1;
}

void printTeamHeader(void) {
    printSeperator();
    printf("%-6s %-24s %16s %20s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printSeperator();
}

void printTeamRow(const teams *team) {
    printf("%-6d %-24s %14.2f %14d\n",
        team->teamId, team->teamName,
        team->averageBattingStrikeRate, team->totalPlayers);
}

void sortTeams(void){
    if(teamHead == NULL){
        printf("List is Empty\n");
        return;
    }

    teams *sorted = NULL;
    teams **tail = &sorted;

    while(teamHead != NULL){
        teams **maxLink = &teamHead;
        for(teams **cur = &(*maxLink)->nextTeam; *cur != NULL; cur = &(*cur)->nextTeam){
            if((*cur)->averageBattingStrikeRate > (*maxLink)->averageBattingStrikeRate){
                maxLink = cur;
            }
        }
        teams *max = *maxLink;
        *maxLink = max->nextTeam;
        max->nextTeam = NULL;
        *tail = max;
        tail = &max->nextTeam;
    }

    teamHead = sorted;
}

void displayTeams(){
    printf("Teams Sorted by Average Batting Strike Rate\n");
    sortTeams();
    teams *currHead = teamHead;
    printTeamHeader();
    while(currHead != NULL){
        printTeamRow(currHead);
        currHead = currHead->nextTeam;
    }
}

players *getRoleHead(teams *team, char *role){
    players *currPlayerHead = team->playerHead;
    while(currPlayerHead != NULL && strcmp(currPlayerHead->role, role) != 0){
        currPlayerHead = currPlayerHead->nextPlayer;
    }
    return currPlayerHead;
}

players *getRoleTail(players *player, char *role, int *length){
    while(player != NULL && strcmp(player->role,role) == 0){
        player = player->nextPlayer;
        (*length) += 1;
    }
    return player;
}

void displayKPlayers(){
    
    printf("Enter Team ID: ");
    int id;
    readInteger(&id);
    teams *team = findTeamById(id);
    if(team == NULL){
        printf("Team does not exists\n");
        return;
    }

    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int role;
    readInteger(&role);
    if(role <= 0 || role > 3){
        printf("Enter a Valid Role\n");
        return;
    }

    printf("Enter number of players: ");
    int numberOfPlayers;
    readInteger(&numberOfPlayers);

    int length = 0;
    players *roleHead = getRoleHead(team, roles[role]);
    getRoleTail(roleHead, roles[role], &length);

    if(numberOfPlayers > length){
        printf("The K value exceeds the team's number of players\n");
        return;
    }

    printf("\nTop %d %s of Team %s:", numberOfPlayers, roles[role], team->teamName);
    printPlayerHeader();

    while(roleHead != NULL && numberOfPlayers-- > 0){
        printPlayerRow(roleHead);
        roleHead = roleHead->nextPlayer;
    }

}

void displayAll(){
    teams *dummy = teamHead;
    while(dummy){
        printf("%s \n",dummy->teamName);
        displaySpecificTeam(dummy->teamId);
        printf("\n");
        dummy = dummy->nextTeam;
    }
}

void printRolePlayerRow(const players *p) {
    printf("%-6d %-24s %-14s %-12s %8d %7.1f %7.1f %6d %6.1f %12.2f\n",
           p->playerId, p->name, p->teamName, p->role, p->totalRuns,
           p->battingAverage, p->strikeRate, p->wickets,
           p->economyRate, p->performanceIndex);
}

void printRoleHeader(const char *roleName) {
    printf("%s of all teams:\n", roleName);
    printSeperator();
    printf("%-6s %-24s %-14s %-12s %8s %7s %7s %6s %6s %12s\n",
           "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printSeperator();
}

void siftDown(players **heap, int heapSize, int startIndex){
    int parentIndex = startIndex;
    while(1){
        int leftChildIndex = 2 * parentIndex + 1;
        int rightChildIndex = leftChildIndex + 1;
        int largestIndex = parentIndex;

        if(leftChildIndex < heapSize &&
           heap[leftChildIndex]->performanceIndex > heap[largestIndex]->performanceIndex){
            largestIndex = leftChildIndex;
        }
        if(rightChildIndex < heapSize &&
           heap[rightChildIndex]->performanceIndex > heap[largestIndex]->performanceIndex){
            largestIndex = rightChildIndex;
        }
        if(largestIndex == parentIndex) return;

        players *swappedPlayer = heap[parentIndex];
        heap[parentIndex] = heap[largestIndex];
        heap[largestIndex] = swappedPlayer;
        parentIndex = largestIndex;
    }
}

void displayAllSpecificRole(){
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int roleChoice;
    readInteger(&roleChoice);

    if(roleChoice < 1 || roleChoice > 3){
        printf("Enter a valid Role\n");
        return;
    }

    const char *selectedRole = roles[roleChoice];

    players **currentPlayerPerTeam = malloc(teamCount * sizeof *currentPlayerPerTeam);
    if(currentPlayerPerTeam == NULL){
        printf("Memory allocation failed\n");
        freeList();
        exit(1);
    }

    int heapSize = 0;
    for(teams *currentTeam = teamHead; currentTeam; currentTeam = currentTeam->nextTeam){
        players *firstRolePlayer = getRoleHead(currentTeam, (char *)selectedRole);
        if(firstRolePlayer != NULL){
            currentPlayerPerTeam[heapSize++] = firstRolePlayer;
        }
    }

    for(int parentIndex = heapSize / 2 - 1; parentIndex >= 0; parentIndex--){
        siftDown(currentPlayerPerTeam, heapSize, parentIndex);
    }

    printRoleHeader(selectedRole);
    while(heapSize > 0){
        players *bestPlayer = currentPlayerPerTeam[0];
        printRolePlayerRow(bestPlayer);

        players *nextInTeam = bestPlayer->nextPlayer;
        if(nextInTeam != NULL && strcmp(nextInTeam->role, selectedRole) == 0){
            currentPlayerPerTeam[0] = nextInTeam;
        }else{
            currentPlayerPerTeam[0] = currentPlayerPerTeam[--heapSize];
        }
        siftDown(currentPlayerPerTeam, heapSize, 0);
    }
    printSeperator();
    free(currentPlayerPerTeam);
}

void printOptions(){
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
}

int main(void)
{
    initialiseTeams();
    initialisePlayers();
    //displayAll();

    bool flag = true;

    while(flag){
        printSeperator();
        printf("ICC ODI Player Performance Analyser\n");
        printSeperator();

        printOptions();
        printSeperator();
        
        printf("Enter your choice: ");
        int choice;
        readInteger(&choice);

        switch(choice){
            case 1: {
                int result = addPlayer();
                if(result == -1){
                    printf("Player addition failed\n");
                }
                break;
            }
            case 2: {
                printf("Enter Team Id: ");
                int id;
                readInteger(&id);
                if(displaySpecificTeam(id) == -1){
                    printf("Team does not exists\n");
                    //flag = false;
                    break;
                }
                break;
            }
            case 3:
                displayTeams();
                break;
            case 4:
                displayKPlayers();
                break;
            case 5:
                displayAllSpecificRole();
                break;
            case 6:
                printf("Exiting...");
                flag = false;
                break;
            default:
                printf("Enter a valid option\n");
                break;
        }

    }
    freeList();
    return 0;
}