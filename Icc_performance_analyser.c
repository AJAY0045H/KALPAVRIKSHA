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
        players *currentPlayer = teamHead->playerHead;
        while (currentPlayer) {
            players *nextPlayer = currentPlayer->nextPlayer;
            free(currentPlayer->name);
            free(currentPlayer);
            currentPlayer = nextPlayer;
        }
        teams *nextTeam = teamHead->nextTeam;
        free(teamHead);
        teamHead = nextTeam;
    }
}

teams *createTeamNode(int teamId, const char *teamName){
    teams *newTeamNode = malloc(sizeof(teams));
    if(newTeamNode == NULL){
        printf("Memory allocation failed\n");
        freeList();
        exit(1);
    }
    //initialising to default values
    newTeamNode->teamId = teamId;
    newTeamNode->totalPlayers = 0;
    newTeamNode->averageBattingStrikeRate = 0.0f;
    newTeamNode->teamName = (char *)teamName;
    newTeamNode->playerHead = NULL;
    newTeamNode->nextTeam = NULL;

    return newTeamNode;
}

int roleRank(const char *role){
    if(strcmp(role, "Batsman") == 0) return 0;
    if(strcmp(role, "Bowler") == 0) return 1;
    return 2;
}

float findAvgBattingStrikeRate(teams *team){
    float strikeRateSum = 0.0f;
    int battingPlayerCount = 0;
    players *currentPlayer = team->playerHead;
    while(currentPlayer != NULL){
        if(strcmp(currentPlayer->role,"Batsman") == 0 || strcmp(currentPlayer->role,"All-rounder") == 0){
            strikeRateSum += currentPlayer->strikeRate;
            battingPlayerCount++;
        }
        currentPlayer = currentPlayer->nextPlayer;
    }
    return battingPlayerCount == 0 ? 0.0f : strikeRateSum / battingPlayerCount;
}

void insertPlayer(teams *playerTeam, players *player){
    int playerRoleRank = roleRank(player->role);
    players **insertionLink = &playerTeam->playerHead;
    while(*insertionLink && roleRank((*insertionLink)->role) < playerRoleRank){
        insertionLink = &(*insertionLink)->nextPlayer;
    }
    while(*insertionLink && roleRank((*insertionLink)->role) == playerRoleRank &&
        (*insertionLink)->performanceIndex >= player->performanceIndex){
        insertionLink = &(*insertionLink)->nextPlayer;
    }
    player->nextPlayer = *insertionLink;
    *insertionLink = player;
    playerTeam->totalPlayers++;
    playerTeam->averageBattingStrikeRate = findAvgBattingStrikeRate(playerTeam);
}

void initialiseTeams(){
    teams *teamTail = NULL;
    for(size_t teamIndex = 0; teamIndex < (size_t)teamCount; teamIndex++){
        teams *newTeam = createTeamNode((int)teamIndex + 1, teamNames[teamIndex]);
        if(teamHead == NULL){
            teamHead = newTeam;
        }else{
            teamTail->nextTeam = newTeam;
        }
        teamTail = newTeam;
    }
}

teams *findTeamByName(char *name){
    teams *teamCursor = teamHead;
    while(teamCursor){
        if(strcmp(teamCursor->teamName,name) == 0){
            return teamCursor;
        }
        teamCursor = teamCursor->nextTeam;
    }
    return NULL;
}

float calculatePerformanceIndex(players player){
    float performanceIndex = 0.0f;
    if(strcmp(player.role,"Batsman") == 0){
        performanceIndex = (player.battingAverage * player.strikeRate) / 100;
    }else if(strcmp(player.role,"Bowler") == 0){
        performanceIndex = (player.wickets * 2) + (100 - player.economyRate);
    }else{
        performanceIndex = ((player.battingAverage * player.strikeRate) / 100) + (player.wickets * 2);
    }
    return performanceIndex;
}

players *createPlayerNode(const Player *playerRecord){
    players *newPlayerNode = malloc(sizeof(players));
    if(newPlayerNode == NULL){
        printf("Memory allocation failed\n");
        freeList();
        exit(1);
    }
    size_t nameLength = strlen(playerRecord->name);
    newPlayerNode->playerId = playerRecord->id;
    newPlayerNode->teamName = (char *)playerRecord->team;
    newPlayerNode->name = malloc(nameLength + 1);
    if(newPlayerNode->name == NULL){
        printf("Memory allocation failed\n");
        free(newPlayerNode);
        freeList();
        exit(1);
    }
    strcpy(newPlayerNode->name, playerRecord->name);
    newPlayerNode->battingAverage = playerRecord->battingAverage;
    newPlayerNode->economyRate = playerRecord->economyRate;
    newPlayerNode->nextPlayer = NULL;
    newPlayerNode->role = (char *)playerRecord->role;
    newPlayerNode->totalRuns = playerRecord->totalRuns;
    newPlayerNode->strikeRate = playerRecord->strikeRate;
    newPlayerNode->wickets = playerRecord->wickets;
    newPlayerNode->performanceIndex = calculatePerformanceIndex(*newPlayerNode);

    return newPlayerNode;
}

void initialisePlayers(){
    for(size_t playerIndex = 0; playerIndex < (size_t)playerCount; playerIndex++){
        players *newPlayerNode = createPlayerNode(&playerData[playerIndex]);
        teams *playerTeam = findTeamByName(newPlayerNode->teamName);
        if(playerTeam == NULL){
            printf("Team not found\n");
            free(newPlayerNode->name);
            free(newPlayerNode);
            continue;
        }
        insertPlayer(playerTeam, newPlayerNode);
    }
}

teams *findTeamById(int teamId) {
    for (teams *team = teamHead; team; team = team->nextTeam)
        if (team->teamId == teamId) return team;
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
    int character;
    while ((character = getchar()) != '\n' && character != EOF);
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

int validName(char *playerName){
    if(strlen(playerName) > 50) return 0;
    for(int characterIndex = 0 ; playerName[characterIndex] != '\0' ;characterIndex++){
        if(!isspace((unsigned char)playerName[characterIndex]) &&
           !isalpha((unsigned char)playerName[characterIndex]) &&
           (unsigned char)playerName[characterIndex] != '-') return 0;
    }
    return 1;
}

int addPlayer(){
    int teamId, playerId, roleChoice, totalRuns, wickets;
    float battingAverage, strikeRate, economyRate;
    char playerName[MAX_LINE];
    
    players *newPlayerNode = malloc(sizeof(players));
    if(newPlayerNode == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }

    //read team id
    teams *selectedTeam = NULL;
    while(1){
        printf("Enter Team ID to add player: ");
        readInteger(&teamId);
        selectedTeam = findTeamById(teamId);
        if(selectedTeam != NULL) break;
        else printf("Team Id does not exists.\n");
    }
    if(selectedTeam->totalPlayers >= 50){
        printf("Max players reached for this team\n");
        free(newPlayerNode);
        return -1;
    }
    newPlayerNode->teamName = NULL;
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
            free(newPlayerNode);
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
        readInteger(&roleChoice);
        if(roleChoice >= 1 && roleChoice <= 3) break;
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

    newPlayerNode->playerId = playerId;
    newPlayerNode->name = strdup(playerName);
    newPlayerNode->teamName = selectedTeam->teamName;
    newPlayerNode->economyRate = economyRate;
    newPlayerNode->role = roles[roleChoice];
    newPlayerNode->battingAverage = battingAverage;
    newPlayerNode->strikeRate = strikeRate;
    newPlayerNode->totalRuns = totalRuns;
    newPlayerNode->wickets = wickets;
    newPlayerNode->performanceIndex = calculatePerformanceIndex(*newPlayerNode);
    newPlayerNode->nextPlayer = NULL;

    insertPlayer(selectedTeam,newPlayerNode);
    printf("\nPlayer added successfully to Team %s!\n",selectedTeam->teamName);
    return 1;
}

void printSeperator(){
    printf("\n");
    for(int separatorIndex = 0 ; separatorIndex < 110 ; separatorIndex++){
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

void printPlayerRow(const players *player) {
    printf("%-6d %-24s %-12s %8d %7.1f %7.1f %6d %6.1f %12.2f\n",
        player->playerId, player->name, player->role, player->totalRuns,
        player->battingAverage, player->strikeRate, player->wickets,
        player->economyRate, player->performanceIndex);
}

int displaySpecificTeam(int teamId) {

    teams *selectedTeam = findTeamById(teamId);
    if(selectedTeam == NULL){
        return -1;
    }
    printf("Players of Team %s\n",selectedTeam->teamName);
    printPlayerHeader();
    for (const players *player = selectedTeam->playerHead; player; player = player->nextPlayer)
        printPlayerRow(player);
    printSeperator();

    printf("Total players: %d\n",selectedTeam->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n",selectedTeam->averageBattingStrikeRate);

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

    teams *sortedTeamHead = NULL;
    teams **sortedTeamTail = &sortedTeamHead;

    while(teamHead != NULL){
        teams **highestTeamLink = &teamHead;
        for(teams **teamLink = &(*highestTeamLink)->nextTeam;
            *teamLink != NULL;
            teamLink = &(*teamLink)->nextTeam){
            if((*teamLink)->averageBattingStrikeRate >
               (*highestTeamLink)->averageBattingStrikeRate){
                highestTeamLink = teamLink;
            }
        }
        teams *highestTeam = *highestTeamLink;
        *highestTeamLink = highestTeam->nextTeam;
        highestTeam->nextTeam = NULL;
        *sortedTeamTail = highestTeam;
        sortedTeamTail = &highestTeam->nextTeam;
    }

    teamHead = sortedTeamHead;
}

void displayTeams(){
    printf("Teams Sorted by Average Batting Strike Rate\n");
    sortTeams();
    teams *teamCursor = teamHead;
    printTeamHeader();
    while(teamCursor != NULL){
        printTeamRow(teamCursor);
        teamCursor = teamCursor->nextTeam;
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
    int teamId;
    readInteger(&teamId);
    teams *team = findTeamById(teamId);
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
    teams *teamCursor = teamHead;
    while(teamCursor){
        printf("%s \n",teamCursor->teamName);
        displaySpecificTeam(teamCursor->teamId);
        printf("\n");
        teamCursor = teamCursor->nextTeam;
    }
}

void printRolePlayerRow(const players *player) {
    printf("%-6d %-24s %-14s %-12s %8d %7.1f %7.1f %6d %6.1f %12.2f\n",
           player->playerId, player->name, player->teamName, player->role, player->totalRuns,
           player->battingAverage, player->strikeRate, player->wickets,
           player->economyRate, player->performanceIndex);
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

    bool keepRunning = true;

    while(keepRunning){
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
                int additionResult = addPlayer();
                if(additionResult == -1){
                    printf("Player addition failed\n");
                }
                break;
            }
            case 2: {
                printf("Enter Team Id: ");
                int teamId;
                readInteger(&teamId);
                if(displaySpecificTeam(teamId) == -1){
                    printf("Team does not exists\n");
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
                keepRunning = false;
                break;
            default:
                printf("Enter a valid option\n");
                break;
        }

    }
    freeList();
    return 0;
}