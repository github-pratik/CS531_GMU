//Name - Pratik Patil
//Student ID - G01523747

#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include <limits.h>


#define max_strings 10 // Maximum number of strings
//#define min_length 1
#define max_length 26 // Maximum length of each string (including null terminator)

//Global Array to hold Strings
char strings[max_strings][max_length];


void getstrings(); // Function to get strings from user
void printstrings(char SortOrder); // Function to print strings
int checkvalidstring(char *str); // Function to check if string is valid
int checkuniqueinput(char *str); // Function to check if string is unique
char usersortorder(); // Function to get sort order from user
void sortstrings(char SortOrder); // Function to sort strings
void swap (char *str1, char *str2); // Function to swap strings



int main(void){
    getstrings();
    
    char SortOrder = usersortorder();

    sortstrings(SortOrder);
    printstrings(SortOrder);
    return 0;
}  

void getstrings(){
    char input[max_length];// Buffer to hold user input
    int i = 0;

    // Loop to get up to max_strings strings
    while (i < max_strings) {
        printf("Enter string without +,*,#,$,%,^,(,) and it should be unique\n");
        printf("String %d (1-25 char)\n: ", i + 1);


       if(fgets(input, max_length, stdin)) {
             // Skip to the next iteration
            if(!strchr(input, '\n')) {
                printf("Error: String length must be between 1 and 25 characters.\n");
                // Clear the input buffer
                while(getchar() != '\n'); // Discard the rest of the input
                continue; // Prompt user again
        }
        input[strcspn(input, "\n")] = 0; // Remove newline character

        // Check if string length is greater than 25
        if (strlen(input) > 25) {
            printf("String is too long. Please enter a string with 1-25 characters.\n");
            // No increment of 'i', so the user will be prompted to re-enter the same string number
            continue;  
        }

        // Check if the string is valid
        if (!checkvalidstring(input)) {
            printf("Invalid string. Please try again.\n");
            // No increment of 'i', so the user will be prompted to re-enter the same string number
            continue;
        }

        // Check if the string is unique
        if (checkuniqueinput(input) != 1) {
            printf("String already exists. Please enter a unique string.\n");
            // No increment of 'i', so the user will be prompted to re-enter the same string number
            continue;
        }

        // If all checks pass, store the string and increment the index
        strcpy(strings[i], input);
        i++; // Move to the next string after successful input
    }
    }
}

int checkvalidstring(char *str) {

    int len = strlen(str);
    if (len < 1 || len > 25) {
        return 0; // Invalid length
    }

    for (int i = 0; i < len; i++) {
        if (strchr("+#$%^&*()", str[i]) != NULL) {
            return 0; // Invalid character found
        }
    }
    return 1; // Valid string
}

int checkuniqueinput(char *str) {
    for (int i = 0; i < max_strings; i++) {
        if (strcmp(strings[i], str) == 0) {
            return 0; // String is not unique
        }
    }
    return 1; // String is unique
}

char usersortorder() { // Function to get sort order from user
    char SortOrder;
    char input[3]; // Buffer to hold user input, allowing for newline and null terminator
    
    do {
        printf("Enter 'A' for ascending or 'D' for descending order: ");
        if(fgets(input, sizeof(input), stdin)){
            if(!strchr(input, '\n')) {
                printf("Error: Input is too long. Please enter only 'A' or 'D'.\n");
                // Clear the buffer if input exceeds the buffer size
                while (getchar() != '\n');
                continue; // Prompt user again
            }
        }

        // Remove trailing newline if present
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        // Check if the input is exactly one character and it is 'A' or 'D'
        if (strlen(input) != 1 || (input[0] != 'A' && input[0] != 'D')) {
            printf("Error: Invalid input. Please enter only a single 'A' or 'D'.\n");
        }
    } while (strlen(input) != 1 || (input[0] != 'A' && input[0] != 'D'));

    return SortOrder;
}

void sortstrings(char SortOrder) { // Function to sort strings
    for (int i = 0; i < max_strings - 1; i++) {  // Bubble sort
        for (int j = 0; j < max_strings - i - 1; j++) {  // Last i elements are already sorted
            int check = strcmp(strings[j], strings[j + 1]);  // Compare strings
            if ((SortOrder == 'A' && check > 0) || (SortOrder == 'D' && check < 0)) {
                swap(strings[j], strings[j + 1]); //   Swap strings
            }
        }
    }
}

void swap(char *str1, char *str2) {
    char temp[max_length];  // Temporary variable to hold string
    strcpy(temp, str1); 
    strcpy(str1, str2);
    strcpy(str2, temp);
}

void printstrings(char SortOrder) {  // Function to print strings
    printf("-----------------------\n"); 
    if ( SortOrder == 'A') {
        printf("Strings Sorted in Ascending Order:\n");
        for (int i = 0; i < max_strings; i++) {  // Print strings in ascending order
            if (strlen(strings[i]) > 0) {
                printf("%s\n", strings[i]);
            }
        }
    } else {
        printf("Strings Sorted in Descending Order:\n");
        for (int i = max_strings - 1; i >= 0; i--) {  // Print strings in descending order
            if (strlen(strings[i]) > 0) {
                printf("%s\n", strings[i]);
            }
        }
    }
    printf("String with *lowest* ascii value: %s\n", strings[0]);  // Print string with lowest ascii value
    printf("String with *highest* ascii value: %s\n", strings[max_strings - 1]);  // Print string with highest ascii value
}