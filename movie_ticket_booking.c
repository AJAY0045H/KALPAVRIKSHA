#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define SIZE 100
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

int readLine(char *buf) {
    if (fgets(buf, SIZE, stdin) == NULL) exit(1);
    size_t len = strcspn(buf, "\n");
    if (buf[len] == '\0') {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return 0;
    }
    buf[len] = '\0';
    return 1;
}
int isValidName(const char *s) {
    if (*s == '\0') return 0;
    for (; *s; s++) {
        if (!isalpha((unsigned char)*s)) return 0;
    }
    return 1;
}
int readInt(int min, int max, const char *rangeMessage) {
    char buf[SIZE], *end;
    while (1) {
        if (!readLine(buf)) {
            printf("Enter a valid integer\n");
            continue;
        }
        long value = strtol(buf, &end, 10);
        if (end == buf || *end != '\0') {
            printf("Enter a valid integer\n");
        } else if (value < min || value > max) {
            printf("%s\n", rangeMessage);
        } else {
            return (int)value;
        }
    }
}
int main(){
    char name[SIZE];
    while(1){
      if (readLine(name) && isValidName(name)) break;
        printf("Name must contain alphabets only, with no spaces and not empty\n");
    }

    int numberOfTickets;
    int tickets = readInt(1, 20, "Number of tickets should be between 1 and 20 (inclusive)");
    int category = readInt(1, 3, "Ticket category must be\n1 (Standard)\n2 (Premium)\n3 (Recliner)");

    ticketCategory ticketCategory;

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
     double subtotal = price * tickets;
     double discount;

    if (tickets >= 5) {
        discount = subtotal * 0.10;
    } else {
        discount = 0;
    }

    double finalBill = subtotal - discount;

    printf("Customer: %s\n", name);
    printf("Ticket Category: %s\n", ticket_names[ticketCategory]);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalBill);
    return 0;
}