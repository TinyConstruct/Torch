/////// GAME CONSTANTS //////////////////////
//Tiles are 1 meter on both sides
global_variable float tileSideLength = 1.0f;

//Game is meant for 16:10 aspect ratio (eg 1280 x 800)
global_variable float numVerticalTiles = 12.50f;
global_variable float numHorizontalTiles = 20.0f;
global_variable float playerSecondsPerTile = .15f;
global_variable float idleFlipTime = .75f;

/////// ENTITY STRUCTURES ///////////////////

enum Facing {FACING_RIGHT = 0, FACING_DOWN = 1,FACING_UP = 2, FACING_LEFT =3};
enum EntityType {E_FIGHTER, E_CAT, };
struct GameObject {
  float width, height;
  v2 center;
};

struct Player {
  float hp;
  v2 center;
  v2 origin, destination;
  float moveDuration;
  float uvX, uvY;
  int classType;
  int tileX, tileY;
  int facing;
  bool moving;
};

enum TileTypes {
  TILE_FLOOR, TILE_WALL, TILE_TOP_WALL, TILE_SIDE_WALL,
};

struct Tile {
  int32 x, y;
  int type;
};

struct GameState {
  v2 cameraCenter;
  int maxTilesX;
  int maxTilesY;
  Tile* tiles;
  bool flipSprites;
  float spriteSet;
};

struct Entity {
  int type, facing;
  float centerX, centerY;
  float uvX, uvY;
};


struct EntityGroup {
  int count, max;
  Entity* base;
};

EntityGroup mobEntities;
GameState gameState;
Player player;
