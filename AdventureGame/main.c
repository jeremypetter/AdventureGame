//
//  main.c
//  AdventureGame
//
//  Created by Jeremy Petter on 2015-05-07.
//  Copyright (c) 2015 Jeremy Petter. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct Room Room;

struct Room {
    Room* north;
    Room* south;
    Room* east;
    Room* west;
};

typedef struct Player Player;

struct Player {
    Room* position;
    int health;
};

Room* cube = NULL;
Room* goal = NULL;
Room* builder = NULL;
int dungeonX = 4;
int dungeonY = 4;
int gameState = 0;

void promptPlayer(Room* pointer);
Room* movePlayer(Room* playerPosition);

int randomNumber(int max);
Room* placeObject(int sizeX, int sizeY);
void checkCollisions (Player* player);
void takeDamage (Player* player);

void buildAllRooms(int sizeX, int sizeY);
int numberOfRoomsWestOfBuilder();
int numberOfRoomsNorthOfBuilder();
void buildRoomEastOfBuilder();
void startNewRowSouthOfPreviousRooms();
Room* newRoom ();

int main(int argc, const char * argv[]) {
    
    /* define RNG seed based on time */
    srand((unsigned)time(NULL));
    
    //BuildDungeon based on X and Y dimensions
    buildAllRooms(dungeonX, dungeonY);
    
    //Create Player, cube and goal in different locations
    Player *player = (Player*)malloc(sizeof(Player));
    player->position = placeObject(dungeonX, dungeonY);
    player->health  = 100;
    
    do {
        cube = placeObject(dungeonX, dungeonY);
    } while (cube == goal || cube == player->position);
        
    do {
        goal = placeObject(dungeonX, dungeonY);
    } while (goal == cube || goal == player->position);
    
    gameState = 1;
    do {
        //Prompt
        promptPlayer(player->position);
        player->position = movePlayer(player->position);
        //Check Collisions
        checkCollisions(player);
        //Win
        //Take Damage
        //Die
    } while (gameState == 1);
    
    
    
    return 0;
}

void win(){
    printf("****You found the treasure! Congratulations, you win!****\n");
    gameState = 0;
}

void lose(){
    printf("****You died to the deadly gelatinous cube!****\n");
    gameState = 0;
}

void takeDamage (Player* player){
    printf("**** You encounter the deadly gelatinous cube!****\n");
    player->health -= 50;
    printf("Health reduced to %d\n\n", player->health);
    
    //check death
    if (player->health < 1) {
        lose();
    } else {
        //if player isn't dead, move the cube
        do {
            cube = placeObject(dungeonX, dungeonY);
        } while (cube == goal || cube == player->position);
    }
}

void checkCollisions (Player* player){
    if (player->position == cube){
        takeDamage(player);
    }
    if (player->position == goal){
    //    win();
    }
}

char* getInput(){
    char* input = malloc(sizeof(char) *255);
    scanf("%s", input);
    return input;
}

Room* movePlayer(Room* playerPosition){
    char* input = getInput();
    switch (input[0]) {
        case 'n':
        case 'N':
            if (playerPosition->north == NULL){
                printf("You can't move that way\n>");
                return movePlayer(playerPosition);
            } else {
                playerPosition = playerPosition->north;
                printf("MOVED NORTH\n\n");
            }
            break;
        case 's':
        case 'S':
            if (playerPosition->south == NULL){
                printf("You can't move that way\n>");
                return movePlayer(playerPosition);
            } else {
                playerPosition = playerPosition->south;
                printf("MOVED SOUTH\n\n");
            }
            break;
        case 'e':
        case 'E':
            if (playerPosition->east == NULL){
                printf("You can't move that way\n>");
                return movePlayer(playerPosition);
            } else {
                playerPosition = playerPosition->east;
                printf("MOVED EAST\n\n");
            }
            break;
        case 'w':
        case 'W':
            if (playerPosition->west == NULL){
                printf("You can't move that way\n>");
                return movePlayer(playerPosition);
            } else {
                playerPosition = playerPosition->west;
                printf("MOVED WEST\n\n");
            }
            break;
        default:
            printf("What do you mean? Which direction?\n>");
            return movePlayer(playerPosition);
            break;
    }
    return playerPosition;
}

void promptPlayer(Room* pointer){
    printf("You are in a dark room. There are available exits in the following directions:\n");
    if (pointer->north != NULL)
        printf("NORTH ");
    if (pointer->south != NULL)
        printf("SOUTH ");
    if (pointer->east != NULL)
        printf("EAST ");
    if (pointer->west != NULL)
        printf("WEST ");
    printf("\nEnter the direction in which you would like to move.\n>");
}

int randomNumber(int max){
    return (rand() % max);
}

Room* placeObject(int sizeX, int sizeY){
    int x = randomNumber(sizeX);
    int y = randomNumber(sizeY);
    Room* pointer = builder;
    
    for (int i = 0 ; i < x; i++){
        pointer = pointer->east;
    }
    for (int i = 0 ; i < y; i++){
        pointer = pointer->south;
    }
    return pointer;
}

//Dungeon Construction Methods
    void buildAllRooms(int sizeX, int sizeY){
//        int count = 0;                      //DEBUG
        
        if (builder == NULL){
            builder = newRoom();        //count++; printf("%d  ", count); //DEBUG
        }
        
        while(numberOfRoomsWestOfBuilder() < sizeX-1){
            buildRoomEastOfBuilder();
//            count++; printf("%d  ", count); //DEBUG
        }
        while (numberOfRoomsNorthOfBuilder() < sizeY-1){
            startNewRowSouthOfPreviousRooms();
//            count++; printf("\n%d  ", count); //DEBUG

            while(numberOfRoomsWestOfBuilder() < sizeX-1){
                buildRoomEastOfBuilder();
//                count++; printf("%d  ", count); //DEBUG
            }
        }
        //return builder to northwest corner
        while (builder->west != NULL) builder = builder->west;
        while (builder->north != NULL) builder = builder->north;
        
    }

    void buildRoomEastOfBuilder(){
        builder->east = newRoom();
        
        // create two-way link between rooms
        builder->east->west = builder;
        builder = builder->east;
        
        //check if builder is on at least the second row
        if (builder->west->north != NULL){
            // create two-way link with room to the north
            builder->north = builder->west->north->east;
            builder->west->north->east->south = builder;
        }
    }

    void startNewRowSouthOfPreviousRooms(){
        // move builder to far west room
        while (builder->west != NULL){
            builder = builder->west;
        }
        builder->south = newRoom();
        
        // create two-way link between rooms
        builder->south->north = builder;
        builder = builder->south;
        
    }

    int numberOfRoomsWestOfBuilder(){
        Room* query = builder;
        
        int count = 0;
        while (query->west != NULL){
            query = query->west;
            count ++;
        }
        return count;
    }

    int numberOfRoomsNorthOfBuilder(){
        Room* query = builder;
        
        int count = 0;
        while (query->north != NULL){
            query = query->north;
            count ++;
        }
        return count;
    }

    Room* newRoom (){
        Room* room = (Room*)malloc(sizeof(Room));
        room-> north = NULL;
        room-> south = NULL;
        room-> east = NULL;
        room-> west = NULL;

        return room;
    }


