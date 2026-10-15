#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

typedef enum ticket{
    Standard = 1,
    Premium = 2,
    Recliner = 3
} ticketCategory;

const char* ticket_names[] = {
    "", 
    "Standard", 
    "Premium", 
    "Recliner"
};

const int standardPrice = 150;
const int premiumPrice = 250;
const int reclinerPrice = 400;

char *getName() {
    int capacity = 16;
    int length = 0;
    int character;

    char *name = malloc(capacity * sizeof(char));

    if (name == NULL) {
        return NULL;
    }

    while ((character = getchar()) != '\n' && character != EOF) {
        
        if (length + 1 >= capacity) {
            capacity *= 2;

            char *temp = realloc(name, capacity * sizeof(char));

            if (temp == NULL) {
                free(name);
                return NULL;
            }

            name = temp;
        }
        if(length == 0 && isspace((unsigned char)character)) continue;
        name[length++] = (char)character;
    }
    name[length] = '\0';
    
    // while(*name == ' '){
    //     *name++;
    // }
    return name;
}
int getInteger(int *value){
    int length = 0;
    int capacity = 20;
    int character;

    long stringConvertedInteger;

    char *end;
    char *number = malloc(capacity * sizeof(char));
    if(number == NULL) return 0;

    //reading all the characters in the input stream
    while((character = getchar()) != '\n' && character != EOF){
        if(length + 1 >= capacity){
            capacity *= 2;
            char *tempNumber = realloc(number, capacity * sizeof(char));
            if(tempNumber == NULL){
                free(number);
                return 0;
            }
            number = tempNumber;
        }
        number[length++] = (char)character;
    }
    number[length] = '\0';



    if(strlen(number) == 0){
        free(number);
        return -1;
    } 

    //validating the string if it only contains digits and signs
    for(int numberIndex = 0; numberIndex < length ; numberIndex++){
        if(!isdigit((unsigned char)number[numberIndex]) && number[numberIndex] != '-' && number[numberIndex] != '+'){
            free(number);
            return -1;
        }
    }
    //converting the string to long and checking the constraints
    stringConvertedInteger = strtol(number,&end,10);
    if(stringConvertedInteger < INT_MIN || *end !='\0' || stringConvertedInteger > INT_MAX){
        free(number);
        return -1;
    }
    *value = (int)stringConvertedInteger;
    free(number);
    return 1;
}
int main(){
    char *name;
    while(1){
        name = getName();
        if(name == NULL){
            printf("Memory allocation failed\n");
            return 1;
        }
        if(strlen(name) == 0){
            printf("name cannot be empty\n");
            free(name);
            continue;
        }
          int valid = 1;

        for (int nameIndex = 0; name[nameIndex] != '\0'; nameIndex++) {
            if (!isalpha((unsigned char)name[nameIndex])) {
                valid = 0;
                break;
            }
        }

        if (!valid) {
            printf("Name must contain alphabets only and name should not contain any spaces\n");
            free(name);
            continue;
        }
            break;
        
    }

    int numberOfTickets;
    while(1){
        int number;
        int option = getInteger(&number);
        if(option == -1){
            //int ch;
            //while((ch = getchar()) != '\n' && ch != EOF);
            printf("Enter a valid Integer\n");
            continue;
        }
        if(option == 0){
            printf("Memory allocation failed\n");
            continue;
        }
        if(number >= 1 && number <= 20){
            numberOfTickets = number;
            break;
        }else{
            printf("Number of tickets should be in the range of 1 to 20(inclusive)\n");
        }
    }

    ticketCategory ticketCategory;

    while(1){
        int category;
        int option = getInteger(&category);
        if(option == -1){
            //int ch;
            //while((ch = getchar()) != '\n' && ch != EOF);
            printf("Enter a valid Integer\n");
            continue;
        }
        if(option == 0){
            printf("Memory allocation failed\n");
            continue;
        }   
        if(category >= 1 && category <= 3){
            switch(category){
                case 1:
                    ticketCategory = Standard;
                    break;
                case 2:
                    ticketCategory = Premium;
                    break;
                case 3:
                    ticketCategory = Recliner;
                    break;
                default:
                    printf("Invalid Option\n");
                    break;
            }
            break;
        }else{
            printf("The options should be\n1 for Standard\n2 for Premium\n3 for Recliner\n");
        }
    }

    float discount = 0;
    int price = 0;
    switch(ticketCategory){
        case Standard:
            price = standardPrice;
            break;
        case Premium:
            price = premiumPrice;
            break;
        case Recliner:
            price = reclinerPrice;
            break;
        default:
            printf("Invalid option\n");
            break;
    }
    float totalTicketPrice = (float)(price * numberOfTickets);
    
    if(numberOfTickets >= 5){
        discount = totalTicketPrice * 0.10;
    }

    float discountedTicketPrice = totalTicketPrice - discount;
    
    printf("Customer: %s\n",name);
    printf("Ticket Category: %s\n",ticket_names[ticketCategory]);
    printf("Tickets: %d\n",numberOfTickets);
    printf("Subtotal: %.2f\n",totalTicketPrice);
    printf("Discount: %.2f\n",discount);
    printf("Final Bill: %.2f\n",discountedTicketPrice);
    
    free(name);
    return 0;
}