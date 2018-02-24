#include "gamestate.h"
inline bool tileIsEmpty(Tile* t) {
  return(!t->entityHere);
}

inline void addTile(int x, int y, int type) {
  int location = x + y*gameState.maxTilesX;
  Tile* t = gameState.tiles + location;
  t->type = type;
  t->entityHere = NULL;
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
  gameState.maxTilesX = 15;
  gameState.maxTilesY = 15;
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
  //addTile(3,3,TILE_WALL);
  //addTile(3,4,TILE_WALL);
  //addTile(4,4,TILE_WALL);

  (getTile(gameState.tiles, player.tileX, player.tileY))->entityHere = (Entity*) &player;
  
  tuneTileSprites(&gameState);
}