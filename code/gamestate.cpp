#include "gamestate.h"

inline void addTile(int x, int y, int type) {
  int location = x + y*gameState.maxTilesX;
  Tile* t = gameState.tiles + location;
  t->type = type;
}

inline Tile* getTile(Tile* ptr, int x, int y) {
  int location = x + y*gameState.maxTilesX;
  return ptr + location;
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
            else if (y == state->maxTilesY - 1) {
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




void initializeEntityGroup(EntityGroup* group, int numEntities) {
  group->base = (Entity*) VirtualAlloc(0, sizeof(Entity)*numEntities, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  assert(group->base != 0);
  group->count = 0;
  group->max = numEntities;
}

void setPlayerDestination(Player* playerPtr, int x, int y) {
  int type = (getTile(gameState.tiles, x, y))->type;
  if(type != TILE_SIDE_WALL && type != TILE_TOP_WALL) {
    playerPtr->moving = true;
    playerPtr->destination = V2(((float)x) + 0.5f, ((float)y) + 0.5f);
    playerPtr->moveDuration = 0.0f;
    playerPtr->tileX = x;
    playerPtr->tileY = y;
  }
}

void updatePlayerMovement(Player* playerPtr, float dt) {
  //move player to destination
  if(playerPtr->moving) {
    playerPtr->moveDuration += dt;
    float movePercentage =  playerPtr->moveDuration / playerSecondsPerTile;
    if(movePercentage >= 1.0f) {
      playerPtr->moving = false;
      playerPtr->origin.x = playerPtr->destination.x;
      playerPtr->origin.y = playerPtr->destination.y;
      playerPtr->moveDuration = 0.0f;
      playerPtr->center.x = playerPtr->origin.x;
      playerPtr->center.y = playerPtr->origin.y;
    }
    else{
      playerPtr->center.x = (1.0f - movePercentage) * playerPtr->origin.x + movePercentage*playerPtr->destination.x;
      playerPtr->center.y = (1.0f - movePercentage) * playerPtr->origin.y + movePercentage*playerPtr->destination.y;
    }
  }
}

void initializeGameState() {
  gameState.cameraCenter.x = 0;
  gameState.cameraCenter.y = 0;
  gameState.maxTilesX = 8;
  gameState.maxTilesY = 8;
  gameState.tiles = (Tile*)VirtualAlloc(0, gameState.maxTilesX*gameState.maxTilesX*sizeof(Tile),
   MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  assert(gameState.tiles!=NULL);
  gameState.flipSprites = false;
  gameState.spriteSet = 0.0f;

  player.center = V2(2.5f,2.5f);
  player.origin = V2(player.center.x, player.center.y);
  player.origin = V2(player.center.x, player.center.y);
  player.moveDuration = 0.0f;
  player.classType = E_FIGHTER;
  player.tileX = (int) player.center.x;
  player.tileY = (int) player.center.y;
  player.facing = FACING_DOWN;
  player.moving = false;

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
  addTile(3,3,TILE_WALL);
  addTile(3,4,TILE_WALL);
  addTile(4,4,TILE_WALL);
  tuneTileSprites(&gameState);
}