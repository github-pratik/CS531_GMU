#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_ALIAS_LENGTH 12

typedef struct address_t {
    int octet[4];
    char alias[MAX_ALIAS_LENGTH];
    struct address_t *leftChild, *rightChild, *parent;
    int height, depth;
} address_t;

address_t *head = NULL;

void load_from_file();
void addAddress();
void look_up_address();
void update_address();
void delete_address();
void display_list();
void display_aliases();
void display_error_log();
// Function prototype for inserting into the BST
address_t* insertBST(address_t *node, address_t *new_node, address_t *parent, int depth);

// Utility function to check the validity of the address and alias
int validateData(address_t *addr);

//check the duplicate alias
address_t* findAlias(address_t *node, char *alias);

// Function to check ipv4 addresses
address_t* findIPv4(address_t *node, int octets[4]);
// Function to find the node with the minimum value
address_t* minValueNode(address_t *node);
// Function to delete a node with a given key from the BST
int deleteNode(address_t** root, char* key);
// Function to print the BST in order
void printInOrder(address_t *node, int depth);


int calculateHeight(address_t *node); //function to calculat the height

//Function to display alaias
void display_aliases_by_octets(int octet1, int octet2);

//Recursive function
void print_matching_aliases(address_t *node, int octet1, int octet2, int *found);

//Log_File display function
void log_error(const char *message);

int main() {
    int choice;
    //char input[20];
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
        printf("7. Display Error Log\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");

        //fgets(input, sizeof(input), stdin);
        scanf("%d", &choice);
        
        // Execute the corresponding function based on user choice
        switch (choice) {
            case 1:
                addAddress();
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
                display_error_log();
                break;
            case 8:
                printf("Exit\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }while(choice != 8 );
    return 0;
}

// function to load the file 
void load_from_file() {
    FILE *file = fopen("cs531_inet.txt", "r");
    FILE *error_log = fopen("cs531_error_log.txt", "w");

    if (!file || !error_log) {
        perror("Failed to open file");
        if (file) fclose(file);
        if (error_log) fclose(error_log);
        return;
    }

    char line[256];  // Buffer to hold each line from the file
    while (fgets(line, sizeof(line), file)) {
        int octets[4];
        char alias[MAX_ALIAS_LENGTH + 1];  // +1 for null terminator
        int dot_count = 0;
        for (int i = 0; i < sizeof(line); i++) {
            if (line[i] == '.') {
                dot_count++;
            }
            
        }
        // Parse the line
        if (sscanf(line, "%d.%d.%d.%d %12s", &octets[0], &octets[1], &octets[2], &octets[3], alias) != 5) {
            fprintf(error_log, "%s", line);
            continue;
        }

        // Validate IP octets
        int valid_ip = 1;
        for (int i = 0; i < 4; i++) {
            if (octets[i] < 0 || octets[i] > 255) {
                valid_ip = 0;
                break;
            }
        }

        // Check for uppercase letters in alias
        int has_upper = 0;
        for (int i = 0; alias[i]; i++) {
            if (isupper(alias[i])) {
                has_upper = 1;
                break;
            }
        }

        // Log errors if any checks fail
        if (!valid_ip || has_upper || strlen(alias) > 10 || dot_count != 3) {
            fprintf(error_log, "%s", line);
            continue;
        }

        // All checks passed, inserting into BST 
        address_t *new_node = malloc(sizeof(address_t));
        if (new_node) {
            new_node->octet[0] = octets[0];
            new_node->octet[1] = octets[1];
            new_node->octet[2] = octets[2];
            new_node->octet[3] = octets[3];
            strncpy(new_node->alias, alias, MAX_ALIAS_LENGTH);
            new_node->leftChild = new_node->rightChild = new_node->parent = NULL;
            head = insertBST(head, new_node, NULL, 0);
        } else {
            fprintf(error_log, "Memory allocation failed\n");
        }
    }

    fclose(file);
    fclose(error_log);
}


//log_error simply writes the error to the error_log file
void log_error(const char *message) {
    FILE *error_log = fopen("cs531_error_log.txt", "a");  // Append mode
    if (error_log) {
        fprintf(error_log, "%s\n", message);
        fclose(error_log);
    }
}


//insert function to add node to tree
address_t* insertBST(address_t *node, address_t *new_node, address_t *parent, int depth) {
    if (node == NULL) {
        new_node->parent = parent;
        new_node->depth = depth;
        new_node->height = 0; // Leaf node, height set to 0
        return new_node;
    }

    // Check if the current node's IP address matches the new node's IP address
    if (node->octet[0] == new_node->octet[0] &&
        node->octet[1] == new_node->octet[1] &&
        node->octet[2] == new_node->octet[2] &&
        node->octet[3] == new_node->octet[3]) {
        free(new_node); // Free the new node as it's a duplicate IP address
        return node; // Return the current node to indicate no insertion was made
    }

    if (strcmp(new_node->alias, node->alias) < 0) {
        node->leftChild = insertBST(node->leftChild, new_node, node, depth + 1);
    } else if (strcmp(new_node->alias, node->alias) > 0) {
        node->rightChild = insertBST(node->rightChild, new_node, node, depth + 1);
    } else {
        free(new_node); // Free the new node as it's a duplicate alias, though this check is redundant with the IP check
        return node;
    }

    // Update the height of the current node
    node->height = calculateHeight(node);
    return node;
}



// Implement the validateData function to check octets and alias validity
int validateData(address_t *addr) {
    // Validate each octet is within the range [0, 255]
    for (int i = 0; i < 4; i++) {
        if (addr->octet[i] < 0 || addr->octet[i] > 255) {
            return 0;  // Invalid data
        }
    }

    // Validate alias length and character criteria
    if (strlen(addr->alias) > 10) {
        return 0;  // Alias too long
    }

    for (int i = 0; i < strlen(addr->alias); i++) {
        if (addr->alias[i] >= 'A' && addr->alias[i] <= 'Z') {
            addr->alias[i] = addr->alias[i] + ('a' - 'A');  // Convert uppercase to lowercase
        }
    }
    return 1;  // Data is valid
}

// Function to find an alias in the BST
address_t* findAlias(address_t *node, char *alias) {
    if (node == NULL) {
        return NULL;
    } else if (strcmp(alias, node->alias) == 0) {
        return node;
    } else if (strcmp(alias, node->alias) < 0) {
        return findAlias(node->leftChild, alias);
    } else {
        return findAlias(node->rightChild, alias);
    }
}

// Function to look up an address by alias
address_t* findIPv4(address_t *node, int octets[4]) {
    if (node == NULL) {
        // Base case: no address found in an empty subtree
        return NULL;
    }
    
    // Compare current node's IP address with the input IP address
    if (node->octet[0] == octets[0] &&
        node->octet[1] == octets[1] &&
        node->octet[2] == octets[2] &&
        node->octet[3] == octets[3]) {
        // Matching address found
        return node;
    }
    
    // Decide the direction to move based on the IP address comparison
    // Since IP addresses aren't naturally ordered like numeric or string values,
    // we may need to check both sides of the tree unless we impose some order.
    address_t* result = NULL;
    
    // First check the left subtree
    result = findIPv4(node->leftChild, octets);
    if (result != NULL) {
        return result;
    }
    
    // Then check the right subtree
    result = findIPv4(node->rightChild, octets);
    return result;
}





//address funciton implementations
void addAddress() {
    address_t *new_address = (address_t *)malloc(sizeof(address_t));
    if (new_address == NULL) {
        printf("Error: Memory allocation failed\n");
        return;
    }

    char ipInput[30];
    char aliasInput[30];
    int octets[4];
    while (getchar() != '\n');  // Clear the input buffer

    printf("Enter IPv4 address (format: x.x.x.x): ");
    fgets(ipInput, sizeof(ipInput), stdin);
    ipInput[strcspn(ipInput, "\n")] = 0;  // Remove newline character

    if (sscanf(ipInput, "%d.%d.%d.%d", &octets[0], &octets[1], &octets[2], &octets[3]) != 4) {
        printf("Invalid IPv4 address format.\n");
        free(new_address);
        return;
    }

    for (int i = 0; i < 4; i++) {
        if (octets[i] < 0 || octets[i] > 255) {
            printf("Error: IP octet #%d is out of the allowed range (0-255).\n", i + 1);
            free(new_address);
            return;
        }
    }

    if (findIPv4(head, octets) != NULL) {
        printf("Error: IPv4 address %d.%d.%d.%d is a duplicate and cannot be added.\n", octets[0], octets[1], octets[2], octets[3]);
        free(new_address);
        return;
    }

    printf("Enter alias (up to 10 characters, no spaces): ");
    fgets(aliasInput, sizeof(aliasInput), stdin);
    aliasInput[strcspn(aliasInput, "\n")] = 0;  // Remove newline character

    if (strlen(aliasInput) > 10) {
        printf("Error: Alias length exceeds 10 characters.\n");
        free(new_address);
        return;
    }

    for (int i = 0; aliasInput[i]; i++) {
        if (isspace(aliasInput[i])) {
            printf("Error: Alias must not contain spaces.\n");
            free(new_address);
            return;
        }
        aliasInput[i] = tolower(aliasInput[i]);  // Convert alias to lowercase
    }

    if (findAlias(head, aliasInput) != NULL) {
        printf("Error: Alias '%s' is a duplicate and cannot be added.\n", aliasInput);
        free(new_address);
        return;
    }

    new_address->octet[0] = octets[0];
    new_address->octet[1] = octets[1];
    new_address->octet[2] = octets[2];
    new_address->octet[3] = octets[3];
    strncpy(new_address->alias, aliasInput, MAX_ALIAS_LENGTH);
    new_address->alias[MAX_ALIAS_LENGTH - 1] = '\0';

    head = insertBST(head, new_address, NULL, 0);  // Start insertion at the root with depth 0
    printf("Address added successfully.\n");
}

//function to look_up_address from BST
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

    address_t *result = findAlias(head, alias);
    if (result != NULL) {
        printf("Address: %d.%d.%d.%d\n", result->octet[0], result->octet[1], result->octet[2], result->octet[3]);
        found = 1;
    }
    if (!found){
        printf("Error: Alias '%s' not found in the list.\n", alias);
    }
}


//Update address function
void update_address() {
    while (getchar() != '\n');
    char alias[MAX_ALIAS_LENGTH];
    char inputBuffer[256];

    printf("Enter alias Name: ");
    fgets(alias, sizeof(alias), stdin);
    alias[strcspn(alias, "\n")] = 0;  

    //finding alias in BST
    address_t *current = findAlias(head, alias);  
    if (current == NULL) {
        printf("Error: Alias '%s' not found\n", alias);
        return;
    }

    printf("Update address for %s: %d.%d.%d.%d\n", alias, current->octet[0], current->octet[1], current->octet[2], current->octet[3]);
    int octet[4], validInput, isDuplicate;

    // Looping through each octet (0 to 3) and enter the value, validating the range and format
    for (int i = 0; i < 4; i++) {
        do {
            printf("Enter octet #%d [0-255]: ", i + 1);
            if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) {
                printf("Error reading input.\n");
                continue;
            }
            inputBuffer[strcspn(inputBuffer, "\n")] = 0; 
            if (strchr(inputBuffer, '.') != NULL) {
                        printf("error: Decimal values are not allowed. Please enter an integer between 0 and 255.\n");
                        validInput = 0;
                        continue;
                    }
            validInput = sscanf(inputBuffer, "%d", &octet[i]);
            if (validInput != 1 || octet[i] < 0 || octet[i] > 255) {
                printf("Error: Invalid entry for octet #%d. Please reenter.\n", i + 1);
                validInput = 0;  // Force re-entry
            }
        } while (!validInput);
    }

    // Check for duplicate IP address excluding the current address
     if (findIPv4(head, octet) != NULL) {
        printf("Error: IPv4 address is a duplicate and cannot be updated.\n");
        return;
    }

    // Update the address with the new valid octet values
    for (int i = 0; i < 4; i++) {
        current->octet[i] = octet[i];
    }

    printf("Updated address for %s: %d.%d.%d.%d\n", alias, current->octet[0], current->octet[1], current->octet[2], current->octet[3]);
}

void delete_address() {
    char alias[MAX_ALIAS_LENGTH];
    while (getchar() != '\n'); 
    printf("Enter the alias Name: ");
    fgets(alias, sizeof(alias), stdin);
    alias[strcspn(alias, "\n")] = 0; 

    // Function to delete a node and handle the result
    if (deleteNode(&head, alias)) {
        printf("Address with alias '%s' successfully deleted.\n", alias);
    } else {
        printf("Error: Alias '%s' not found.\n", alias);
    }
}
//Inorder DeleteNode
// Recursive function to delete a node with a given key from the BST
int deleteNode(address_t** root, char* key) {
    // Base case
    if (*root == NULL) return 0;

    // Recursive calls for following of node to be deleted
    if (strcmp(key, (*root)->alias) < 0) {
        return deleteNode(&((*root)->leftChild), key);
    } else if (strcmp(key, (*root)->alias) > 0) {
        return deleteNode(&((*root)->rightChild), key);
    } else {
        // Node with only one child or no child
        if ((*root)->leftChild == NULL) {
            address_t* temp = (*root)->rightChild;
            free(*root);
            *root = temp;  // Connect parent to child
            return 1;
        } else if ((*root)->rightChild == NULL) {
            address_t* temp = (*root)->leftChild;
            free(*root);
            *root = temp;  // Connect parent to child
            return 1;
        }

        // Node with two children, get the inorder successor (smallest in the right subtree)
        address_t* temp = minValueNode((*root)->rightChild);

        // Copy the inorder successor's content to this node
        strcpy((*root)->alias, temp->alias);
        memcpy((*root)->octet, temp->octet, 4 * sizeof(int));

        // Delete the inorder successor
        return deleteNode(&((*root)->rightChild), temp->alias);
    }
}

//used in delete operation for two children case
address_t* minValueNode(address_t* node) {
    address_t* current = node;
    // Loop down to find the leftmost leaf
    while (current && current->leftChild != NULL)
        current = current->leftChild;
    return current;
}


void display_list() {
    printf("\nList of Addresses:\n");
    printf("%-10s %-16s %-6s %-5s %-14s\n", "Alias", "IP Address", "Height", "Depth", "Parent");
    printInOrder(head,0);
}

// Utility function to calculate the height of a node
int calculateHeight(address_t *node) {
    if (node == NULL) return -1;  // Height of non-existent node is -1
    int leftHeight = calculateHeight(node->leftChild);
    int rightHeight = calculateHeight(node->rightChild);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

// Function to print the BST in-order with depth calculation
void printInOrder(address_t *node, int depth) {
    if (node == NULL) return;

    printInOrder(node->leftChild, depth + 1);

    printf("%-10s %3d.%3d.%3d.%3d   %-6d %-5d %-10s\n",
           node->alias,
           node->octet[0], node->octet[1], node->octet[2], node->octet[3],
           node->height, depth,
           node->parent ? node->parent->alias : "NONE");

    printInOrder(node->rightChild, depth + 1);
}

//Display all the aliases in BST
void display_aliases(){
    char inputBuffer[256];
    int octet[2];
    int validInput =1;
    while (getchar() != '\n'); 
    for (int i = 0; i < 2; i++) {
        do {
            printf("Enter octet #%d [0-255]: ", i + 1);
            if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) {
                printf("Error reading input.\n");
                continue;
            }
            inputBuffer[strcspn(inputBuffer, "\n")] = 0; 
            if (strchr(inputBuffer, '.') != NULL) {
                        printf("error: Decimal values are not allowed. Please enter an integer between 0 and 255.\n");
                        validInput = 0;
                        continue;
                    }
            validInput = sscanf(inputBuffer, "%d", &octet[i]);
            if (validInput != 1 || octet[i] < 0 || octet[i] > 255) {
                printf("Error: Invalid entry for octet #%d. Please reenter.\n", i + 1);
                validInput = 0;  // Force re-entry
            }
        } while (!validInput);
    }
    display_aliases_by_octets(octet[0], octet[1]);
}

// Function to print aliases based on two matching octets

void display_aliases_by_octets(int octet1, int octet2) {
    if (head == NULL) {
        printf("No addresses to display. The list is empty.\n");
        return;
    }
    int found = 0; 
    print_matching_aliases(head, octet1, octet2, &found);
    if (!found) {
        printf("No matching locations found for octets %d and %d.\n", octet1, octet2);
    }
}

// Helper function to traverse the tree and find matches
void print_matching_aliases(address_t *node, int octet1, int octet2, int *found) {
    if (node == NULL) return;
    print_matching_aliases(node->leftChild, octet1, octet2, found);

    //printf("Checking node with Alias: %s, IP: %d.%d.%d.%d\n", node->alias, node->octet[0], node->octet[1], node->octet[2], node->octet[3]);

    // Check if the first two octets match the specified octets
    if (node->octet[0] == octet1 && node->octet[1] == octet2) {
        printf("Alias: %s\n", node->alias);
        *found = 1;  // Set found flag
    }

    // Traverse the tree
    //print_matching_aliases(node->leftChild, octet1, octet2, found);
    print_matching_aliases(node->rightChild, octet1, octet2, found);
}


//Display the error log 
void display_error_log() {
    FILE *error_log = fopen("cs531_error_log.txt", "r");
    if (error_log == NULL) {
        printf("Error: Could not open error log file.\n");
        return;
    }

    char line[256];
    printf("Error Log:\n");
    while (fgets(line, sizeof(line), error_log)) {
        printf("%s", line);
    }
    
    fclose(error_log);
    printf("\n");
}