#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <math.h>
#include "functions.h"

//// Define the dungeon grid size
//#define DUNGEON_WIDTH 120
//#define DUNGEON_HEIGHT 40
//
//// Cell types
//#define WALL 1
//#define FLOOR 0
//#define CORRIDOR '#'
//#define DOOR '+'
//#define WALL_H '_'
//#define WALL_V '|'
//#define PLAYER 'P'

// Dungeon grid
int dungeon[FLOORS_NUM][DUNGEON_HEIGHT][DUNGEON_WIDTH];
int copyDungeon[FLOORS_NUM][DUNGEON_HEIGHT][DUNGEON_WIDTH];

//// Structure for a rectangle (sub-dungeon)
//typedef struct {
//    int x_min, x_max, y_min, y_max;
//} Rect;
//
//typedef struct {
//    int x_min, x_max, y_min, y_max;
//    int index;
//} Room;
//
//typedef struct {
//    int x;
//    int y;
//    // Other player properties like health, name, etc.
//} Player;

// Stairs
Staircase staircases[FLOORS_NUM * 2];  // Array to store staircases (2 per floor)
int staircaseCount = 0;                // Counter for staircases

// List of rooms
Room rooms[MAX_ROOMS];
int roomCount = 0;
int currentFloor = 0;

// Function to initialize the dungeon with walls
void initDungeon() {
    for (int y = 0; y < DUNGEON_HEIGHT; y++) {
        for (int x = 0; x < DUNGEON_WIDTH; x++) {
            dungeon[currentFloor][y][x] = WALL;  // Start with all walls
        }
    }
}

// Function to carve a room inside a sub-dungeon
void carveRoom(Rect area) {
    // Ensure the room is at least 5x5
    int room_width = area.x_max - area.x_min;
    int room_height = area.y_max - area.y_min;

    if (room_width < 5 || room_height < 5) {
        //mvprintw(0, 0, "Area too small for a room: %dx%d", room_width, room_height);
        refresh();  // Refresh to display the message
        return;  // Skip if the area is too small for a 5x5 room
    }

    // Generate random room dimensions (width and height) that are at least 5x5
    int room_rand_width = 5 + rand() % (room_width - 4);  // Random width between 5 and room_width
    int room_rand_height = 5 + rand() % (room_height - 4);  // Random height between 5 and room_height

    // Ensure the room fits inside the sub-dungeon (room's position and size must be valid)
    int room_x_min = area.x_min + rand() % (room_width - room_rand_width);  // Random x position within bounds
    int room_x_max = room_x_min + room_rand_width;
    int room_y_min = area.y_min + rand() % (room_height - room_rand_height);  // Random y position within bounds
    int room_y_max = room_y_min + room_rand_height;

    // Ensure the room is within the dungeon bounds
    if (room_x_min < 0 || room_x_max >= DUNGEON_WIDTH || room_y_min < 0 || room_y_max >= DUNGEON_HEIGHT) {
        //mvprintw(1, 0, "Room out of bounds: (%d,%d) to (%d,%d)", room_x_min, room_y_min, room_x_max, room_y_max);
        refresh();  // Refresh to display the message
        return;  // Skip if the room is out of bounds
    }

    // Carve out the floor: set all tiles inside the room to FLOOR (.)
    for (int y = room_y_min + 1; y < room_y_max - 1; y++) {  // Exclude the room boundaries
        for (int x = room_x_min + 1; x < room_x_max - 1; x++) {
            dungeon[currentFloor][y][x] = FLOOR;  // Set the floor inside the room
        }
    }

    // Place vertical walls (|) on the left and right boundaries of the room
    for (int y = room_y_min; y < room_y_max; y++) {
        if (room_x_min >= 0 && room_x_min < DUNGEON_WIDTH) {
            dungeon[currentFloor][y][room_x_min] = WALL_V;  // Left wall
        }
        if (room_x_max - 1 >= 0 && room_x_max - 1 < DUNGEON_WIDTH) {
            dungeon[currentFloor][y][room_x_max - 1] = WALL_V;  // Right wall
        }
    }

    // Place horizontal walls (_) on the top and bottom boundaries of the room
    for (int x = room_x_min; x < room_x_max; x++) {
        if (room_y_min >= 0 && room_y_min < DUNGEON_HEIGHT) {
            dungeon[currentFloor][room_y_min][x] = WALL_H;  // Top wall
        }
        if (room_y_max - 1 >= 0 && room_y_max - 1 < DUNGEON_HEIGHT) {
            dungeon[currentFloor][room_y_max - 1][x] = WALL_H;  // Bottom wall
        }
    }
    // Save the room in the rooms list
    rooms[roomCount].x_min = room_x_min;
    rooms[roomCount].x_max = room_x_max;
    rooms[roomCount].y_min = room_y_min;
    rooms[roomCount].y_max = room_y_max;
    rooms[roomCount].index = roomCount;  // Assign a unique index
    roomCount++;
    //mvprintw(2, 0, "Carved room: (%d,%d) to (%d,%d)", room_x_min, room_y_min, room_x_max, room_y_max);
    refresh();  // Refresh to display the message
}


// Function to split the dungeon into 4 equal rectangles and then split 2 horizontally and 2 vertically
void splitDungeon(Rect dungeonArea) {
    // Split the dungeon into 4 equal rectangles
    int midX = (dungeonArea.x_min + dungeonArea.x_max) / 2;
    int midY = (dungeonArea.y_min + dungeonArea.y_max) / 2;

    Rect subDungeons[4] = {
            {dungeonArea.x_min, midX, dungeonArea.y_min, midY},  // Top-left
            {midX, dungeonArea.x_max, dungeonArea.y_min, midY},  // Top-right
            {dungeonArea.x_min, midX, midY, dungeonArea.y_max},  // Bottom-left
            {midX, dungeonArea.x_max, midY, dungeonArea.y_max}   // Bottom-right
    };

    // Randomly choose 2 of the 4 sub-dungeons to split horizontally
    int indices[4] = {0, 1, 2, 3};
    for (int i = 0; i < 2; i++) {
        int randIndex = rand() % (4 - i);  // Random index from remaining indices
        int chosenIndex = indices[randIndex];

        // Swap the chosen index with the last unprocessed index
        indices[randIndex] = indices[3 - i];
        indices[3 - i] = chosenIndex;

        // Split the chosen sub-dungeon horizontally
        Rect chosenSubDungeon = subDungeons[chosenIndex];
        int splitY = chosenSubDungeon.y_min + 4 + rand() % (chosenSubDungeon.y_max - chosenSubDungeon.y_min - 8);

        // Split the chosen sub-dungeon into two parts
        Rect subA = {chosenSubDungeon.x_min, chosenSubDungeon.x_max, chosenSubDungeon.y_min, splitY};
        Rect subB = {chosenSubDungeon.x_min, chosenSubDungeon.x_max, splitY, chosenSubDungeon.y_max};

        // Carve rooms in the new sub-dungeons
        carveRoom(subA);
        carveRoom(subB);
    }

    // Split the remaining 2 sub-dungeons vertically
    for (int i = 0; i < 2; i++) {
        Rect chosenSubDungeon = subDungeons[indices[i]];

        // Split the chosen sub-dungeon vertically
        int splitX = chosenSubDungeon.x_min + 4 + rand() % (chosenSubDungeon.x_max - chosenSubDungeon.x_min - 8);

        // Split the chosen sub-dungeon into two parts
        Rect subA = {chosenSubDungeon.x_min, splitX, chosenSubDungeon.y_min, chosenSubDungeon.y_max};
        Rect subB = {splitX, chosenSubDungeon.x_max, chosenSubDungeon.y_min, chosenSubDungeon.y_max};

        // Carve rooms in the new sub-dungeons
        carveRoom(subA);
        carveRoom(subB);
    }
}

// Function to calculate Manhattan distance between two rooms
int manhattanDistance(Room r1, Room r2) {
    int center_x1 = (r1.x_min + r1.x_max) / 2;
    int center_y1 = (r1.y_min + r1.y_max) / 2;
    int center_x2 = (r2.x_min + r2.x_max) / 2;
    int center_y2 = (r2.y_min + r2.y_max) / 2;

    return abs(center_x2 - center_x1) + abs(center_y2 - center_y1);
}

//// Union-Find structure to manage room connections
//typedef struct {
//    int *parent;
//    int *rank;
//    int n;
//} UnionFind;

// Initialize Union-Find data structure
UnionFind* createUnionFind(int n) {
    UnionFind *uf = (UnionFind*)malloc(sizeof(UnionFind));
    uf->n = n;
    uf->parent = (int*)malloc(n * sizeof(int));
    uf->rank = (int*)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
    }

    return uf;
}

int allRoomsConnected(UnionFind* uf, int roomCount);  // Function prototype

// Find with path compression
int find(UnionFind *uf, int x) {
    if (uf->parent[x] != x)
        uf->parent[x] = find(uf, uf->parent[x]);
    return uf->parent[x];
}

// Union by rank
void unionSets(UnionFind *uf, int x, int y) {
    int rootX = find(uf, x);
    int rootY = find(uf, y);

    if (rootX != rootY) {
        if (uf->rank[rootX] > uf->rank[rootY]) {
            uf->parent[rootY] = rootX;
        } else if (uf->rank[rootX] < uf->rank[rootY]) {
            uf->parent[rootX] = rootY;
        } else {
            uf->parent[rootY] = rootX;
            uf->rank[rootX]++;
        }
    }
}

// Function to carve a flexible corridor between two rooms (allowing for turns)
void carveCorridor(Room r1, Room r2) {
    int x1 = (r1.x_min + r1.x_max) / 2;  // Center of room 1
    int y1 = (r1.y_min + r1.y_max) / 2;  // Center of room 1
    int x2 = (r2.x_min + r2.x_max) / 2;  // Center of room 2
    int y2 = (r2.y_min + r2.y_max) / 2;  // Center of room 2
    int ys=y1;

    // Carve horizontally first if necessary
    if (x1 != x2) {
        if (x1 > x2) {  // If room 1 is to the right of room 2
            for (int x = x1; x >= x2; x--) {
                // Only replace wall with corridor once
                if (dungeon[currentFloor][ys][x] == WALL_V) {
                    dungeon[currentFloor][ys][x] = DOOR;  // Replace wall with corridor
                }else if (dungeon[currentFloor][ys][x] == WALL_H && ys > y2) {
                    ys=ys-1;
                    x=x+1;
                    dungeon[currentFloor][ys][x] = CORRIDOR;  // Carve the corridor
                }else if (dungeon[currentFloor][ys][x] == WALL_H && ys < y2) {
                    ys=ys+1;
                    x=x+1;
                    dungeon[currentFloor][ys][x] = CORRIDOR;  // Carve the corridor
                }else if (dungeon[currentFloor][ys][x] != DOOR && dungeon[currentFloor][ys][x] != CORRIDOR && dungeon[currentFloor][ys][x] != FLOOR) {
                    dungeon[currentFloor][ys][x] = CORRIDOR;  // Carve the corridor
                }
            }
        } else {  // If room 1 is to the left of room 2
            for (int x = x1; x <= x2; x++) {
                // Only replace wall with corridor once
                if (dungeon[currentFloor][ys][x] == WALL_V) {
                    dungeon[currentFloor][ys][x] = DOOR;  // Replace wall with corridor
                } else if (dungeon[currentFloor][ys][x] == WALL_H && ys > y2) {
                    ys=ys-1;
                    x=x-1;
                    dungeon[currentFloor][ys][x] = CORRIDOR;  // Carve the corridor
                }else if (dungeon[currentFloor][ys][x] == WALL_H && ys < y2) {
                    ys=ys+1;
                    x=x-1;
                    dungeon[currentFloor][ys][x] = CORRIDOR;  // Carve the corridor
                } else if (dungeon[currentFloor][ys][x] != DOOR && dungeon[currentFloor][ys][x] != CORRIDOR && dungeon[currentFloor][ys][x] != FLOOR) {
                    dungeon[currentFloor][ys][x] = CORRIDOR;  // Carve the corridor
                }
            }
        }
    }

    // Now carve vertically from the corridor end to the other room
    if (y1 != y2) {
        if (y1 > y2) {  // If room 1 is below room 2
            for (int y = ys; y >= y2; y--) {
                if ((dungeon[currentFloor][y][x2] == WALL_V || dungeon[currentFloor][y][x2] == DOOR) && dungeon[currentFloor][y][x2 + 1] == WALL) {
                    dungeon[currentFloor][y][x2 + 1] = CORRIDOR;
                } else if ((dungeon[currentFloor][y][x2] == WALL_V || dungeon[currentFloor][y][x2] == DOOR) && dungeon[currentFloor][y][x2 - 1] == WALL) {
                    dungeon[currentFloor][y][x2 - 1] = CORRIDOR;
                }else if (dungeon[currentFloor][y][x2] == WALL_H && dungeon[currentFloor][y][x2 + 1] == WALL) {
                    dungeon[currentFloor][y][x2 + 1] = CORRIDOR;
                    dungeon[currentFloor][y - 1][x2 + 1] = CORRIDOR;
                } else if (dungeon[currentFloor][y][x2] == WALL_H && dungeon[currentFloor][y][x2 - 1] == WALL) {
                    dungeon[currentFloor][y][x2 - 1] = CORRIDOR;
                    dungeon[currentFloor][y - 1][x2 - 1] = CORRIDOR;
                } else if (dungeon[currentFloor][y][x2] != DOOR && dungeon[currentFloor][y][x2] != CORRIDOR && dungeon[currentFloor][y][x2] != WALL_V && dungeon[currentFloor][y][x2] != WALL_H && dungeon[currentFloor][y][x2] != FLOOR) {
                    dungeon[currentFloor][y][x2] = CORRIDOR;  // Carve the corridor
                } else if (dungeon[currentFloor][y][x2] == WALL_H) {
                    dungeon[currentFloor][y][x2] = DOOR;
                }
            }
        } else {  // If room 1 is above room 2
            for (int y = ys; y <= y2; y++) {
                if ((dungeon[currentFloor][y][x2] == WALL_V || dungeon[currentFloor][y][x2] == DOOR) && dungeon[currentFloor][y][x2 + 1] == WALL) {
                    dungeon[currentFloor][y][x2 + 1] = CORRIDOR;
                } else if ((dungeon[currentFloor][y][x2] == WALL_V || dungeon[currentFloor][y][x2] == DOOR) && dungeon[currentFloor][y][x2 - 1] == WALL) {
                    dungeon[currentFloor][y][x2 - 1] = CORRIDOR;
                }else if (dungeon[currentFloor][y][x2] == WALL_H && dungeon[currentFloor][y][x2 + 1] == WALL) {
                    dungeon[currentFloor][y][x2 + 1] = CORRIDOR;
                    dungeon[currentFloor][y + 1][x2 + 1] = CORRIDOR;
                } else if (dungeon[currentFloor][y][x2] == WALL_H && dungeon[currentFloor][y][x2 - 1] == WALL) {
                    dungeon[currentFloor][y][x2 - 1] = CORRIDOR;
                    dungeon[currentFloor][y + 1][x2 - 1] = CORRIDOR;
                } else if (dungeon[currentFloor][y][x2] != DOOR && dungeon[currentFloor][y][x2] != CORRIDOR && dungeon[currentFloor][y][x2] != WALL_V && dungeon[currentFloor][y][x2] != WALL_H && dungeon[currentFloor][y][x2] != FLOOR) {
                    dungeon[currentFloor][y][x2] = CORRIDOR;  // Carve the corridor
                } else if (dungeon[currentFloor][y][x2] == WALL_H) {
                    dungeon[currentFloor][y][x2] = DOOR;
                }
            }
        }
    }
}

void connectRooms(Room rooms[], int roomCount) {
    UnionFind* uf = createUnionFind(roomCount);

    // Create an edge list to store the distance and pair of rooms
    typedef struct {
        int dist;
        int room1;
        int room2;
    } Edge;

    Edge edges[roomCount * (roomCount - 1) / 2];
    int edgeIndex = 0;

    // Generate all edges (room pairs) with their distances
    for (int i = 0; i < roomCount; i++) {
        for (int j = i + 1; j < roomCount; j++) {
            edges[edgeIndex].dist = manhattanDistance(rooms[i], rooms[j]);
            edges[edgeIndex].room1 = i;
            edges[edgeIndex].room2 = j;
            edgeIndex++;
        }
    }

    // Sort edges based on distance (bubble sort or any efficient sort)
    for (int i = 0; i < edgeIndex; i++) {
        for (int j = i + 1; j < edgeIndex; j++) {
            if (edges[i].dist > edges[j].dist) {
                Edge temp = edges[i];
                edges[i] = edges[j];
                edges[j] = temp;
            }
        }
    }

    // Kruskal’s algorithm to connect rooms
    for (int i = 0; i < edgeIndex; i++) {
        int room1 = edges[i].room1;
        int room2 = edges[i].room2;

        // If rooms are not connected, carve a corridor and union them
        if (find(uf, room1) != find(uf, room2)) {
            carveCorridor(rooms[room1], rooms[room2]);
            unionSets(uf, room1, room2);
        }
    }

    // Check if all rooms are connected
    if (!allRoomsConnected(uf, roomCount)) {
        //printf("Warning: Not all rooms are connected.\n");
        // Here we can try to connect the remaining unconnected rooms
        for (int i = 0; i < roomCount; i++) {
            for (int j = i + 1; j < roomCount; j++) {
                if (find(uf, i) != find(uf, j)) {
                    carveCorridor(rooms[i], rooms[j]);
                    unionSets(uf, i, j);
                }
            }
        }
    }

    // Clean up the UnionFind structure after use
    free(uf->parent);
    free(uf->rank);
    free(uf);
}

// Function to display the dungeon
void displayDungeon() {
    // Render the dungeon using ncurses
    for (int y = 0; y < DUNGEON_HEIGHT; y++) {
        for (int x = 0; x < DUNGEON_WIDTH; x++) {
            if (dungeon[currentFloor][y][x] == WALL) {
                mvaddch(y, x, ' ');  // Display walls as ' '
            } else if (dungeon[currentFloor][y][x] == FLOOR) {
                mvaddch(y, x, '.');  // Display empty space as '.'
            } else if (dungeon[currentFloor][y][x] == CORRIDOR) {
                mvaddch(y, x, '#');  // Display corridors as '#'
            } else if (dungeon[currentFloor][y][x] == DOOR) {
                mvaddch(y, x, '+'); // Display doors as '+'
            } else if (dungeon[currentFloor][y][x] == WALL_V) {
                mvaddch(y, x, '|'); // Display wall v as '|'
            } else if (dungeon[currentFloor][y][x] == WALL_H) {
                mvaddch(y, x, '_'); // Display wall h as '_'
            } else if (dungeon[currentFloor][y][x] == PLAYER) {
                mvaddch(y, x, 'P'); // Display player as 'P'
            } else if (dungeon[currentFloor][y][x] == DOWN_STAIR) {
                mvaddch(y, x, '<'); // Display down stair as '<'
            } else if (dungeon[currentFloor][y][x] == UP_STAIR) {
                mvaddch(y, x, '>'); // Display up stair as '>'
            } else if (dungeon[currentFloor][y][x] == COLUMN) {
                mvaddch(y, x, 'O'); // Display column as 'O'
            } else if (dungeon[currentFloor][y][x] == GOLD) {
                attron(COLOR_PAIR(4));
                mvaddch(y, x, 'G'); // Display gold as 'G'
                attroff(COLOR_PAIR(4));
            } else if (dungeon[currentFloor][y][x] == BLACK_GOLD) {
                attron(COLOR_PAIR(5));
                mvaddch(y, x, 'B'); // Display black gold as 'B'
                attroff(COLOR_PAIR(5));
            } else if (dungeon[currentFloor][y][x] == TRAP) {
                attron((COLOR_PAIR(2)));
                mvaddch(y, x, 'T'); // Display trap as 'T'
                attroff(COLOR_PAIR(2));
            }
        }
    }
    refresh();  // Refresh the screen to show the dungeon
}

// Function to check if all rooms are connected in the Union-Find structure
int allRoomsConnected(UnionFind* uf, int roomCount) {
    int root = find(uf, 0); // Get the root of the first room
    for (int i = 1; i < roomCount; i++) {
        if (find(uf, i) != root) {
            return 0;  // Not all rooms are connected
        }
    }
    return 1;  // All rooms are connected
}

// Function to initialize player in the first room
void placePlayerInFirstRoom(Player *player) {
    if (roomCount > 0) {
        Room firstRoom = rooms[0]; // Get the first room
        currentFloor = 0;
        int centerX = (firstRoom.x_min + firstRoom.x_max) / 2;
        int centerY = (firstRoom.y_min + firstRoom.y_max) / 2;

        // Make sure the center is a valid position (not a wall or out of bounds)
        while (dungeon[currentFloor][centerY][centerX] == WALL || dungeon[currentFloor][centerY][centerX] == WALL_H || dungeon[currentFloor][centerY][centerX] == WALL_V) {
            // Move around the center if it's blocked
            centerX++;
            if (centerX > firstRoom.x_max) {
                centerX = firstRoom.x_min;
                centerY++;
                if (centerY > firstRoom.y_max) {
                    centerY = firstRoom.y_min;
                }
            }
            // Ensure we don't go out of the room's bounds
            if (centerX > firstRoom.x_max || centerX < firstRoom.x_min || centerY > firstRoom.y_max || centerY < firstRoom.y_min) {
                centerX = firstRoom.x_min;  // Reset to starting point if overflow occurs
                centerY = firstRoom.y_min;
            }
        }

        // Place the player at the valid position
        player->x = centerX;
        player->y = centerY;

        // Mark the player's position on the dungeon map
        dungeon[currentFloor][player->y][player->x] = PLAYER;  // Use 'P' for the player character
    }
}

void copyDung() {
    for (int y = 0; y < DUNGEON_HEIGHT; y++) {
        for (int x = 0; x < DUNGEON_WIDTH; x++) {
            copyDungeon[currentFloor][y][x] = dungeon[currentFloor][y][x];
        }
    }
}

void movePlayer(Player *player, int newX, int newY) {
    // Check if the new position is within bounds and not a wall
    if (newX >= 0 && newX < DUNGEON_WIDTH && newY >= 0 && newY < DUNGEON_HEIGHT) {
        if (dungeon[currentFloor][newY][newX] == FLOOR || dungeon[currentFloor][newY][newX] == CORRIDOR || dungeon[currentFloor][newY][newX] == DOOR || dungeon[currentFloor][newY][newX] == UP_STAIR || dungeon[currentFloor][newY][newX] == DOWN_STAIR || dungeon[currentFloor][newY][newX] == GOLD || dungeon[currentFloor][newY][newX] == BLACK_GOLD || dungeon[currentFloor][newY][newX] == TRAP) {

            if (dungeon[currentFloor][newY][newX] == UP_STAIR) {
                int j = 0;
                dungeon[currentFloor][player->y][player->x] = copyDungeon[currentFloor][player->y][player->x];
                currentFloor += 1;
                for (int i = 0; i < staircaseCount; i++) {
                    if (staircases[i].floor == currentFloor && staircases[i].type == DOWN_STAIR) {
                        j = i;
                    }
                }
                player->x = staircases[j].x;
                player->y = staircases[j].y;
                dungeon[currentFloor][player->y][player->x] = PLAYER;

            } else if (dungeon[currentFloor][newY][newX] == DOWN_STAIR) {
                int j = 0;
                dungeon[currentFloor][player->y][player->x] = copyDungeon[currentFloor][player->y][player->x];
                currentFloor -= 1;
                for (int i = 0; i < staircaseCount; i++) {
                    if (staircases[i].floor == currentFloor && staircases[i].type == UP_STAIR) {
                        j = i;
                    }
                }
                player->x = staircases[j].x;
                player->y = staircases[j].y;
                dungeon[currentFloor][player->y][player->x] = PLAYER;
            } else if (dungeon[currentFloor][newY][newX] == GOLD) {
                int coins = rand() % 3 + 1;
                player->gold += coins;
                dungeon[currentFloor][player->y][player->x] = copyDungeon[currentFloor][player->y][player->x];
                player->x = newX;
                player->y = newY;
                dungeon[currentFloor][player->y][player->x] = PLAYER;
            } else if (dungeon[currentFloor][newY][newX] == BLACK_GOLD) {
                int coins = rand() % 3 + 5;
                player->gold += coins;
                dungeon[currentFloor][player->y][player->x] = copyDungeon[currentFloor][player->y][player->x];
                player->x = newX;
                player->y = newY;
                dungeon[currentFloor][player->y][player->x] = PLAYER;
            } else if (dungeon[currentFloor][newY][newX] == TRAP) {
                player->health -= 5;
                dungeon[currentFloor][player->y][player->x] = copyDungeon[currentFloor][player->y][player->x];
                player->x = newX;
                player->y = newY;
                dungeon[currentFloor][player->y][player->x] = PLAYER;
            } else {
                // Clear the previous position
                dungeon[currentFloor][player->y][player->x] = copyDungeon[currentFloor][player->y][player->x];

                // Update player position
                player->x = newX;
                player->y = newY;

                // Mark the new player position
                dungeon[currentFloor][player->y][player->x] = PLAYER;
            }
        }
    }
}

void handleInput(Player *player, int *running) {
    int ch = getch();  // Get user input

    switch (ch) {
        case '8': case 'j': movePlayer(player, player->x, player->y - 1); break;
        case '2': case 'k': movePlayer(player, player->x, player->y + 1); break;
        case '4': case 'h': movePlayer(player, player->x - 1, player->y); break;
        case '6': case 'l': movePlayer(player, player->x + 1, player->y); break;
        case '7': case 'y': movePlayer(player, player->x - 1, player->y - 1); break;
        case '9': case 'u': movePlayer(player, player->x + 1, player->y - 1); break;
        case '1': case 'b': movePlayer(player, player->x - 1, player->y + 1); break;
        case '3': case 'n': movePlayer(player, player->x + 1, player->y + 1); break;
        case 'q':  // Quit the game if 'q' is pressed
            *running = 0;  // Set running to 0 to break the loop
            break;
        default:
            break;  // No movement
    }

    // Display the dungeon after the move
    displayDungeon();
}

// Function to initialize stairs in room
void placeStairs() {
    if (roomCount > 0) {
        Room firstRoom = rooms[0];
        Room lastRoom = rooms[roomCount - 1];

        Staircase newStaircase;

        if (currentFloor == 0) {
            int x1 = lastRoom.x_min;
            int x2 = lastRoom.x_max;
            int y1 = lastRoom.y_min;
            int y2 = lastRoom.y_max;

            int randX = x1 + 1 + rand() % (x2 - x1 - 2);
            int randY = y1 + 1 + rand() % (y2 - y1 - 2);

            if (dungeon[currentFloor][randY][randX] == FLOOR) {
                newStaircase.x = randX;
                newStaircase.y = randY;
                newStaircase.type = UP_STAIR;
                newStaircase.floor = currentFloor;

                staircases[staircaseCount++] = newStaircase;

                dungeon[currentFloor][randY][randX] = UP_STAIR;
            }

        } else if (currentFloor == 1) {
            int x1 = firstRoom.x_min;
            int x2 = firstRoom.x_max;
            int y1 = firstRoom.y_min;
            int y2 = firstRoom.y_max;

            int randX = x1 + 1 + rand() % (x2 - x1 - 2);
            int randY = y1 + 1 + rand() % (y2 - y1 - 2);

            if (dungeon[currentFloor][randY][randX] == FLOOR) {
                newStaircase.x = randX;
                newStaircase.y = randY;
                newStaircase.type = DOWN_STAIR;
                newStaircase.floor = currentFloor;

                staircases[staircaseCount++] = newStaircase;

                dungeon[currentFloor][randY][randX] = DOWN_STAIR;
            }
        }
    }
}

// Function to initialize columns in room
void addColumns() {
    for (int i = 0; i < roomCount - 1; i++) {
        int columnCount = rand() % 2;

        Room currentRoom = rooms[i];

        int x1 = currentRoom.x_min;
        int x2 = currentRoom.x_max;
        int y1 = currentRoom.y_min;
        int y2 = currentRoom.y_max;

        for (int j = 0; j < columnCount; j++) {
            int randX = x1 + 1 + rand() % (x2 - x1 - 2);
            int randY = y1 + 1 + rand() % (y2 - y1 - 2);

            if (dungeon[currentFloor][randY][randX] == FLOOR) {
                dungeon[currentFloor][randY][randX] = COLUMN;
            }
        }
    }
}

// Function to initialize gold in room
void addGold() {
    for (int i = 0; i < roomCount - 1; i++) {
        int goldCount = rand() % 4;

        Room currentRoom = rooms[i];

        int x1 = currentRoom.x_min;
        int x2 = currentRoom.x_max;
        int y1 = currentRoom.y_min;
        int y2 = currentRoom.y_max;

        for (int j = 0; j < goldCount; j++) {
            int randX = x1 + 1 + rand() % (x2 - x1 - 2);
            int randY = y1 + 1 + rand() % (y2 - y1 - 2);

            if (dungeon[currentFloor][randY][randX] == FLOOR) {
                dungeon[currentFloor][randY][randX] = GOLD;
            }
        }

        int blackGoldCount = rand() % 2;

        for (int j = 0; j < blackGoldCount; j++) {
            int randX = x1 + 1 + rand() % (x2 - x1 - 2);
            int randY = y1 + 1 + rand() % (y2 - y1 - 2);

            if (dungeon[currentFloor][randY][randX] == FLOOR) {
                dungeon[currentFloor][randY][randX] = BLACK_GOLD;
            }
        }
    }
}

// Function to initialize gold in room
void addTraps() {
    for (int i = 0; i < roomCount - 1; i++) {
        int trapCount = rand() % 3;

        Room currentRoom = rooms[i];

        int x1 = currentRoom.x_min;
        int x2 = currentRoom.x_max;
        int y1 = currentRoom.y_min;
        int y2 = currentRoom.y_max;

        for (int j = 0; j < trapCount; j++) {
            int randX = x1 + 1 + rand() % (x2 - x1 - 2);
            int randY = y1 + 1 + rand() % (y2 - y1 - 2);

            if (dungeon[currentFloor][randY][randX] == FLOOR) {
                dungeon[currentFloor][randY][randX] = TRAP;
            }
        }
    }
}
