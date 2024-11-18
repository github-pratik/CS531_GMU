#include <stdio.h>   // Standard input-output library
#include <stdlib.h>  // Standard library for memory allocation, process control, etc.
#include <string.h>  // String handling functions
#include <ctype.h>   // Character handling functions

// Structure to represent an IP address with an alias
struct address_t {
    int octet[4];           // Array to store the four octets of the IP address
    char alias[11];         // Alias for the IP address, maximum length of 10 characters
    struct address_t *next; // Pointer to the next address in the linked list
};

// Head pointer for the linked list of addresses
struct address_t *head = NULL;

// Function prototypes
void add_address();       // Function to add a new address to the list
void look_up_address();   // Function to look up an address by alias
void update_address();    // Function to update an existing address
void delete_address();    // Function to delete an address from the list
void display_list();      // Function to display all addresses in the list
void display_aliases();   // Function to display all aliases in the list
void save_to_file();      // Function to save the list of addresses to a file
void load_from_file();    // Function to load the list of addresses from a file

int main() {
    int choice;
    char input[20];
    // Load addresses from file at the start
    load_from_file();

    // Main menu loop
    do{
        printf("\nMenu:\n");
        printf("1. Add Address\n");
        printf("2. Look Up Address\n");
        printf("3. Update Address\n");
        printf("4. Delete Address\n");
        printf("5. Display List\n");
        printf("6. Display Aliases\n");
        printf("7. Save to File\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");

        //fgets(input, sizeof(input), stdin);
        scanf("%d", &choice);
        
        // Execute the corresponding function based on user choice
        switch (choice) {
            case 1:
                add_address();
                break;
            case 2:
                look_up_address();
                break;
            case 3:
                update_address();
                break;
            case 4:
                delete_address();
                break;
            case 5:
                display_list();
                break;
            case 6:
                display_aliases();
                break;
            case 7:
                save_to_file();
                break;
            case 8:
                // Save addresses to file before exiting
                printf("Exit\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }while(choice != 8);
    return 0;
}

// Function to add a new address to the list
void add_address() {
    char input[100]; // Buffer for the IP input
    char alias[12];  // Extra space to detect overflow (10 + 1 for newline + 1 for overflow check)
    int octets[4], valid_ip = 0;
    while ( getchar() != '\n' );
    while (1) {
        printf("Enter alias (max 10 characters, no spaces): ");
        if (fgets(alias, sizeof(alias), stdin) == NULL) {
            printf("Input error. Please try again.\n");
            continue;
        }

        // Check if input was too long
        if (alias[strlen(alias) - 1] != '\n') {
            while ( getchar() != '\n' );// Clear extra characters from the input buffer
            printf("Invalid alias name. Must be 10 characters or fewer.\n");
            continue; // Reprompt for alias
        }

        alias[strcspn(alias, "\n")] = 0; // Remove newline

        // Check length and spaces
        if (strlen(alias) > 10) {
            printf("Invalid alias name. Must be 10 characters or fewer.\n");
            continue; // Reprompt for alias
        }

        if (strchr(alias, ' ') != NULL) {
            printf("Alias cannot contain spaces. Try again.\n");
            continue; // Reprompt for alias
        }

        // Check for alias uniqueness
        struct address_t *current = head;
        int is_unique = 1;
        while (current != NULL) {
            if (strcasecmp(current->alias, alias) == 0) {
                printf("Alias already exists. Returning to menu.\n");
                return;
            }
            current = current->next;
        }
        
        // If all checks pass, break the loop and proceed to IP address
        break;
    }

    while (!valid_ip) {
        printf("Enter IPv4 address (format: x.x.x.x): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Input error. Please try again.\n");
            continue;
        }
        
        // Check if the input was too long
        if (input[strlen(input) - 1] != '\n') {
             while ( getchar() != '\n' ) // Clear extra characters from the input buffer
            printf("Invalid IP format. Must contain exactly 3 dots. Try again.\n");
            continue; // Reprompt for IP
        }

        input[strcspn(input, "\n")] = 0; // Remove newline
        int dot_count = 0;
        for (int i = 0; input[i]; i++) {
            if (input[i] == '.') dot_count++;
        }
        if (dot_count != 3) {
            printf("Invalid IP format. Try again.\n");
            continue;
        }
        
        if (sscanf(input, "%d.%d.%d.%d", &octets[0], &octets[1], &octets[2], &octets[3]) != 4) {
            printf("Invalid IP format. Try again.\n");
            continue;
        }

        valid_ip = 1;
        for (int i = 0; i < 4; i++) {
            if (octets[i] < 0 || octets[i] > 255) {
                printf("Invalid IP components. Must be between 0-255. Try again.\n");
                valid_ip = 0;
                break;
            }
        }
    }

    struct address_t *new_node = (struct address_t *)malloc(sizeof(struct address_t));
    memcpy(new_node->octet, octets, sizeof(octets));
    strcpy(new_node->alias, alias);
    new_node->next = head;
    head = new_node;
    printf("Address added successfully.\n");
}

void look_up_address() {
    char alias[50];
    int found = 0;
    while(getchar() != '\n');
    printf("Enter alias to look up: ");
    fgets(alias, sizeof(alias), stdin);
    if (alias[strlen(alias) - 1] != '\n') {
            while ( getchar() != '\n' );// Clear extra characters from the input buffer
            printf("Invalid alias name\n");
        }
    if(strlen(alias) > 10){
        printf("Invalid alias name\n");
        return;
    }
    alias[strcspn(alias, "\n")] = 0;
    struct address_t *current = head;
    while (current != NULL) {
        if (strcmp(current->alias, alias) == 0) {
            printf("Address: %d.%d.%d.%d\n", current->octet[0], current->octet[1], current->octet[2], current->octet[3]);
            found = 1;
            break;
        }
        current = current->next;
    }
    if (!found){
        printf("Error: Alias '%s' not found in the list.\n", alias);
    }
}

void update_address() {
    char alias[20];
    printf("Enter alias: ");
    scanf("%s", alias);  // Get alias from user
    fflush(stdin);
    //while(getchar() != '\n');  // Clear the input buffer


    struct address_t *current = head;
    while (current != NULL) {
        if (strcmp(current->alias, alias) == 0) {
            int octet[4];
            char input[20];
            int validInput;
            int isDuplicate = 0;

            printf("Update address for %s: %d.%d.%d.%d\n", alias, current->octet[0], current->octet[1], current->octet[2], current->octet[3]);

            // Loop through each octet (1 to 4) and get the value, validating the range and format
            for (int i = 0; i < 4; i++) {
                do {
                    validInput = 1;  // Assume input is valid
                    printf("Enter location value #%d[0-255]: ", i + 1);
                    if (fgets(input, sizeof(input), stdin) == NULL) {
                        printf("Error reading input.\n");
                        return;
                    }

                     if (strchr(input, '.') != NULL) {
                        printf("error: Decimal values are not allowed. Please enter an integer between 0 and 255.\n");
                        validInput = 0;
                        continue;
                    }
                    if (sscanf(input,"%d", &octet[i]) != 1 || octet[i] < 0 || octet[i] > 255 ) {
                        
                        validInput = 0;  // Invalid input detected
                        //fflush(stdin);
                        printf("Error: Invalid entry for value #%d. Please reenter.\n", i + 1);

                    }


                } while (!validInput);  // Repeat until valid input is provided
            }
            struct address_t *check = head;
             while (check != NULL) {
                // If the entered octets match an existing address, mark it as a duplicate
                if (check != current && 
                    check->octet[0] == octet[0] && check->octet[1] == octet[1] &&
                    check->octet[2] == octet[2] && check->octet[3] == octet[3]) {
                    isDuplicate = 1;
                    break;
                }
                check = check->next;
            }

            // If a duplicate address is found, display an error message and return to the menu
            if (isDuplicate) {
                printf("Error: The entered address %d.%d.%d.%d is a duplicate. Returning to the menu.\n", octet[0], octet[1], octet[2], octet[3]);
                return;  // Exit the function without updating the address
            }

            // Update the address with the new valid octet values
            for (int i = 0; i < 4; i++) {
                current->octet[i] = octet[i];
            }

            printf("Updated address for %s: %d.%d.%d.%d\n", alias, current->octet[0], current->octet[1], current->octet[2], current->octet[3]);
            return;
        }
        current = current->next;
    }

    printf("Error: Alias '%s' not found\n", alias);
    
}


void delete_address(){
    char alias[20];
    printf("Enter alias:");
    scanf("%s", alias);

    struct address_t *current = head;
    struct address_t *prev = NULL;
    
    while(current != NULL){
        if (strcmp(current->alias, alias) == 0){
            printf("Delete %s %d.%d.%d.%d?(y/n)", alias, current->octet[0], current->octet[1], current->octet[2], current->octet[3]);
            char choice;
            scanf(" %c", &choice);
            if (choice == 'Y' || choice == 'y'){
                if (prev == NULL){
                head = current->next;
                }else{
                prev->next = current->next;
                }
                free(current);
                printf("%s deleted successfully\n", alias);
            } else if(choice == 'N' || choice == 'n'){
                printf("Address for %s not deleted\n", alias);
            }else{
                 
                printf("Invalid choice. Please select y or n\n");
            }
            
            return;
        }else{
            printf("Alias %s not found! \n", alias);
            display_list(); 
            
            return;
        }

        prev = current;
        current = current->next;
    }
    printf("Error:'%s' not found.\n", alias);
}

void display_list(){
    struct address_t *current = head;
    int count = 0;
    if (current == NULL){
        printf("List is empty\n");
        return;
    }
    while (current != NULL){

        printf("%d.%d.%d.%d %s\n",current->octet[0], current->octet[1], current->octet[2], current->octet[3],current->alias);
        count++;
        current = current->next;
    }
    printf("Total number of addresses: %d\n", count);
}

void display_aliases() {
    int octet[2];
    char input[20];
    int validInput;
    while(getchar() != '\n');
    for (int i = 0; i < 2; i++) {
        do {
            validInput = 1;  // Assume input is valid
            printf("Enter location value #%d[0-255]: ", i + 1);
            if (fgets(input, sizeof(input), stdin) == NULL) {
                printf("Error reading input.\n");
                return;
            }

            if (strchr(input, '.') != NULL) {
                printf("error: Decimal values are not allowed. Please enter an integer between 0 and 255.\n");
                validInput = 0;
                continue;
            }
            if (sscanf(input,"%d", &octet[i]) != 1|| octet[i] < 0 || octet[i] > 255 ) {
                validInput = 0;  // Invalid input detected 
                printf("Error: Invalid entry for value #%d. Please reenter.\n", i + 1);    
            } 
        } while (!validInput); 
    }
    struct address_t *current = head;
    int count = 0;

    if (current == NULL) {
        printf("List is empty\n");
        return;
    }

    //printf("Aliases for %d.%d:\n", octet[0], octet[1]);
    while (current != NULL) {
        if (current->octet[0] == octet[0] && current->octet[1] == octet[1]) {
            printf("%d.%d.%d.%d %s\n", current->octet[0], current->octet[1], current->octet[2], current->octet[3], current->alias);
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        printf("No aliases found for %d.%d\n", octet[0], octet[1]);
    }
}

void save_to_file() {
    char filename[256];  // Buffer for file name
    printf("Enter file name to save: ");
    scanf("%255s", filename);  // Get the file name from the user, limiting input to avoid buffer overflow

    FILE *file = fopen(filename, "w");  // Open the user-specified file for writing
    if (file == NULL) {
        printf("Error: Unable to open file '%s'\n", filename);
        return;
    }

    struct address_t *current = head;
    while (current != NULL) {
        fprintf(file, "%d.%d.%d.%d %s\n", current->octet[0], current->octet[1], current->octet[2], current->octet[3], current->alias);
        current = current->next;
    }
    fclose(file);  // Close the file after writing is complete
    printf("Data saved to '%s'\n", filename);
}


void load_from_file() {
    FILE *file = fopen("cs531_inet.txt", "r");
    if (file == NULL) {
        printf("error: File not found\n");
        return;
    }

    while (1) {
        struct address_t *new_address = (struct address_t *)malloc(sizeof(struct address_t));
        if (new_address == NULL) {
            printf("error: Memory allocation failed\n");
            fclose(file);
            return;
        }

        // Use fscanf to read from the file directly into new_address
        int result = fscanf(file, "%d.%d.%d.%d %s\n", &new_address->octet[0], &new_address->octet[1], &new_address->octet[2], &new_address->octet[3], new_address->alias);

        // If fscanf fails to read 5 items (4 octets and 1 alias), exit the loop
        if (result != 5) {
            free(new_address);  // Free the memory since it's not used
            break;
        }

        // Add the new_address to the linked list
        new_address->next = head;
        head = new_address;
    }

    fclose(file);
    printf("Data loaded from file\n");
}
