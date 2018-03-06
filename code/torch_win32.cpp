#include <Windows.h>
#include <stdint.h>
#include <stdio.h>

#include <gl/gl.h>
#include <math.h>

#include <intrin.h>
#include <immintrin.h>

#include <stdlib.h>
#include <time.h>

#include "aro_generic.h"
#include "torch_win32.h"
#include "aro_math.h"
#include "aro_platform_win32.cpp"
#include "aro_opengl.cpp"
#include "gamestate.cpp"
#include "rendering.cpp"
#include "simulation.cpp"

global_variable bool globalRunning;
global_variable Win32WindowDimensions globalWindowDimensions;


LRESULT CALLBACK
win32MainWindowCallback(HWND window, UINT message, WPARAM WParam, LPARAM LParam) {       
  LRESULT result = 0;
  if(message == WM_KEYDOWN && gameState.turn != MONSTER_TURN) {
    switch(WParam) {
      case 'W': {
        if(player.playerState==PS_WAITING){
          player.facing = FACING_UP;
          handlePlayerMove(&player, player.tileX, player.tileY+1);
        }
      } break;
      case 'A': {
        if(player.playerState==PS_WAITING){
          player.facing = FACING_LEFT;
          handlePlayerMove(&player, player.tileX-1, player.tileY);
        }
      } break;
      case 'S': {
        if(player.playerState==PS_WAITING){
          player.facing = FACING_DOWN;
          handlePlayerMove(&player, player.tileX, player.tileY-1);
        }
      } break;
      case 'D': {
        if(player.playerState==PS_WAITING){
          player.facing = FACING_RIGHT;
          handlePlayerMove(&player, player.tileX+1, player.tileY);
        }
      } break;
      case VK_ESCAPE: {
        globalRunning = false;
      } break;
      default: {
        return result;
      } break;
    }
  }
  else {
    switch(message)
    {
      case WM_CLOSE: {
        globalRunning = false;
      } break;
      case WM_ACTIVATEAPP: {
        if(WParam == WA_INACTIVE) {
          OutputDebugStringA("App sent to background\n");
        }
        else {
          OutputDebugStringA("App sent to foreground\n");
        }
      } break;
      case WM_DESTROY: {
        globalRunning = false;
      } break;
      case WM_PAINT: {
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(window, &Paint);
        globalWindowDimensions = win32GetWindowDimension(window);
        EndPaint(window, &Paint);
        glViewport(0, 0, globalWindowDimensions.width, globalWindowDimensions.height);
      } break;
      default: {
        result = DefWindowProc(window, message, WParam, LParam);
      } break;
    }
  }
  return(result);
}

int CALLBACK
WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode) {
  srand(time(NULL));
  WNDCLASS windowClass = {};
  windowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
  windowClass.lpfnWndProc = win32MainWindowCallback;
  windowClass.hInstance = instance;
  //  windowClass.hIcon = ;
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW); 
  //ShowCursor(FALSE);
  windowClass.lpszClassName = "Torch";
  if(RegisterClassA(&windowClass))
  {
  //drawable area 1280x800
    RECT clientRect;
    clientRect.left = 80;
    clientRect.right = clientRect.left + 1279;
    clientRect.top = 80;
    clientRect.bottom = clientRect.top + 799;
    if(!AdjustWindowRectEx(&clientRect, WS_OVERLAPPEDWINDOW|WS_VISIBLE, FALSE, NULL)){ 
      InvalidCodePath;
    }
    HWND window = CreateWindowExA(0, windowClass.lpszClassName, "Torch",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE, clientRect.left, clientRect.top,
        clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
        0, 0, instance, 0);
    if(window) {
      //TODO: make this not broken
      RECT mouseClipRect;           // new area for ClipCursor
      RECT oldMouseClipRect;        // previous area for ClipCursor
      // Record the area in which the cursor can move. 
      GetClipCursor(&oldMouseClipRect); 
      // Get the dimensions of the application's window. 
      GetWindowRect(window, &mouseClipRect); 
      // Confine the cursor to the application's window. 
      ClipCursor(&mouseClipRect); 
      // Restore the cursor to its previous area. 
      ClipCursor(&oldMouseClipRect); 

      win32InitOpenGL(window); //find a pixel format, wglCreateContext, etc.
      //All OpenGL functions are now loaded. 

      HDC dc = GetDC(window);
      globalRunning = true;

      loadAssets();
      initializeGameState();
      initializeEntityGroup(&mobEntities, 20);
      initializeRenderState();
      initializeSim();
      simMinHeap = (TileMinHeap*)simulationSwapSpace.space;
      simMinHeap->max = gameState.maxMinHeapNodes;
      simMinHeap->head = (TileMinHeapNode*) (((char*)simMinHeap) + sizeof(TileMinHeap));//   ((TileMinHeap)*simMinHeap) + 1;
      initializeMobs();

      LARGE_INTEGER lastCounter, counterFreq;
      char strTime[40];
      QueryPerformanceCounter(&lastCounter);
      QueryPerformanceFrequency(&counterFreq);

      float t = 0.0f;
      float spriteFlipCounter = 0.0f;


      while(globalRunning)
      {
        LARGE_INTEGER beginCounter;
        QueryPerformanceCounter(&beginCounter);
        //TODO: tracking short durations is okay...BUT LET'S NOT USE FLOATS FOR TOTAL GAME TIME THAT'S BAD
        float dif = beginCounter.QuadPart - lastCounter.QuadPart;
        float lastFrameMS = (1000 * dif) / (float)counterFreq.QuadPart;
        float lastFrameSec = dif/counterFreq.QuadPart;
        //sprintf_s(strTime, 40, "turn:%f\n", gameState.turn);
        //OutputDebugString(strTime);

        float fps = (float) counterFreq.QuadPart / (float)dif;
        //sprintf_s(strTime, 40, "fps:%f\n", fps);
        //OutputDebugString(strTime);
        lastCounter.QuadPart = beginCounter.QuadPart;

        //poll for keyboard input
        MSG message;
        while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
          if(message.message == WM_QUIT)
          {
            globalRunning = false;
          }
          TranslateMessage(&message);
          DispatchMessageA(&message);
        }

        updatePlayerMovement(&player, lastFrameSec);
        mobUpdate(&mobEntities, &player, lastFrameSec);

        //update sprite idle animations
        spriteFlipCounter += lastFrameSec;
        if(spriteFlipCounter > idleFlipTime) {
          spriteFlipCounter -= idleFlipTime;
          gameState.flipSprites = true;
        }

        updateQuadRenderGroupSprites(&mobSprites, &mobEntities);
        updatePlayerSprite(&playerSprite, &player);
        frameUpdateCleanup();
        
        glClearColor(1.0,1.0,0.0,1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        t += lastFrameSec/2.0f; 
        v3 xmov = V3(sin(t),0.0f,0.0f);
        
        /* Currently no use for a per-quad model matrix
        4x4 modelMat = identity();
        GLuint modelID = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelID, 1, GL_TRUE, (float*) modelMat.n);*/


        m4x4 projMat;
        float verticalCamOffset = numVerticalTiles/2.0f;
        float horizontalCamOffset = numHorizontalTiles/2.0f;
        gameState.cameraCenter.x = player.center.x;
        if(player.facing == FACING_RIGHT || player.facing == FACING_LEFT) {
          gameState.cameraCenter.y = player.destination.y;
        }
        else {
          gameState.cameraCenter.y = player.center.y;
        }
        getOrthoProjMatrix(gameState.cameraCenter.y - verticalCamOffset, gameState.cameraCenter.y + verticalCamOffset, 
          gameState.cameraCenter.x - horizontalCamOffset, gameState.cameraCenter.x + horizontalCamOffset, -4.0f, 4.0f, projMat);
        
        int projID = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projID, 1, GL_FALSE, (float*) projMat.n);

        glBindBuffer(GL_ARRAY_BUFFER, tileSprites.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tileSprites.ebo);
        tileSprites.posAttrib = glGetAttribLocation(shaderProgram, "position");
        tileSprites.texAttrib = glGetAttribLocation(shaderProgram, "texcoordIn");
        glVertexAttribPointer(tileSprites.posAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
        glEnableVertexAttribArray(tileSprites.posAttrib);
        glVertexAttribPointer(tileSprites.texAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(tileSprites.texAttrib);
        glDrawElements(GL_TRIANGLES, tileSprites.indexArrayCurrentSize, GL_UNSIGNED_INT, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, mobSprites.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mobSprites.ebo);
        mobSprites.posAttrib = glGetAttribLocation(shaderProgram, "position");
        mobSprites.texAttrib = glGetAttribLocation(shaderProgram, "texcoordIn");
        glVertexAttribPointer(mobSprites.posAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
        glEnableVertexAttribArray(mobSprites.posAttrib);
        glVertexAttribPointer(mobSprites.texAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(mobSprites.texAttrib);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*20*mobEntities.count, mobSprites.vertBase);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(float)*mobSprites.indexArrayCurrentSize, mobSprites.indexBase);
        glDrawElements(GL_TRIANGLES, mobSprites.indexArrayCurrentSize, GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, playerSprite.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerSprite.ebo);
        playerSprite.posAttrib = glGetAttribLocation(shaderProgram, "position");
        playerSprite.texAttrib = glGetAttribLocation(shaderProgram, "texcoordIn");
        glVertexAttribPointer(playerSprite.posAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
        glEnableVertexAttribArray(playerSprite.posAttrib);
        glVertexAttribPointer(playerSprite.texAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(playerSprite.texAttrib);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*20, playerSprite.vertBase);
        glDrawElements(GL_TRIANGLES, playerSprite.indexArrayCurrentSize, GL_UNSIGNED_INT, 0);

        glFinish();
        SwapBuffers(dc);
      }    
    }
    else
    {
        // TODO: Window creation failed, log?
    }
  }
  else
  {
      // TODO: registering window class failed, log?
  }
    
  return(0);
}

