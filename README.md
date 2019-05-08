Operating Systems - Program 2

lewisjos.buildrooms.c
   - Builds 7 rooms
   - Each assigned a random name from list of 10
   - Each room is assigned one of three types
      - One room is the START_ROOM
      - One room is the END_ROOM
      - All other rooms are MID_ROOM
   - Each room is randomly connected to other rooms
      - Rooms connection go both ways (A connected to B and B connected to A)
      - Each room has anywhere from 3 to 6 connections
      - Rooms cannot be connected to themselves
      - Rooms can only be connected to another room once (no repeat connections)
   - Directory is created to store the room files (named with current process ID)

lewisjos.game.c
   - Reads in data from most recent rooms directory
      - checks the directory's last modified time to find most recent
      - file format must match that specified in assignment
   - The game begins
      - User is prompted with their current room and the rooms connected to it
      - User input is requested to navigate the rooms
      - The game ends when the room with a type of END_ROOM is reached
   - The user is congratulated and their number of steps and path taken is displayed