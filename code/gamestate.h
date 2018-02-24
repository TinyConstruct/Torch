/////// GAME CONSTANTS //////////////////////
//Tiles are 1 meter on both sides
global_variable float tileSideLength = 1.0f;

//Game is meant for 16:10 aspect ratio (eg 1280 x 800)
global_variable float numVerticalTiles = 12.50f;
global_variable float numHorizontalTiles = 20.0f;
global_variable float playerSecondsPerTile = .15f;
global_variable float mobSecondsPerTile = .15f;
global_variable float idleFlipTime = .75f;

/////// ENTITY STRUCTURES ///////////////////

enum Facing {FACING_RIGHT = 0, FACING_DOWN = 1,FACING_UP = 2, FACING_LEFT =3};
enum EntityType {E_PLAYER=1, E_FIGHTER, E_CAT, GOBLIN_SWORD,};
enum PlayerState {PS_MOVING, PS_DEFENDING, PS_WAITING};
enum EntityState {ES_MOVING, ES_DEFENDING, ES_WAITING, ES_HUNTING};
enum TileTypes {
  TILE_FLOOR, TILE_WALL, TILE_TOP_WALL, TILE_SIDE_WALL,
};
enum Turn {PLAYER_TURN, MONSTER_TURN,};
struct GameObject {
  float width, height;
  v2 center;
};

struct Player {
  int type;
  float hp;
  v2 center;
  v2 origin, destination;
  float moveDuration;
  float uvX, uvY;
  int classType;
  int tileX, tileY;
  int facing;
  int playerState;
};

struct Entity {
  int type;
  float hp;
  float centerX, centerY;
  v2 origin, destination;   
  float moveDuration;
  float uvX, uvY;
  int tileX, tileY;
  int facing;
  int state;
  float lastSawPlayerX, lastSawPlayerY;
};

struct Tile {
  int32 x, y;
  int type;
  Entity* entityHere;
};

struct GameState {
  v2 cameraCenter;
  int turn;
  int maxTilesX;
  int maxTilesY;
  Tile* tiles;
  bool flipSprites;
  float spriteSet;
};

struct EntityGroup {
  int count, max;
  Entity* base;
};

global_variable EntityGroup mobEntities;
global_variable GameState gameState;
global_variable Player player;