#include "simulation.h"



void initializeMobs() {
  createEntity(&mobEntities, &mobSprites, E_FIGHTER, 5,5);
  createEntity(&mobEntities, &mobSprites, E_FIGHTER, 5,6);
  createEntity(&mobEntities, &mobSprites, E_FIGHTER, 5,7);
}

inline int manhattanCostToTile(int from, int to) {
  v2 f = getTileXY(from);
  v2 t = getTileXY(to);
  return abs(f.x - t.x) + abs(f.y - t.y);
}


int AStarToTile(Entity* start, Tile* target) { 
  //Assumption: the max tile dimension is always a wall, and therefore assumed blocked.
  //int start = getTileInt(start->x, start->y);
  //int end = getTileInt(target->x, target->y);
  return 0;
}

void mobUpdate(EntityGroup* mobEntities, Player* player, float) {
  if(player->playerState == PS_DEFENDING) {
    for(int i = 0; i < mobEntities->max; i++) {
      Entity* mob = mobEntities->base + i;
      //TODO: if mob can see player, set last seen
      //TODO: regardless of whether can see player, set dest to lastSeen if it's not negative
      //TODO: run A* to find tile that is next step toward player
    }
    player->playerState = PS_WAITING;
  }
}

void initializeSim() {
  simulationSwapSpace.space = VirtualAlloc(0, megabyte(5),
   MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  simulationSwapSpace.sizeInBytes = megabyte(5);
  assert(simulationSwapSpace.space != NULL);
}