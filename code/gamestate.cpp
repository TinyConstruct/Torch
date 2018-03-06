#include "gamestate.h"
inline bool tileIsEmpty(Tile* t) {
  return(!t->entityHere);
}

inline void addTile(int x, int y, int type) {
  int location = x + y*gameState.maxTilesX;
  Tile* t = gameState.tiles + location;
  t->type = type;
  t->entityHere = NULL;
  t->navListNum = 0;
}

inline Tile* getTile(Tile* ptr, int x, int y) {
  int location = x + y*gameState.maxTilesX;
  return ptr + location;
}

inline int getTileInt(int x, int y) {
  return x + y*gameState.maxTilesX;
}

inline v2 getTileXY(int t) {
  v2 v;
  Tile* tile = gameState.tiles + t;
  v.x = tile->x;
  v.y = tile->y;
  return v;
}

inline Tile* getTileFromInt(int t) {
  return gameState.tiles + t;
}

inline int getTileEntityType(Tile* ptr, int x, int y) {
  int location = x + y*gameState.maxTilesX;
  return ((Entity*) ((ptr + location)->entityHere))->type;
}

void tuneTileSprites(GameState* state) {
  Tile* currentTile = state->tiles;
  for (int y = 0; y < state->maxTilesY; y++) {
    for (int x = 0; x < state->maxTilesX; x++) {
      switch(currentTile->type) {
        case TILE_WALL: { 
            if(y==0) {
              currentTile->type = TILE_TOP_WALL;
            }
            else if(x == 0 || x == state->maxTilesX-1) {
              currentTile->type = TILE_SIDE_WALL;
            }
            else if (y == state->maxTilesY - 1 && (getTile(state->tiles, x, y - 1))->type == TILE_FLOOR) {
             currentTile->type = TILE_TOP_WALL; 
            }
            else {
              int below = (getTile(state->tiles,x,y-1))->type;
              int above = getTile(state->tiles,x,y+1)->type;
              if(below==TILE_TOP_WALL || below == TILE_SIDE_WALL) {
                currentTile->type = TILE_SIDE_WALL;
              }
              else {
                currentTile->type = TILE_TOP_WALL;
              }
            }
          break;}
        default:
          break;
      }
      currentTile++;
    }
  }
}

inline int manhattanCostToTile(int from, int to) {
  v2 f = getTileXY(from);
  v2 t = getTileXY(to);
  return abs(f.x - t.x) + abs(f.y - t.y);
}

inline int manhattanCostToTile(int fromX, int fromY, int toX, int toY) {
  return abs(fromX - toX) + abs(fromY - toY);
}

int getTileCrossingCost(Tile* t) {
  switch(t->type) {
    case TILE_WALL:
    case TILE_TOP_WALL:
    case TILE_SIDE_WALL:
      return -1; break;
    default:
      return 1; break;
  }
}

inline void initializeTileMinHeap(TileMinHeap* heap, int max, TileMinHeapNode* ptr) {
  heap->max = max;
  heap->head = ptr;
  heap->count = 0;
}

void swap(TileMinHeapNode* a, TileMinHeapNode* b) {
  TileMinHeapNode temp;
  temp.tile = a->tile;
  temp.costFromStart = a->costFromStart;
  temp.hCost = a->hCost;
  temp.dir = a->dir;
  a->tile = b->tile;
  a->costFromStart = b->costFromStart;
  a->hCost = b->hCost;
  a->dir = b->dir;
  b->tile = temp.tile;
  b->costFromStart = temp.costFromStart;
  b->hCost = temp.hCost;
  b->dir = temp.dir;
}

void insertTileMinHeap(TileMinHeap* heap, int hCost, int costFromStart, Tile* tile, int dir) {
  heap->count++;
  if(heap->count <= heap->max){
    int pos = heap->count;
    TileMinHeapNode* curPtr = heap->head + pos; //insert new node onto end, then compare with parents
    curPtr->costFromStart = costFromStart;
    curPtr->hCost = hCost;
    curPtr->dir = dir;
    curPtr->tile = tile;
    pos = pos/2;
    TileMinHeapNode* parentPtr = heap->head + pos;
    while (pos != 0 && (parentPtr->costFromStart + parentPtr->hCost) > (costFromStart + hCost)){
      swap(curPtr, parentPtr);
      curPtr = heap->head + pos;
      pos = pos/2;
      parentPtr = heap->head + pos;
    }
  }
  else{InvalidCodePath;}
}

void minHeapify(TileMinHeap* heap, int i) {
  int leftChildOffset = i*2;
  int rightChildOffset = i*2+1;
  int smallestIndex = 0;
  TileMinHeapNode* rootNode = heap->head + i;
  TileMinHeapNode* leftChild = heap->head + leftChildOffset;
  TileMinHeapNode* rightChild = leftChild+1;
  TileMinHeapNode* smallest = NULL; 
  if(leftChildOffset <= heap->count && (leftChild->costFromStart + leftChild->hCost) < 
      rootNode->costFromStart + rootNode->hCost) {
    smallest = leftChild;
    smallestIndex = leftChildOffset;
  }
  else{
    smallest = rootNode;
  }
  if(rightChildOffset <= heap->count && (rightChild->costFromStart + rightChild->hCost) < 
      rootNode->costFromStart + rootNode->hCost) {
    smallest = rightChild;
    smallestIndex = rightChildOffset;
  }
  if(smallest != rootNode) {
    swap(smallest, rootNode);
    minHeapify(heap, smallestIndex);
  }
}

TileMinHeapNode* popTileMinHeap(TileMinHeap* heap) {
  if(heap->count > 1) {
    TileMinHeapNode* root = (heap->head + 1);
    TileMinHeapNode* leaf = heap->head + heap->count;
    swap(root, leaf);
    heap->count--;
    minHeapify(heap, 1);
    return leaf;
  }
  else if (heap->count == 1) {
    TileMinHeapNode* root = (heap->head + 1);
    heap->count--;
    return root;
  }
  else{
    return NULL;
  }
}

void initializeEntityGroup(EntityGroup* group, int numEntities) {
  group->base = (Entity*) VirtualAlloc(0, sizeof(Entity)*numEntities, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  assert(group->base != 0);
  group->count = 0;
  group->max = numEntities;
}

void setPlayerDestination(Player* playerPtr, int x, int y) {
  int type = (getTile(gameState.tiles, x, y))->type;
  playerPtr->playerState = PS_MOVING;
  playerPtr->destination = V2(((float)x) + 0.5f, ((float)y) + 0.5f);
  playerPtr->moveDuration = 0.0f;
}

void setMobDestination(Entity* entPtr, int x, int y) {
  entPtr->state = ES_MOVING;
  entPtr->destination = V2(((float)x) + 0.5f, ((float)y) + 0.5f);
  entPtr->moveDuration = 0.0f;
}

void updatePlayerMovement(Player* playerPtr, float dt) {
  //move player to destination
  if(playerPtr->playerState == PS_MOVING) {
    playerPtr->moveDuration += dt;
    float movePercentage =  playerPtr->moveDuration / playerSecondsPerTile;
    float yJumpOffset = 0.0f;
    if(playerPtr->facing==FACING_LEFT || playerPtr->facing==FACING_RIGHT) {
      yJumpOffset = 0.5f * sinf(movePercentage * M_PI_2);
    }
    if(movePercentage >= 1.0f) {
      gameState.turn = MONSTER_TURN;
      playerPtr->playerState = PS_DEFENDING;
      playerPtr->origin.x = playerPtr->destination.x;
      playerPtr->origin.y = playerPtr->destination.y;
      playerPtr->moveDuration = 0.0f;
      playerPtr->center.x = playerPtr->origin.x;
      playerPtr->center.y = playerPtr->origin.y;
      (getTile(gameState.tiles, playerPtr->tileX, playerPtr->tileY))->entityHere = NULL;
      playerPtr->tileX = (int) playerPtr->origin.x;
      playerPtr->tileY = (int) playerPtr->origin.y;
      (getTile(gameState.tiles, playerPtr->tileX, playerPtr->tileY))->entityHere = (Entity*) playerPtr;
    }
    else{
      playerPtr->center.x = (1.0f - movePercentage) * playerPtr->origin.x + movePercentage*playerPtr->destination.x;
      playerPtr->center.y = (1.0f - movePercentage) * playerPtr->origin.y + movePercentage*playerPtr->destination.y + yJumpOffset;
    }
  }
}

void updateMobMovement(Entity* entPtr, float dt) {
  entPtr->moveDuration += dt;
  float movePercentage =  entPtr->moveDuration / mobSecondsPerTile;
  float yJumpOffset = 0.0f;
  if(entPtr->facing==FACING_LEFT || entPtr->facing==FACING_RIGHT) {
    yJumpOffset = 0.5f * sinf(movePercentage * M_PI_2);
  }
  if(movePercentage >= 1.0f) {
    entPtr->state = ES_WAITING;
    entPtr->origin.x = entPtr->destination.x;
    entPtr->origin.y = entPtr->destination.y;
    entPtr->moveDuration = 0.0f;
    entPtr->centerX = entPtr->origin.x;
    entPtr->centerY = entPtr->origin.y;
    (getTile(gameState.tiles, entPtr->tileX, entPtr->tileY))->entityHere = NULL;
    entPtr->tileX = (int) entPtr->origin.x;
    entPtr->tileY = (int) entPtr->origin.y;
    (getTile(gameState.tiles, entPtr->tileX, entPtr->tileY))->entityHere = (Entity*) entPtr;
  }
  else{
    entPtr->centerX = (1.0f - movePercentage) * entPtr->origin.x + movePercentage*entPtr->destination.x;
    entPtr->centerY = (1.0f - movePercentage) * entPtr->origin.y + movePercentage*entPtr->destination.y + yJumpOffset;
  }
}

void initPlayer(int x, int y) {
  player.type = E_PLAYER;
  player.center = V2(x + .5f, y + .5f);
  player.origin = V2(player.center.x, player.center.y);
  player.origin = V2(player.center.x, player.center.y);
  player.moveDuration = 0.0f;
  player.classType = E_FIGHTER;
  player.tileX = (int) player.center.x;
  player.tileY = (int) player.center.y;
  player.facing = FACING_DOWN;
  player.playerState = PS_WAITING;
}

void initializeGameState() {
  gameState.cameraCenter.x = 0;
  gameState.cameraCenter.y = 0;
  gameState.turn = PLAYER_TURN;
  gameState.maxTilesX = 7;
  gameState.maxTilesY = 7;
  gameState.tiles = (Tile*)VirtualAlloc(0, gameState.maxTilesX*gameState.maxTilesX*sizeof(Tile),
   MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  assert(gameState.tiles!=NULL);
  gameState.flipSprites = false;
  gameState.spriteSet = 0.0f;

  initPlayer(2,2);

  Tile* tile = gameState.tiles; 
  for(int i = 0; i < gameState.maxTilesY; i++) {
    for(int j = 0; j < gameState.maxTilesX; j++) {
      tile->x = j;
      tile->y = i;
      if(i==0 || i == gameState.maxTilesX-1 ) {
        addTile(j,i,TILE_WALL);
      }
      else if (j==0 || j == gameState.maxTilesY-1) {
        addTile(j,i,TILE_WALL);
      }
      else{      
        addTile(j,i,TILE_FLOOR);
      }
      tile++;
    }     
  }
  (getTile(gameState.tiles, player.tileX, player.tileY))->entityHere = (Entity*) &player;
  addTile(3,6,TILE_WALL);
  addTile(3,5,TILE_WALL);
  addTile(3,4,TILE_WALL);
  addTile(3,3,TILE_WALL);
  addTile(3,2,TILE_WALL);
  tuneTileSprites(&gameState);
}