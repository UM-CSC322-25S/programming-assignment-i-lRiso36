//// Online C compiler to run C program online
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BOATS 120
#define MAX_NAME_LENGTH 128
#define MAX_PLACE_LENGTH 10
#define MAX_UNION_INFO 10
#define SLIP_FEE 12.5
#define LAND_FEE 14.00
#define TRAILOR_FEE 25.00
#define STORAGE_FEE 11.20
//_________________________________________________________________________

typedef char BoatName[MAX_NAME_LENGTH];
typedef char Place[MAX_PLACE_LENGTH];
typedef enum {
    SLIP,
    LAND,
    TRAILOR,
    STORAGE,
    NONE
} PlaceType;

typedef union {
    int slipNumber;        // For SLIP (1–85)
    char bayLetter;        // For LAND (A–Z)
    char licenseTag[16];   // For TRAILOR
    int storageNumber;     // For STORAGE (1–50)
} BoatUnionType;

typedef struct {
    BoatName name;
    int length;            
    PlaceType type;
    BoatUnionType otherInfo;
    double amountOwed;
} Boat;
//___________________________________________________________________
//functions
PlaceType StringToPlaceType(char *Place);
void LoadBoatsFromFile(Boat *allBoats[MAX_BOATS], char *fileName, int *totalBoats);
int compareBoats(const void *A, const void *B);
void SortBoats(Boat *allBoats[MAX_BOATS], int totalBoats);
void PrintMenu();
char getMenuChoice();
void PrintInventory(Boat *allBoats[], int totalBoats);
void AddBoat(Boat *allBoats[], int *totalBoats);
void RemoveBoat(Boat *allBoats[], int *totalBoats);
void PaymentBoat(Boat *allBoats[], int totalBoats);
void MonthUpdate(Boat *allBoats[], int totalBoats);
void SaveBoatsToFile(Boat *allBoats[], int totalBoats, const char *fileName);
//____________________________________________________________________

//main function
int main(int argc, char *argv[]) {
    FILE *BoatFile;
    Boat *allBoats[MAX_BOATS];  // Array to store boats
    //opening the file
    if (argc < 2) {
        printf("Must provide file name as argument\n");
            exit(EXIT_FAILURE);
    } //end of if
    BoatFile = fopen(argv[1], "r");
    if (BoatFile == NULL) {
    perror("Opening boat file");  // Print error if file can't be opened
    exit(EXIT_FAILURE);
    }//end of if


    char *fileName = argv[1];  // Get the file name from the command line
    int totalBoats = 0;

    // Load boats from the CSV file
    LoadBoatsFromFile(allBoats, fileName, &totalBoats);
    SortBoats(allBoats, totalBoats);

    char userChoice;
    printf("Welcome to the Boat Management System\n");
    printf("-------------------------------------");
    //opening statement
    do {
        PrintMenu();
        //scanf(" %c ", &userChoice);
        userChoice = getMenuChoice();
        switch(toupper(userChoice)) {
            case 'I':
                 if (totalBoats == 0)
        printf("No boats in the system.\n");
    else
        PrintInventory(allBoats, totalBoats);
    break;
            case 'A':
                printf("Please enter the boat data in CSV format                 : ");
                AddBoat(allBoats, &totalBoats);
                break;
            case 'R':
                RemoveBoat(allBoats, &totalBoats);
                break;
            case 'P':
                PaymentBoat(allBoats, totalBoats);
                break;
            case 'M':
                MonthUpdate(allBoats, totalBoats);
                break;
            case 'X':
                SaveBoatsToFile(allBoats, totalBoats, fileName);
                printf("Exiting the Boat Management System\n");
                break;
            default:
                printf("Invalid Option %c", userChoice);
                break;
        }
    } while (toupper(userChoice) != 'X');

    //close file
    if (fclose(BoatFile) == EOF){
       perror("Closing boat file");
       exit(EXIT_FAILURE);
    }//end of if
    return (EXIT_SUCCESS);
} //end of main

//function to turn string entered into an enum
PlaceType StringToPlaceType(char *Place) {
    if (!strcasecmp(Place, "slip")) return SLIP;
    if (!strcasecmp(Place, "land")) return LAND;
    if (!strcasecmp(Place, "trailor")) return TRAILOR;
    if (!strcasecmp(Place, "storage")) return STORAGE;
    return NONE;
} // end of StringToPlace

//function to put boat info from file into boat array
void LoadBoatsFromFile(Boat *allBoats[MAX_BOATS], char *fileName, int *totalBoats){
    FILE *currentFile; 
    if((currentFile = fopen(fileName, "r")) == NULL) {
        printf("Error. Could not open file %s\n", fileName);
        exit(EXIT_FAILURE);
    } //error message if file error
    
    char line[256];
    while (fgets(line, sizeof(line), currentFile)) {
        if (*totalBoats >= MAX_BOATS) {
            printf("Maximum boat count reached.\n");
            break;
        }// end of if
        //allocate memory for boat
            Boat* currentBoat = malloc(sizeof(Boat));
            if (!currentBoat) {
                printf("Memory allocation failed :(\n");
                fclose(currentFile);
                exit(EXIT_FAILURE);
                }//end of if
        
        //create temp variables to later input into structure
        char tempName[MAX_NAME_LENGTH], tempPlaceType[MAX_PLACE_LENGTH], tempOtherInfo[MAX_UNION_INFO];
        int tempLength;
        double tempAmountOwed;
         if (sscanf(line, " %127[^,],%d,%9[^,],%127[^,],%lf",
                   tempName, &tempLength, tempPlaceType, tempOtherInfo, &tempAmountOwed) != 5) {
            printf("Skipping invalid line: %s\n", line);
            free(currentBoat);
            continue;  // Skip to the next line if invalid
        }
    strncpy(currentBoat->name, tempName, MAX_NAME_LENGTH - 1);
        currentBoat->name[MAX_NAME_LENGTH - 1] = '\0';  // Ensure null termination

        // Assign other fields directly
        currentBoat->length = tempLength;
        currentBoat->type = StringToPlaceType(tempPlaceType);
        currentBoat->amountOwed = tempAmountOwed;
        
         switch (currentBoat->type) {
            case SLIP:
                currentBoat->otherInfo.slipNumber = atoi(tempOtherInfo);  // Convert string to int for slip number
                break;
            case LAND:
                currentBoat->otherInfo.bayLetter = tempOtherInfo[0];  // Store the bay letter
                break;
            case TRAILOR:
                // Assign the license tag using strncpy
                strncpy(currentBoat->otherInfo.licenseTag, tempOtherInfo, 15);  // 15 for the maximum length of the license tag
                currentBoat->otherInfo.licenseTag[15] = '\0';  // Ensure null termination
                break;
            case STORAGE:
                currentBoat->otherInfo.storageNumber = atoi(tempOtherInfo);  // Convert string to int for storage number
                break;
            default:
                printf("Unknown boat type: %s\n", tempPlaceType);
                free(currentBoat);
                continue;  // Skip to the next line if the boat type is invalid
        } //end of switch
         allBoats[*totalBoats] = currentBoat;
        (*totalBoats)++;  // Increment the total boat count
    } //end of while
    fclose(currentFile);  // Close the file
    
}// end of function

//function to compare alphabetical order
int compareBoats(const void *A, const void *B) {
    Boat *boatA = *(Boat **)A;
    Boat *boatB = *(Boat **)B;
    return strcasecmp(boatA->name, boatB->name);  // Case-insensitive comparison
} //end of cfunction

//function to sort boats alphabetically
void SortBoats(Boat *allBoats[MAX_BOATS], int totalBoats) {
    qsort(allBoats, totalBoats, sizeof(Boat *), compareBoats);
}//end of function

//function to print th emenu
void PrintMenu() {
    printf("\n(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
}//end of function

//function to get the userinput
char getMenuChoice() {
    char input;
        if (scanf(" %c", &input) != 1) {
        // If there's an error, clear and return default
        clearerr(stdin);
        return 'X';  // default to exit on error
    }

    // Clear rest of the line to avoid leftover characters (like '\n')
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    return input;
}//end of function

//function to print the inventory
void PrintInventory(Boat *allBoats[], int totalBoats) {
    // Loop over all boats and print them
    for (int i = 0; i < totalBoats; i++) {
        Boat *currentBoat = allBoats[i];

        // Print boat name and length (with ' after the length)
        printf("%-25s %3d'  ", currentBoat->name, currentBoat->length);
        // Print boat type and additional information
        switch (currentBoat->type) {
            case SLIP:
                printf("slip    ");
                printf(" # %3d" , currentBoat->otherInfo.slipNumber);
                break;
            case LAND:
                printf("land    ");
                printf("%6c", currentBoat->otherInfo.bayLetter);
                break;
            case TRAILOR:
                printf("trailor ");
                printf("%6s", currentBoat->otherInfo.licenseTag);
                break;
            case STORAGE:
                printf("storage ");
                printf(" # %3d" , currentBoat->otherInfo.storageNumber);
                break;
            default:
                printf("unknown");
        }

        // Print the amount owed, formatted to two decimal places
        printf("   Owes $%-8.2f\n", currentBoat->amountOwed);
    }
}//end of function

//function to add boats
void AddBoat(Boat *allBoats[], int *totalBoats) {
    char tempName[MAX_NAME_LENGTH], tempType[MAX_PLACE_LENGTH], tempUnion[16];
    int tempLength;
    double tempAmount;
    
    if (*totalBoats >= MAX_BOATS) {
        printf("Cannot add more boats. Marina is full.");
        return;
    }
    char line[256];
    fgets(line, sizeof(line), stdin);
    // Remove trailing newline
    line[strcspn(line, "\n")] = '\0';
    
    if (sscanf(line, " %127[^,],%d,%9[^,],%63[^,],%lf",
               tempName, &tempLength, tempType, tempUnion, &tempAmount) != 5) {
        printf("Invalid format. Boat not added.\n");
        return;
    } // end of if
    //allocate memory
    Boat *newBoat = malloc(sizeof(Boat));
    if (!newBoat) {
        printf("Memory allocation failed.\n");
        return;
    }
    strncpy(newBoat->name, tempName, MAX_NAME_LENGTH);
    newBoat->name[MAX_NAME_LENGTH - 1] = '\0';  // Ensure null-termination
    newBoat->length = tempLength;
    newBoat->amountOwed = tempAmount;
    newBoat->type = StringToPlaceType(tempType);
    
    switch (newBoat->type) {
        case SLIP:
            newBoat->otherInfo.slipNumber = atoi(tempUnion);
            break;
        case LAND:
            newBoat->otherInfo.bayLetter = toupper(tempUnion[0]);
            break;
        case TRAILOR:
            strncpy(newBoat->otherInfo.licenseTag, tempUnion, 15);
            newBoat->otherInfo.licenseTag[15] = '\0';
            break;
        case STORAGE:
            newBoat->otherInfo.storageNumber = atoi(tempUnion);
            break;
        default:
            printf("Invalid type. Boat not added.\n");
            free(newBoat);
            return;
    }
    int insertIndex = *totalBoats;
    while (insertIndex > 0 &&
           !strcasecmp(newBoat->name, allBoats[insertIndex - 1]->name)) {
               
        allBoats[insertIndex] = allBoats[insertIndex - 1];
        insertIndex--;
    }
    allBoats[insertIndex] = newBoat;
    (*totalBoats)++;
}//end of function

//function to remove a boat
void RemoveBoat(Boat *allBoats[], int *totalBoats) {
    char nameToRemove[MAX_NAME_LENGTH];
    int i, j;
    
    printf("Enter the name of the boat to remove: ");
    scanf(" %[^\n]", nameToRemove);
    
    for (i = 0; i < *totalBoats; i++) {
        if (!(strcasecmp(allBoats[i]->name, nameToRemove))) {
            // Match found — remove boat
            free(allBoats[i]);
            for (j = i; j < *totalBoats - 1; j++) {
                allBoats[j] = allBoats[j + 1];
            }
            (*totalBoats)--;
            printf("Boat \"%s\" removed successfully.\n", nameToRemove);
            return;
        }
    }//end of for

    printf("No boat found with the name \"%s\".\n", nameToRemove);
} // end of function

//function to pay for boat
void PaymentBoat(Boat *allBoats[], int totalBoats) {
    char name[MAX_NAME_LENGTH];
    double paymentAmount;
    int found = 0;

    printf("Please enter the boat name                               : ");
    scanf(" %[^\n]", name);  // Read full line including spaces

    for (int i = 0; i < totalBoats; i++) {
        if (strcasecmp(allBoats[i]->name, name) == 0) {
            found = 1;
            printf("Please enter the amount to be paid                       : ");
            scanf("%lf", &paymentAmount);

            if (paymentAmount > allBoats[i]->amountOwed) {
                printf("That is more than the amount owed, $%.2f\n", allBoats[i]->amountOwed);
                return;
            }

            allBoats[i]->amountOwed -= paymentAmount;
            printf("Payment successful. Remaining balance: $%.2f\n", allBoats[i]->amountOwed);
            return;
        }
    }

    if (!found) {
        printf("No boat found with the name \"%s\".\n", name);
    }
}//end o ffunction

//function to update prices
void MonthUpdate(Boat *allBoats[], int totalBoats) {
    for (int i = 0; i < totalBoats; i++) {
        Boat *currentBoat = allBoats[i];
        double monthlyCharge = 0.0;

        // Calculate the monthly charge based on the boat's type and length
        switch (currentBoat->type) {
            case SLIP:
                monthlyCharge = currentBoat->length * SLIP_FEE;
                break;
            case LAND:
                monthlyCharge = currentBoat->length * LAND_FEE;
                break;
            case TRAILOR:
                monthlyCharge = currentBoat->length * TRAILOR_FEE;
                break;
            case STORAGE:
                monthlyCharge = currentBoat->length * STORAGE_FEE;
                break;
            default:
                // Print the boat name if the type is unknown
                printf("Unknown boat type for \"%s\". Skipping fee calculation.\n", currentBoat->name);
                continue;  // Skip to the next boat if the type is unknown
        }

        // Update the amount owed for the boat
        currentBoat->amountOwed += monthlyCharge;
    }
}//end of function

//function to save boats to csv file
void SaveBoatsToFile(Boat *allBoats[], int totalBoats, const char *fileName) {
    FILE *outputFile;
    outputFile = fopen(fileName, "w");
    if (outputFile  == NULL) {
        printf("Error. Could not open file %s for writing.\n", fileName);
        exit(EXIT_FAILURE);
    }

    // Write the data of all boats into the CSV file
    for (int i = 0; i < totalBoats; i++) {
        Boat *currentBoat = allBoats[i];

        // Write boat data in CSV format
        switch (currentBoat->type) {
            case SLIP:
                fprintf(outputFile, "%s,%d,slip,%d,%.2f\n",
                        currentBoat->name,
                        currentBoat->length,
                        currentBoat->otherInfo.slipNumber,
                        currentBoat->amountOwed);
                break;
            case LAND:
                fprintf(outputFile, "%s,%d,land,%c,%.2f\n",
                        currentBoat->name,
                        currentBoat->length,
                        currentBoat->otherInfo.bayLetter,
                        currentBoat->amountOwed);
                break;
            case TRAILOR:
                fprintf(outputFile, "%s,%d,trailor,%s,%.2f\n",
                        currentBoat->name,
                        currentBoat->length,
                        currentBoat->otherInfo.licenseTag,
                        currentBoat->amountOwed);
                break;
            case STORAGE:
                fprintf(outputFile, "%s,%d,storage,%d,%.2f\n",
                        currentBoat->name,
                        currentBoat->length,
                        currentBoat->otherInfo.storageNumber,
                        currentBoat->amountOwed);
                break;
            default:
                // In case of an unknown boat type, skip saving that boat
                printf("Unknown boat type, skipping save for boat: %s\n", currentBoat->name);
                continue;
        }
    }

    // Close the file after writing
    if(fclose(outputFile) == EOF){
       perror("Closing File");
       exit(EXIT_FAILURE);
    }//end of if
}//end of function


