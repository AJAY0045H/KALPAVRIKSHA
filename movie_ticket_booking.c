#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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

const int ticket_price[] = {
    0,
    150,
    250,
    400
};

char *getName() {
    int capacity = 16;
    int length = 0;
    int character;

    char *name = malloc(capacity * sizeof(char));

    if (name == NULL) {
        return NULL;
    }

    while ((character = getchar()) != ' ' && character != '\n' && character != EOF) {
        
        if (length + 1 >= capacity) {
            capacity *= 2;

            char *temp = realloc(name, capacity * sizeof(char));

            if (temp == NULL) {
                free(name);
                return NULL;
            }

            name = temp;
        }

        name[length++] = (char)character;
    }

    name[length] = '\0';

    return name;
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

        for (int i = 0; name[i] != '\0'; i++) {
            if (!isalpha((unsigned char)name[i])) {
                valid = 0;
                break;
            }
        }

        if (!valid) {
            printf("Name must contain alphabets only\n");
            free(name);
            continue;
        }
            break;
        
    }

    int numberOfTickets;
    while(1){
        int number = getInteger();
        if(scanf("%d",&number) != 1){
            int ch;
            while((ch = getchar()) != '\n' && ch != EOF);
            printf("Enter a valid Integer\n");
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
        if(scanf("%d",&category) != 1){
            int ch;
            while((ch = getchar()) != '\n' && ch != EOF);
            printf("Enter a valid Integer\n");
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
    float totalTicketPrice = (ticket_price[ticketCategory] * numberOfTickets);
    
    if(numberOfTickets >= 5){
        discount = totalTicketPrice * 0.10f;
    }

    float discountedTicketPrice = totalTicketPrice - discount;
    
    printf("Customer: %s\n",name);
    printf("Ticket Category: %s\n",ticket_names[ticketCategory]);
    printf("Tickets: %d\n",numberOfTickets);
    printf("Subtotal: %.2f\n",totalTicketPrice);
    printf("Discount: %.2f\n",discount);
    printf("Final Bill: %.2f",discountedTicketPrice);
    
    free(name);
    return 0;
}