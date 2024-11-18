

#include<stdio.h>
#include<string.h>
#include<ctype.h>

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
    char input[max_length];
    int i = 0;
    while(i < max_strings){
        printf("Enter string without +,*,#,$,%,^,(,) and it should be unique\n"); 
        printf("String %d (1-25 char)\n: ", i + 1);
        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = 0; // Remove newline character

        if (!checkvalidstring(input)) {
            printf("Invalid string. Please try again.\n");
            continue;
        }

        if (!checkuniqueinput(input)) {
            printf("String already exists. Please enter a unique string.\n");
            continue;
        }

        strcpy(strings[i], input);
        i++;
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

char usersortorder() {
    char SortOrder;
    do{
        printf("Enter 'A' for ascending or 'D' for descending order: ");
        SortOrder = getchar();
        while(getchar() != '\n'); 
        SortOrder = toupper(SortOrder);
        if (SortOrder != 'A' && SortOrder != 'D') {
            printf("Invalid input. Please enter 'A' or 'D'.\n");
        }

    } while(SortOrder != 'A' && SortOrder != 'D');
    return SortOrder;
}

void sortstrings(char SortOrder) {
    for (int i = 0; i < max_strings - 1; i++) {
        for (int j = 0; j < max_strings - i - 1; j++) {
            int check = strcmp(strings[j], strings[j + 1]);
            if ((SortOrder == 'A' && check > 0) || (SortOrder == 'D' && check < 0)) {
                swap(strings[j], strings[j + 1]);
            }
        }
    }
}

void swap(char *str1, char *str2) {
    char temp[max_length];
    strcpy(temp, str1);
    strcpy(str1, str2);
    strcpy(str2, temp);
}

void printstrings(char SortOrder) {
    printf("-----------------------\n");
    printf("Strings Sorted %c:\n", SortOrder);
    if(SortOrder == 'A'){
        for (int i = 0; i < max_strings; i++) {
            if (strlen(strings[i]) > 0) {
                printf("%s\n", strings[i]);
            }
        }
    } else {
        for (int i = max_strings - 1; i >= 0; i--) {
            if (strlen(strings[i]) > 0) {
                printf("%s\n", strings[i]);
            }
        }
    }
    printf("String with *lowest* ascii value: %s\n", strings[0]);
    printf("String with *highest* ascii value: %s\n", strings[max_strings - 1]);
}
