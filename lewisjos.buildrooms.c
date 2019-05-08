#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct Room {
   char name[9];
   char type[11];
   int numConnections;
   int connections[6];
   char connectionNames[6][9];
};

/* *************** HELPER FUNCTIONS *************** */
int contains(int *array, int value) {
   
   int i;
   for (i=0; i<7; i++) {
      if (array[i] == value) {
         return 1;
      }
   }
   return 0;
}

void connectRooms(struct Room* rooms, int a, int b) {

   // connect a to b
   rooms[a].connections[rooms[a].numConnections] = b;
   strcpy(rooms[a].connectionNames[rooms[a].numConnections], rooms[b].name);
   rooms[a].numConnections++;

   // connect b to a
   rooms[b].connections[rooms[b].numConnections] = a;
   strcpy(rooms[b].connectionNames[rooms[b].numConnections], rooms[a].name);
   rooms[b].numConnections++;

}

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

/* ************* LIFECYCLE FUNCTIONS ************* */
char* makeDir() {

   // get PID
   int pid = getpid();

   // create directory name
   char* pidString = malloc(25 * sizeof(char));
   memset(pidString, '\0', 25);
   sprintf(pidString, "lewisjos.rooms.%d", pid);

   // create directory
   mkdir(pidString);

   return pidString;

}

struct Room* initRooms() {

   // random number seed
   srand(time(NULL));

   struct Room *rooms = malloc(7*sizeof(struct Room));

   int i, j;
   for (i=0; i<7; i++) {
      rooms[i].numConnections = 0;
      for (j=0; j<6; j++) {
         rooms[i].connections[j] = 99;
      }
   }

   return rooms;
}

void assignNames(struct Room* rooms) {

   // array of potential room names
   char roomNames[10][8] = {"Oregon", "Aloha", "Corv", "Doggo", "Endgame", "Miami", "Golem", "Hilltop", "Atom", "Jupiter"};

   int i;                  // loop index
   int randomIndex;        // random number container
   int randSize = 10;      // random upper bound (exclusive)

   //assign each room a name
   for (i=0; i<7; i++) {   // 7 is hardcoded

      // get random int from 0 to randSize
      randomIndex = rand() % randSize;

      // copy the name into the room struct
      strcpy(rooms[i].name, roomNames[randomIndex]);

      // if random value is not the end of the array
      if (randomIndex != randSize-1)

         // replace the used name with the back of array
         strcpy(roomNames[randomIndex], roomNames[randSize-1]);

      // decrement the upper bound
      randSize--;
   }
}

void assignTypes(struct Room* rooms) {

   /*
      Nothing random here
      Room 0 will always be start
      Rooms 1-5 will always be mid
      Room 6 will always be end
   */

   strcpy(rooms[0].type, "START_ROOM");
   strcpy(rooms[6].type, "END_ROOM");

   int i;
   for (i=1; i<6; i++) {
      strcpy(rooms[i].type, "MID_ROOM");
   }
}

void assignConnections(struct Room* rooms) {

   int i, j;
   int randNumConnections;
   int randomConnection;
   for (i=0; i<7; i++) {
      
      // determine number of connections
      randNumConnections = rand() % 4 + 3;
      
      // loop from current number of connections to final number
      for (j=rooms[i].numConnections; j<randNumConnections; j++) {

         // obtain a random room number that is
            // not the current room
            // not already coonected to the current room
         do {
            randomConnection = rand() % 7;
         } while (randomConnection == i || contains(rooms[i].connections, randomConnection));

         // connect the rooms to eachother
         connectRooms(rooms, i, randomConnection);

      }
   }
}

void writeRooms(struct Room* rooms) {

   // make the directory and get its name
   char* currentDir = makeDir();

   FILE * fp;
   int i, j;
   char fileName[25];
   for (i=0; i<7; i++) {

      // make file name (including path)
      memset(fileName, '\0', 25);
      sprintf(fileName, "%s/%s_room", currentDir, rooms[i].name);

      // open the current room file
      fp = fopen(fileName, "w+");

      // add room name
      fprintf(fp, "ROOM NAME: %s\n", rooms[i].name);
      
      // add the connected rooms
      for (j=0; j<rooms[i].numConnections; j++) {
         fprintf(fp, "CONNECTION %d: %s\n", j+1, rooms[i].connectionNames[j]);
      }

      // add the room type
      fprintf(fp, "ROOM TYPE: %s\n", rooms[i].type);

      // Close the file
      fclose(fp);
   }

   free(currentDir);
}

void freeRooms(struct Room* rooms) {
   free(rooms);
}

/* **************** MAIN FUNCTION **************** */ 
int main() {

   // allocate 7 Room structs
   struct Room *roomArray = initRooms();

   assignNames(roomArray);
   assignTypes(roomArray);
   assignConnections(roomArray);
   writeRooms(roomArray);
   freeRooms(roomArray);
   
   return 0;
}
