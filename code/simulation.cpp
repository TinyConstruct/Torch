#include "simulation.h"

void createEntity(EntityGroup* entityGroup, RenderGroup* renderGroup, int entityType, int x, int y) {
  assert(entityGroup->count + 1 < entityGroup->max);
  Entity* e = entityGroup->base + entityGroup->count;
  e->type = entityType;
  e->centerX = x + TILE_SPACE_CENTER_OFFSET;
  e->centerY = y + TILE_SPACE_CENTER_OFFSET;
  e->origin = V2(e->centerX, e->centerY);
  e->destination = V2(e->centerX, e->centerY);
  v2 uv = entityTypeToUpperUV(entityType);
  e->uvX = uv.x;
  e->uvY = uv.y;
  e->tileX = x;
  e->tileY = y;
  e->facing = FACING_DOWN;
  e->state = ES_DEFENDING;
  entityGroup->count++;
  addQuadToRenderGroup(renderGroup, e->centerX, e->centerY, 0.5f, e->uvX, e->uvY);
}

void initializeMobs() {
  createEntity(&mobEntities, &mobSprites, GOBLIN_SWORD, 5,5);
  //createEntity(&mobEntities, &mobSprites, GOBLIN_SWORD, 5,6);
}

bool monsterAttackCheck(Entity* mob, Tile** t) {
  Tile* temp = getTile(gameState.tiles, mob->tileX-1, mob->tileY);
  if(!tileIsEmpty(temp) && (temp->entityHere)->type == E_PLAYER) {
    *t = temp;
    return true;
  }
  temp = getTile(gameState.tiles, mob->tileX+1, mob->tileY);
  if(!tileIsEmpty(temp) && (temp->entityHere)->type == E_PLAYER) {
    *t = temp;
    return true;
  }
  temp = getTile(gameState.tiles, mob->tileX, mob->tileY-1);
  if(!tileIsEmpty(temp) && (temp->entityHere)->type == E_PLAYER) {
    *t = temp;
    return true;
  }
  temp = getTile(gameState.tiles, mob->tileX, mob->tileY+1);
  if(!tileIsEmpty(temp) && (temp->entityHere)->type == E_PLAYER) {
    *t = temp;
    return true;
  }
  return false;
}

v2i dirToUnitVec(int dir) {
  switch(dir) {
    case DIR_UP:
      return V2i(0,1); break;
    case DIR_DOWN:
      return V2i(0,-1); break;
    case DIR_LEFT:
      return V2i(1,0); break;
    case DIR_RIGHT:
      return V2i(1,0); break;
    default: 
      return V2i(0,0); break;
  }
}

inline void aStarListCheck(Tile* neighbor, int xTarget, int yTarget, int dir) {
  int neighborCrossCost =  getTileCrossingCost(neighbor);
  int neighborH = manhattanCostToTile(neighbor->x, neighbor->y, xTarget, yTarget);
  if(neighborCrossCost>=0 && neighbor->navListNum!=(gameState.currentAStarNum+1)){
    if(neighbor->navListNum!=gameState.currentAStarNum) {
      insertTileMinHeap(simMinHeap, neighborH, neighborCrossCost, neighbor, dir);
      neighbor->navListNum = gameState.currentAStarNum;
    }
    else {
      //TODO: fix this
    }
  }
}

v2i AStarToTile(Entity* start, int xTarget, int yTarget) { 
//Assumption: the max tile dimension is always a wall, and therefore assumed blocked.
  initializeTileMinHeap(simMinHeap, gameState.maxMinHeapNodes, (TileMinHeapNode*)&((TileMinHeap)*simMinHeap) + 1);
  Tile* targetTile = getTile(gameState.tiles, xTarget, yTarget);
  Tile* startTile = getTile(gameState.tiles, start->tileX, start->tileY);
  startTile->navListNum = gameState.currentAStarNum + 1; //add to closed list

  Tile* neighbor = getTile(gameState.tiles, start->tileX - 1, start->tileY);
  int neighborCrossCost =  getTileCrossingCost(neighbor);
  int neighborH = manhattanCostToTile(neighbor->x, neighbor->y, xTarget, yTarget);
  if(neighborCrossCost>=0){
    insertTileMinHeap(simMinHeap, neighborH, neighborCrossCost, neighbor, DIR_LEFT);
    neighbor->navListNum = gameState.currentAStarNum;
  }
  neighbor = getTile(gameState.tiles, start->tileX+1, start->tileY);
  neighborCrossCost =  getTileCrossingCost(neighbor);
  neighborH = manhattanCostToTile(neighbor->x, neighbor->y, xTarget, yTarget);
  if(neighborCrossCost>=0){
    insertTileMinHeap(simMinHeap, neighborH, neighborCrossCost, neighbor, DIR_RIGHT);
    neighbor->navListNum = gameState.currentAStarNum;
  }
  neighbor = getTile(gameState.tiles, start->tileX, start->tileY+1);
  neighborCrossCost =  getTileCrossingCost(neighbor);
  neighborH = manhattanCostToTile(neighbor->x, neighbor->y, xTarget, yTarget);
  if(neighborCrossCost>=0){
    insertTileMinHeap(simMinHeap, neighborH, neighborCrossCost, neighbor, DIR_UP);
    neighbor->navListNum = gameState.currentAStarNum;
  }
  neighbor = getTile(gameState.tiles, start->tileX-1, start->tileY);
  neighborCrossCost =  getTileCrossingCost(neighbor);
  neighborH = manhattanCostToTile(neighbor->x, neighbor->y, xTarget, yTarget);
  if(neighborCrossCost>=0){
    insertTileMinHeap(simMinHeap, neighborH, neighborCrossCost, neighbor, DIR_DOWN);
    neighbor->navListNum = gameState.currentAStarNum;
  }

  while(simMinHeap->count > 0) {
    TileMinHeapNode* least = popTileMinHeap(simMinHeap);
    least->tile->navListNum++;
    if(least->tile == targetTile) {
      return dirToUnitVec(least->dir);
      gameState.currentAStarNum += 2;
    }
    neighbor = getTile(gameState.tiles, start->tileX-1, start->tileY);
    aStarListCheck(neighbor, xTarget, yTarget, least->dir);
    neighbor = getTile(gameState.tiles, start->tileX+1, start->tileY);
    aStarListCheck(neighbor, xTarget, yTarget, least->dir);
    neighbor = getTile(gameState.tiles, start->tileX, start->tileY-1);
    aStarListCheck(neighbor, xTarget, yTarget, least->dir);
    neighbor = getTile(gameState.tiles, start->tileX, start->tileY+1);
    aStarListCheck(neighbor, xTarget, yTarget, least->dir);
  }
  gameState.currentAStarNum += 2;
  return V2i(0,0);
}

v2i naiveMove(Entity* start, int xTarget, int yTarget) { 
  //Assumption: the max tile dimension is always a wall, and therefore assumed blocked.
  //int start = getTileInt(start->x, start->y);
  //int end = getTileInt(target->x, target->y);
  int yDir = yTarget - start->tileY;
  int xDir = xTarget - start->tileX;
  if(abs(yDir) > abs(xDir)){
    yDir = clamp(yDir, -1, 1);
    Tile* t = getTile(gameState.tiles, start->tileX, start->tileY+yDir);
    v2i result = V2i(0, 0);
    if(t->type==TILE_FLOOR && t->entityHere == NULL) {
      result.y = yDir; 
      return result;
    }
    xDir = clamp(xDir, -1, 1);
    t = getTile(gameState.tiles, start->tileX + xDir, start->tileY);
    if(t->type==TILE_FLOOR && t->entityHere == NULL) {
      result.x = xDir; 
      return result;
    }
    return result;
  }
  else {
    xDir = clamp(xDir, -1, 1);
    Tile* t = getTile(gameState.tiles, start->tileX + xDir, start->tileY);
    v2i result = V2i(0, 0);
    if(t->type==TILE_FLOOR && t->entityHere == NULL) {
      result.x = xDir; 
      return result;
    }
    yDir = clamp(yDir, -1, 1);
    t = getTile(gameState.tiles, start->tileX, start->tileY+yDir);
    if(t->type==TILE_FLOOR && t->entityHere == NULL) {
      result.y = yDir; 
      return result;
    }
    return result;
  }
}

void setMobFacingByDir(Entity* ent, int x, int y) {
  switch(x) {
    case 1:
      ent->facing = FACING_RIGHT; 
      return;
    break;
    case -1:
      ent->facing = FACING_LEFT; 
      return;
    break;
  }
  switch(y) {
    case 1:
      ent->facing = FACING_UP; 
      return;
    break;
    case -1:
      ent->facing = FACING_DOWN; 
      return;
    break;
  }
}

void setMobFacingByDir(Entity* ent, v2i dir) {
  switch(dir.x) {
    case 1:
      ent->facing = FACING_RIGHT; 
      return;
    break;
    case -1:
      ent->facing = FACING_LEFT; 
      return;
    break;
  }
  switch(dir.y) {
    case 1:
      ent->facing = FACING_UP; 
      return;
    break;
    case -1:
      ent->facing = FACING_DOWN; 
      return;
    break;
  }
}

void attackPlayer(Entity* entPtr, Player* palyerPtr) {
  entPtr->state = ES_WAITING;
}

void mobUpdate(EntityGroup* mobEntities, Player* playerPtr, float dt) {
  if(gameState.turn == MONSTER_TURN) {
    bool mobsAllDone = true;
    Entity* mob;
    for(int i = 0; i < mobEntities->count; i++) {
      mob = mobEntities->base + i;
      Tile* tempTilePtr = NULL;
      switch(mob->state) {
        case ES_DEFENDING:  //ES_DEFENDING should only last for first frame of monster turn
          mobsAllDone = false;
          if(monsterAttackCheck(mob, &tempTilePtr)) {
            //attack!
            v2i dir;
            dir.x = tempTilePtr->x - mob->tileX;
            dir.y = tempTilePtr->y - mob->tileY;
            setMobFacingByDir(mob, dir);
            attackPlayer(mob, playerPtr);
          }
          else{
            //set move destination
            v2i dir = AStarToTile(mob, playerPtr->tileX, playerPtr->tileY);
            setMobFacingByDir(mob, dir.x, dir.y);
            char strTime[40];
            sprintf_s(strTime, 40, "dir:%i %i\n", dir.x, dir.y);
            OutputDebugString(strTime);
            setMobDestination(mob, mob->tileX + dir.x, mob->tileY + dir.y);
            updateMobMovement(mob, dt);
            //sprintf_s(strTime, 40, "monster tile:%i %i\n", mob->tileX, mob->tileY);
            //OutputDebugString(strTime);
            //sprintf_s(strTime, 40, "player tile:%i %i\n", playerPtr->tileX, playerPtr->tileY);
            //OutputDebugString(strTime);
          }
        break;
        case ES_MOVING:
          mobsAllDone = false;
          updateMobMovement(mob, dt);
        break;
        case ES_HUNTING:
          mobsAllDone = false;
        break;
        default:
        break;
      }
      //TODO: if mob can see player, set last seen
      //TODO: regardless of whether can see player, set dest to lastSeen if it's not negative
      //TODO: run A* to find tile that is next step toward player
    }
    if(mobsAllDone) {
      for(int i = 0; i < mobEntities->count; i++) {
        mob = mobEntities->base + i;
        mob->state = ES_DEFENDING;
      }
      gameState.turn = PLAYER_TURN;
      playerPtr->playerState = PS_WAITING;
    }
  }
  else { //Still the player's turn
    //do nothing
  }
}

bool tileIsWalkable(Tile* t) {
  switch(t->type) {
    case TILE_FLOOR:
      return true; break;
    default: 
      return false; break;
  }
}

void handlePlayerMove(Player* playerPtr, int x, int y) {
  Tile* targetTile = getTile(gameState.tiles, x, y);
  if(tileIsEmpty(targetTile) && tileIsWalkable(targetTile)) {
    setPlayerDestination(&player, x, y);
  }
}

void initializeSim() {
  simulationSwapSpace.space = VirtualAlloc(0, megabyte(5),
   MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  simulationSwapSpace.sizeInBytes = megabyte(5);
  assert(simulationSwapSpace.space != NULL);
  gameState.maxMinHeapNodes = gameState.maxTilesX*gameState.maxTilesY;
  gameState.currentAStarNum = 1;
  TileMinHeap* minHeap = (TileMinHeap*)simulationSwapSpace.space;
  int minHeapSize = sizeof(TileMinHeap) + sizeof(TileMinHeapNode)*minHeap->max;
  assert((minHeapSize - simulationSwapSpace.sizeInBytes) <= 0);

}