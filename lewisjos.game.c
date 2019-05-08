#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

struct Room {
   char name[9];
   char type[11];
   int numConnections;
   char connectionNames[6][9];
};

struct gameData {
    int startRoom;
    int numSteps;
    char path[20][9];
};


/* *************** HELPER FUNCTIONS *************** */

void printRooms(struct Room* rooms) {

   int i, j;
   for (i=0; i<7; i++) {
      printf("ROOM NAME: %s\n", rooms[i].name);
      
      for (j=0; j<rooms[i].numConnections; j++) {
         printf("CONNECTION %d: %s\n", j+1, rooms[i].connectionNames[j]);
      }

      printf("ROOM TYPE: %s\n\n", rooms[i].type);
   }

}

void extractData(struct Room* rooms, FILE* currentFile, int i, struct gameData* data) {

    char identifier[64];        // strings to hold data from files
    char nameOrType[64];
    char value[64];
    int j = 0;                  // used to count number of connections

    while (fscanf(currentFile, "%s %s %s", identifier, nameOrType, value) == 3) {
        
        // add the conneciton
        if (!strcmp(identifier, "CONNECTION")) {
            strcpy(rooms[i].connectionNames[j], value);
            j++;
        }

        // add the rooms name
        else if (!strcmp(nameOrType, "NAME:")) {
            strcpy(rooms[i].name, value);

        }

        // add the rooms type
        else if (!strcmp(nameOrType, "TYPE:")) {
            strcpy(rooms[i].type, value);

            if (!strcmp(value, "START_ROOM"))
                data->startRoom = i;
        }

        // error in file format
        else {
            printf("ERROR: INVALID FILE FORMAT\n");
        }

    }
    
    // update the number of connections
    rooms[i].numConnections = j;

}

void displayCurrentRoom(struct Room currentRoom) {

    printf("CURRENT LOCATION: %s\n", currentRoom.name);
    printf("POSSIBLE CONNECTIONS: ");

    int i;
    for (i=0; i<currentRoom.numConnections; i++) {
        printf("%s", currentRoom.connectionNames[i]);
        if (i != currentRoom.numConnections-1)
            printf(", ");
    }

    printf("\n");
}

void removeNewLine(char *str) {

    int len = strlen(str);

    if (str[len-1] == '\n')
        str[len-1] = '\0';

}

void getUserInput(char *str) {

    printf("WHERE TO? >");
    fgets(str, 64, stdin);
    removeNewLine(str);

}

int contains(struct Room currentRoom, char *userInput) {

    int i;

    for (i=0; i<currentRoom.numConnections; i++) {
        if (!strcmp(currentRoom.connectionNames[i], userInput))
            return 1;
    }
    return 0;

}

int getRoomID(struct Room *rooms, char *name) {

    int i;

    for (i=0; i<7; i++) {

        if (!strcmp(rooms[i].name, name))
            return i;

    }

    return 0;

}

void congratulate(struct gameData* data, struct Room currentRoom) {

    int i;

    printf("YOU HAVE FOUND THE END ROOM. CONGRATULAIONS!\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", data->numSteps);

    // print the name of all rooms visited (besides start and end)
    for (i=1; i<data->numSteps; i++)
        printf("%s\n", data->path[i]);
    
    // print the name of the last room
    printf("%s\n", currentRoom.name);

}

/* ************* LIFECYCLE FUNCTIONS ************* */
char * findDir() {

    DIR *dirP;                                  // directory pointer
    struct dirent *dirEntry;                    // directory entry
    char* newestDir = malloc(64 * sizeof(char));// allocate space for path name
    dirP = opendir(".");                        // open the directory
    struct stat sb;                             // struct to contain dir info
    time_t current = 0;

    // if directory opens
    if (dirP) {

        // loop over files in the directory
        while ((dirEntry = readdir(dirP)) != NULL) {

            // only operate on rooms that contain "lewisjos.rooms."
            if (strstr(dirEntry->d_name, "lewisjos.rooms.")) {

                stat(dirEntry->d_name, &sb);

                if(difftime(current, sb.st_mtime) < 0.0) {
                    current = sb.st_mtime;
                    strcpy(newestDir, dirEntry->d_name);
                }
            }           
        }

        // close the directory pointer
        closedir(dirP);
    }

    return newestDir;

}

void readFiles(struct Room* rooms, char* dir, struct gameData* data) {

    DIR *dirP;                                  // directory pointer
    FILE *currentFile;                          // file pointer
    struct dirent *dirEntry;                    // directory entry
    char* filePath = malloc(64 * sizeof(char)); // allocate space for path name
    dirP = opendir(dir);                        // open the directory
    int i = 0;                                  // current file counter

    // if directory opens
    if (dirP) {

        // loop over files in the directory
        while ((dirEntry = readdir(dirP)) != NULL) {

            // skip current and parent directories
            if (!strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, "..")) {
                continue;
            }

            // build pathname
            memset(filePath, '\0', 64);
            sprintf(filePath, "%s/%s", dir, dirEntry->d_name);

            // open the current file
            currentFile = fopen(filePath, "r+");

            // extract file data into room array
            extractData(rooms, currentFile, i, data);

            // close the current file
            fclose(currentFile);

            // increment file counter
            i++;
        }

        // close the directory pointer
        closedir(dirP);
    }

    free(filePath);
}

void play(struct Room* rooms, struct gameData* data) {

    int nextRoom;               // room id
    data->numSteps = 0;         // initialize the number of steps
    char userResponse[64];      // buffer for user input

    // set current room
    struct Room currentRoom = rooms[data->startRoom];    

    // run while the user has not reached the end
    while (strcmp(currentRoom.type, "END_ROOM")) {

        displayCurrentRoom(currentRoom);

        getUserInput(userResponse);
        
        if(contains(currentRoom, userResponse)) {

            // add name to path
            strcpy(data->path[data->numSteps], currentRoom.name);

            // add a step
            data->numSteps++;
            
            // get the id of the next room
            nextRoom = getRoomID(rooms, userResponse);

            // increment current room
            currentRoom = rooms[nextRoom];
        }
        else {
            printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
        }
        printf("\n");
    }

    congratulate(data, currentRoom);
}

void freeData(struct Room* rooms, struct gameData* data, char* recentDir) {
   free(rooms);
   free(data);
   free(recentDir);
}

/* **************** MAIN FUNCTION **************** */ 
int main() {

    // init rooms and game data
    struct Room* rooms = malloc(7*sizeof(struct Room));
    struct gameData* data = malloc(sizeof(struct gameData));

    // find most recent directory
    char* recentDir = findDir();

    // read the data in from most recent directory
    readFiles(rooms, recentDir, data);

    // play the game
    play(rooms, data);

    // free allocated memory
    freeData(rooms, data, recentDir);

    return 0;
}