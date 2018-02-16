//Tiles are square, conceptually a meter on a side
#define TILE_UV_OFFSET 16.0f/1024.0f
#define TILE_SPACE_CENTER_OFFSET .5

struct Texture {
  GLuint glTextureNum;
  Bitmap data;
};

struct Sprite {
  float upperLUV, upperRUV, lowerLUV, lowerRUV;
  p3 upperL, upperR, lowerL, lowerR;
};

//Intent: The float array is used to hold quad data, so (x,y,z,s,t)*4 per entity
struct RenderGroup {
  uint32 currentEntityCount, maxEntityCount; 
  GLint vertArrayMaxSize, indexArrayMaxSize, vertArrayCurrentSize, indexArrayCurrentSize;
  float* vertBase;
  GLuint* indexBase;
  GLuint vbo, ebo;
  GLint posAttrib, texAttrib;
};

global_variable Texture atlas = {};
global_variable RenderGroup mobSprites = {};
global_variable RenderGroup tileSprites = {};
global_variable RenderGroup playerSprite = {};

const char* vertexSource = R"FOO(
#version 150 core
in vec3 position;
in vec2 texcoordIn;

uniform mat4 projection;
uniform mat4 model;


out vec2 texcoordOutFromVert;

void main()
{
  gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0f);
  texcoordOutFromVert = vec2(texcoordIn.x, texcoordIn.y);
})FOO";


const char* fragmentSource = R"FOO(
#version 150 core
in vec2 texcoordOutFromVert;
out vec4 outColor;

uniform sampler2D tex;
//Todo: oh no an if statement; if things are drawn in order toward the camera this isn't needed, 
//so if monsters never collide with one another and are always in front of and drawn after the player. 
//for now, this makes things Just Work. 
void main()
{
  vec4 texel = texture(tex, texcoordOutFromVert);
  if(texel.w < .2) {
    discard;
  } 
  outColor = texel;
}
)FOO";
