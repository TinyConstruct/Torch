#include "rendering.h"
GLuint shaderProgram;

//Returns the upper-left texture coordinate for a sprite (indexed where 0,0 is the top left)
inline v2 getSpriteUVFromIndex(float x, float y) {
  v2 result;
  result.y = (1024.0f - y*16.0f)/1024.0f;
  result.x = (0.0f + x*16.0f)/1024.0f;
  return result;
}

GLuint loadTexture(Texture* texture, char* bmpPath) {
  GLuint texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  texture->data = loadBMP(bmpPath);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->data.width, texture->data.height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, texture->data.contents);
  if(!freeBMP(&texture->data)) {
    InvalidCodePath; //Failed to free bitmap
  }
  return texID;
}

void loadAssets() {
  atlas.glTextureNum = loadTexture(&atlas, "atlas.bmp");
}

void initializeQuadRenderGroup(RenderGroup* buffer, int spriteCapacity) {
  buffer->vertArrayMaxSize = spriteCapacity*20*sizeof(float);
  buffer->indexArrayMaxSize = sizeof(GLuint)*6*spriteCapacity;
  buffer->vertBase = (float*)VirtualAlloc(0, buffer->vertArrayMaxSize,
   MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  assert(buffer->vertBase != NULL);
  buffer->indexBase = (GLuint*) VirtualAlloc(0, buffer->indexArrayMaxSize,
   MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  assert(buffer->indexBase != NULL);
  buffer->maxEntityCount = spriteCapacity;
  buffer->indexArrayCurrentSize = 0;
  buffer->vertArrayCurrentSize = 0;
}

void updateQuadRenderGroupSprites(RenderGroup* sprites, EntityGroup* entities) {
  /*Andy: For now, let's commit to doing translate/rotates on the CPU. I'd rather have one long
    entity sprite buffer whose vertices are always updated every frame than many draw calls. 
    I honestly do not know if this is the "correct" choice! */
  //int offset = 0;
  float* writePointer = sprites->vertBase;
  Entity* entityPointer = entities->base;
  float spriteSet = gameState.spriteSet;
  if(gameState.flipSprites) {
    //If idle sprites need to be flipped, flip them
    if(gameState.spriteSet == 0.0f){
      spriteSet = 1.0f;
    }
    else {
      spriteSet = 0.0f;
    }
  }
  assert(sprites->currentEntityCount == entities->count);
  int quadByteOffset = sizeof(float)*20;
  float* quadStart;
  for(int i = 0; i < entities->count; i++) {
    float facingOffset = ((float)entityPointer->facing)*TILE_UV_OFFSET;
    quadStart = writePointer;
    //upper-left vert
    *writePointer = entityPointer->centerX - TILE_SPACE_CENTER_OFFSET;
    writePointer++;
    *writePointer = entityPointer->centerY + TILE_SPACE_CENTER_OFFSET ;
    writePointer++;
    //*writePointer = z;
    writePointer++;
    *writePointer = entityPointer->uvX + facingOffset;
    writePointer++;
    *writePointer = entityPointer->uvY - TILE_UV_OFFSET*spriteSet;
    assert(*writePointer > 0 && *writePointer <=1.0);
    writePointer++;

    //upper-right vert
    *writePointer = entityPointer->centerX + TILE_SPACE_CENTER_OFFSET;
    writePointer++;
    *writePointer = entityPointer->centerY + TILE_SPACE_CENTER_OFFSET;
    writePointer++;
    //*writePointer = z;
    writePointer++;
    *writePointer = entityPointer->uvX + facingOffset + TILE_UV_OFFSET;
    writePointer++;
    *writePointer = entityPointer->uvY - TILE_UV_OFFSET*spriteSet;
    assert(*writePointer > 0 && *writePointer <=1.0);
    writePointer++;

    //lower-left vert
    *writePointer = entityPointer->centerX - TILE_SPACE_CENTER_OFFSET;
    writePointer++;
    *writePointer = entityPointer->centerY - TILE_SPACE_CENTER_OFFSET;
    writePointer++;
    //*writePointer = z;
    writePointer++;
    *writePointer = entityPointer->uvX + facingOffset;
    writePointer++;
    *writePointer = entityPointer->uvY - TILE_UV_OFFSET*spriteSet - TILE_UV_OFFSET;
    assert(*writePointer > 0 && *writePointer <=1.0);
    writePointer++;

    //lower-right vert
    *writePointer = entityPointer->centerX + TILE_SPACE_CENTER_OFFSET;
    writePointer++;
    *writePointer = entityPointer->centerY - TILE_SPACE_CENTER_OFFSET;
    writePointer++;
    //*writePointer = z;
    writePointer++;
    *writePointer = entityPointer->uvX + facingOffset + TILE_UV_OFFSET;
    writePointer++;
    *writePointer = entityPointer->uvY - TILE_UV_OFFSET*spriteSet - TILE_UV_OFFSET;
    assert(*writePointer > 0 && *writePointer <=1.0);
    writePointer++;    
    entityPointer++;
    //TODO: make whole buffer update at once? Or is this asynchronous in any way?
    glBufferSubData(GL_ARRAY_BUFFER, i*quadByteOffset , quadByteOffset, quadStart);
  }
}

void updatePlayerSprite(RenderGroup* spritePtr, Player* playerPtr) {
  float* writePointer = spritePtr->vertBase;
  float spriteSet = gameState.spriteSet;
  if(gameState.flipSprites) {
    //If idle sprite needs to be flipped, flip it
    if(gameState.spriteSet == 0.0f){
      spriteSet = 1.0f;
    }
    else {
      spriteSet = 0.0f;
    }
  }
  int quadByteOffset = sizeof(float)*20;
  float* quadStart;
  float facingOffset = ((float)playerPtr->facing)*TILE_UV_OFFSET;
  quadStart = writePointer;
  float playerCenterX = playerPtr->center.x;
  float playerCenterY = playerPtr->center.y;
  float uvX = playerPtr->uvX;
  float uvY = playerPtr->uvY;
  //upper-left vert
  *writePointer = playerCenterX - TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = playerCenterY + TILE_SPACE_CENTER_OFFSET ;
  writePointer++;
  //*writePointer = z;
  writePointer++;
  *writePointer = uvX + facingOffset;
  writePointer++;
  *writePointer = uvY - TILE_UV_OFFSET*spriteSet;
  writePointer++;

  //upper-right vert
  *writePointer = playerCenterX + TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = playerCenterY + TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  //*writePointer = z;
  writePointer++;
  *writePointer = uvX + facingOffset + TILE_UV_OFFSET;
  writePointer++;
  *writePointer = uvY - TILE_UV_OFFSET*spriteSet;
  writePointer++;
  
  //lower-left vert
  *writePointer = playerCenterX - TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = playerCenterY - TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  //*writePointer = z;
  writePointer++;
  *writePointer = uvX + facingOffset;
  writePointer++;
  *writePointer = uvY - TILE_UV_OFFSET*spriteSet - TILE_UV_OFFSET;
  writePointer++;
  
  //lower-right vert
  *writePointer = playerCenterX + TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = playerCenterY - TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  //*writePointer = z;
  writePointer++;
  *writePointer = uvX + facingOffset + TILE_UV_OFFSET;
  writePointer++;
  *writePointer = uvY - TILE_UV_OFFSET*spriteSet - TILE_UV_OFFSET;
  writePointer++;    
  glBufferSubData(GL_ARRAY_BUFFER, 0, quadByteOffset, quadStart);
}



float* addQuadToRenderGroup(RenderGroup* group, float x, float y, float z, float s, float t) {
  uint32 count = group->currentEntityCount;
  if(count + 1 > group->maxEntityCount) {
    InvalidCodePath;
  }
  uint32 vertOffset = count*20;
  float* returnPointer = &group->vertBase[vertOffset];
  float* writePointer = returnPointer;

  //upper-left vert
  *writePointer = x - TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = y + TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = z;
  writePointer++;
  *writePointer = s;
  writePointer++;
  *writePointer = t;
  writePointer++;
//upper-right vert
  *writePointer = x + TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = y + TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = z;
  writePointer++;
  *writePointer = s + TILE_UV_OFFSET;
  writePointer++;
  *writePointer = t;
  writePointer++;
//lower-left vert
  *writePointer = x - TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = y - TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = z;
  writePointer++;
  *writePointer = s;
  writePointer++;
  *writePointer = t - TILE_UV_OFFSET;
  writePointer++;
//lower-right vert
  *writePointer = x + TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = y - TILE_SPACE_CENTER_OFFSET;
  writePointer++;
  *writePointer = z;
  writePointer++;
  *writePointer = s + TILE_UV_OFFSET;
  writePointer++;
  *writePointer = t - TILE_UV_OFFSET;
  writePointer++;    

  uint32 indexOffset = count*4;
  GLuint* indexWritePointer = &group->indexBase[count*6];
  
  *indexWritePointer = indexOffset;
  indexWritePointer++;
  *indexWritePointer = indexOffset+2;
  indexWritePointer++;
  *indexWritePointer = indexOffset+1;
  indexWritePointer++;
  *indexWritePointer = indexOffset+1;
  indexWritePointer++;
  *indexWritePointer = indexOffset+2;
  indexWritePointer++;
  *indexWritePointer = indexOffset+3;

  group->currentEntityCount += 1;
  group->vertArrayCurrentSize += 20;
  group->indexArrayCurrentSize += 6;

  return returnPointer;
}

v2 tileTypeToUpperUV(int type) {
  int i = rand() % 4;
  switch(type) {
    case TILE_FLOOR:
      return getSpriteUVFromIndex(19+i, 19);
      break;
    case TILE_TOP_WALL:
      return getSpriteUVFromIndex(19+i, 15);
    break;
    case TILE_SIDE_WALL:
      return getSpriteUVFromIndex(25+i,15);
    break;
    default: 
      break;
  }
}

v2 entityTypeToUpperUV(int type) {
  switch(type) {
    case E_FIGHTER: {
      return getSpriteUVFromIndex(0,0);
    } break;
    case E_CAT: {
      return getSpriteUVFromIndex(4,14);
    } break;
    default: {
      assert(1==1);
      return V2(0,0);
    } break;
  }
}

void createEntity(EntityGroup* entityGroup, RenderGroup* renderGroup, int entityType, float x, float y) {
  assert(entityGroup->count + 1 < entityGroup->max);
  Entity* e = entityGroup->base + entityGroup->count;
  e->type = entityType;
  e->facing = FACING_DOWN;
  e->centerX = x + TILE_SPACE_CENTER_OFFSET;
  e->centerY = y + TILE_SPACE_CENTER_OFFSET;
  v2 uv = entityTypeToUpperUV(entityType);
  e->uvX = uv.x;
  e->uvY = uv.y;
  entityGroup->count++;
  addQuadToRenderGroup(renderGroup, e->centerX, e->centerY, 0.5f, e->uvX, e->uvY);
}

void initializeRenderState() {
  //If our dungeon level is always a dense square, and it can be 100x100, and each square 
  //can have a tile plus the player/monster plus an item, that's 100*100*5*4 floats. Which
  //would never happen, but there's one hard limit on vertices.
  int maxEntitiesPerTile = 2;
  int maxTotalEntities = gameState.maxTilesX*gameState.maxTilesY*maxEntitiesPerTile;

  initializeQuadRenderGroup(&mobSprites, maxTotalEntities);
  initializeQuadRenderGroup(&tileSprites, gameState.maxTilesY*gameState.maxTilesX);
  initializeQuadRenderGroup(&playerSprite, 1);

  v2 uv = entityTypeToUpperUV(player.classType);
  addQuadToRenderGroup(&playerSprite, player.center.x, player.center.y, 0.5f, uv.x, uv.y);
  //createEntity(&mobEntities, &mobSprites, E_FIGHTER, 0,0);
  
  Tile* tile = gameState.tiles; 
  for(float i = 0.0f; i < gameState.maxTilesY; i++) {
    for(float j = 0.0f; j < gameState.maxTilesX; j++) {
      v2 uv = tileTypeToUpperUV(tile->type);
      addQuadToRenderGroup(&tileSprites, j + .5, i + .5, 0.0f, uv.x, uv.y);
      tile++;
    }     
  }

  wglSwapInterval(1);
  shaderProgram = compileShader(vertexSource, fragmentSource);
  
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &tileSprites.vbo); 
  glGenBuffers(1, &tileSprites.ebo);
  glBindBuffer(GL_ARRAY_BUFFER, tileSprites.vbo);
  glBufferData(GL_ARRAY_BUFFER, tileSprites.vertArrayMaxSize, tileSprites.vertBase, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tileSprites.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, tileSprites.indexArrayMaxSize, tileSprites.indexBase, GL_DYNAMIC_DRAW);
  tileSprites.posAttrib = glGetAttribLocation(shaderProgram, "position");
  tileSprites.texAttrib = glGetAttribLocation(shaderProgram, "texcoordIn");

  glGenBuffers(1, &mobSprites.vbo); 
  glGenBuffers(1, &mobSprites.ebo);
  glBindBuffer(GL_ARRAY_BUFFER, mobSprites.vbo);
  glBufferData(GL_ARRAY_BUFFER, mobSprites.vertArrayMaxSize, mobSprites.vertBase, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mobSprites.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mobSprites.indexArrayMaxSize, mobSprites.indexBase, GL_DYNAMIC_DRAW);
  mobSprites.posAttrib = glGetAttribLocation(shaderProgram, "position");
  mobSprites.texAttrib = glGetAttribLocation(shaderProgram, "texcoordIn");

  glGenBuffers(1, &playerSprite.vbo); 
  glGenBuffers(1, &playerSprite.ebo);
  glBindBuffer(GL_ARRAY_BUFFER, playerSprite.vbo);
  glBufferData(GL_ARRAY_BUFFER, playerSprite.vertArrayMaxSize, playerSprite.vertBase, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerSprite.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, playerSprite.indexArrayMaxSize, playerSprite.indexBase, GL_DYNAMIC_DRAW);
  playerSprite.posAttrib = glGetAttribLocation(shaderProgram, "position");
  playerSprite.texAttrib = glGetAttribLocation(shaderProgram, "texcoordIn");

  glUseProgram(shaderProgram);
  loadAssets();
  glEnable(GL_DEPTH_TEST);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, atlas.glTextureNum);
}

void frameUpdateCleanup() {
  if(gameState.flipSprites) {
    if(gameState.spriteSet == 0.0f){
      gameState.spriteSet = 1.0f;
    }
    else {
      gameState.spriteSet = 0.0f;
    }
    gameState.flipSprites = false;
  }
}