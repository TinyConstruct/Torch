#include "aro_opengl.h"


opengl_info checkOpenGLExtensions() {
  if(!glGetStringi) {
    glGetStringi = (gl_get_stringi*) getWin32GLFunc("glGetStringi");
  }
  opengl_info result = {};
  result.vendor = (char*)glGetString(GL_VENDOR);
  result.renderer = (char*)glGetString(GL_RENDERER);
  result.version = (char*)glGetString(GL_VERSION);
  //result.shadingLanguageVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
  //result.extensions = (char*)glGetString(GL_EXTENSIONS);
  
  /*For posterity, this is how to query the extension string array
  
  GLint numExt;
  glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
  for(int i = 0; i < numExt; i++){
    char* ext = (char*) glGetStringi(GL_EXTENSIONS, i);
    if(strcmp(ext, "GL_ARB_vertex_program")==0) {
      
    }
  }
  */
  return result;
}
 
bool loadGLWinFunctions() {
  if(!(wglSwapInterval = (wgl_swap_interval_ext*) getWin32GLFunc("wglSwapIntervalEXT"))){return false;}
  return true;
}

void loadDebugFunctions() {
  //TODO: Just leave this if it fails? Only needs to work on my machine?
  glEnable(DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallbackARB = (gl_debug_message_callback_arb*) getWin32GLFunc("glDebugMessageCallbackARB");
  if(glDebugMessageCallbackARB) {
    glDebugMessageCallbackARB(glDebugOutput, 0);
  }
}

static bool loadGLCoreFunctions() {
  if(!(glGenBuffers = (gl_gen_buffers*) getWin32GLFunc("glGenBuffers"))) {return false;}
  if(!(glBindBuffer = (gl_bind_buffer*) getWin32GLFunc("glBindBuffer")) ){return false;}
  if(!(glDeleteBuffers = (gl_delete_buffers*) getWin32GLFunc("glDeleteBuffers")) ){return false;}
  if(!(glBufferData = (gl_buffer_data*) getWin32GLFunc("glBufferData")) ){return false;}
  if(!(glCreateShader = (gl_create_shader*) getWin32GLFunc("glCreateShader")) ){return false;}
  if(!(glShaderSource = (gl_shader_source*) getWin32GLFunc("glShaderSource")) ){return false;}  
  if(!(glCompileShader = (gl_compile_shader*) getWin32GLFunc("glCompileShader")) ){return false;}  
  if(!(glGetShaderiv = (gl_get_shader_iv*) getWin32GLFunc("glGetShaderiv")) ){return false;}  
  if(!(glGetShaderInfoLog = (gl_get_shader_info_log*) getWin32GLFunc("glGetShaderInfoLog")) ){return false;}  
  if(!(glCreateProgram = (gl_create_program*) getWin32GLFunc("glCreateProgram")) ){return false;}  
  if(!(glAttachShader = (gl_attach_shader*) getWin32GLFunc("glAttachShader")) ){return false;}    
  if(!(glBindFragDataLocation = (gl_bind_frag_data_location*) getWin32GLFunc("glBindFragDataLocation")) ){return false;}    
  if(!(glLinkProgram = (gl_link_program*) getWin32GLFunc("glLinkProgram")) ){return false;}    
  if(!(glUseProgram = (gl_use_program*) getWin32GLFunc("glUseProgram")) ){return false;}    
  if(!(glGetAttribLocation = (gl_get_attrib_location*) getWin32GLFunc("glGetAttribLocation")) ){return false;}    
  if(!(glVertexAttribPointer = (gl_vertex_attrib_pointer*) getWin32GLFunc("glVertexAttribPointer")) ){return false;}    
  if(!(glEnableVertexAttribArray = (gl_enable_vertex_attrib_array*) getWin32GLFunc("glEnableVertexAttribArray")) ){return false;}    
  if(!(glGenVertexArrays = (gl_gen_vertex_arrays*) getWin32GLFunc("glGenVertexArrays")) ){return false;}    
  if(!(glBindVertexArray = (gl_bind_vertex_array*) getWin32GLFunc("glBindVertexArray")) ){return false;}    
  if(!(glGetUniformLocation  = (gl_get_uniform_location*) getWin32GLFunc("glGetUniformLocation")) ){return false;}    
  if(!(glUniform1f = (gl_uniform_1f*) getWin32GLFunc("glUniform1f")) ){return false;}    
  if(!(glUniform2f = (gl_uniform_2f*) getWin32GLFunc("glUniform2f")) ){return false;}    
  if(!(glUniform3f = (gl_uniform_3f*) getWin32GLFunc("glUniform3f")) ){return false;}    
  if(!(glUniform4f = (gl_uniform_4f*) getWin32GLFunc("glUniform4f")) ){return false;}    
  if(!(glGetProgramiv = (gl_get_programiv*) getWin32GLFunc("glGetProgramiv")) ){return false;}    
  if(!(glGetProgramInfoLog = (gl_get_program_info_log*) getWin32GLFunc("glGetProgramInfoLog")) ){return false;}    
  if(!(glDeleteShader = (gl_delete_shader*) getWin32GLFunc("glDeleteShader")) ){return false;}    
  if(!(glActiveTexture = (gl_active_texture*) getWin32GLFunc("glActiveTexture")) ){return false;}    
  if(!(glUniformMatrix4fv = (gl_uniform_matrix4fv*) getWin32GLFunc("glUniformMatrix4fv")) ){return false;}    
  if(!(glBufferSubData = (gl_buffer_sub_data*) getWin32GLFunc("glBufferSubData")) ){return false;}    

  return true;
}

void win32InitOpenGL(HWND window) {
  HDC windowDC = GetDC(window);
  PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
  desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
  desiredPixelFormat.nVersion = 1;
  desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
  desiredPixelFormat.iPixelType = PFD_TYPE_RGBA; //4 components, not color-indexed
  desiredPixelFormat.cColorBits = 32;
  desiredPixelFormat.cAlphaBits = 8;
  desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
  int suggestedPixelFormatIndex = ChoosePixelFormat(windowDC, &desiredPixelFormat);
  
  PIXELFORMATDESCRIPTOR suggestedPixelFormat;
  DescribePixelFormat(windowDC, suggestedPixelFormatIndex, sizeof(suggestedPixelFormat), &suggestedPixelFormat);
  SetPixelFormat(windowDC, suggestedPixelFormatIndex, &suggestedPixelFormat);
  
  HGLRC openGLRC = wglCreateContext(windowDC);
  if(wglMakeCurrent(windowDC, openGLRC)) { //creating doesn't make current
    wgl_create_context_attribs_arb* wglCreateContextAttribsARB = (wgl_create_context_attribs_arb*)wglGetProcAddress("wglCreateContextAttribsARB");
    if(wglCreateContextAttribsARB) {
      int attribs[] = {WGL_CONTEXT_MAJOR_VERSION, 3,  WGL_CONTEXT_MINOR_VERSION, 3, 
      WGL_CONTEXT_FLAGS , WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 
      #ifdef DEBUG
      | WGL_CONTEXT_DEBUG_BIT
      #endif
      ,WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 0};
      HGLRC shareContext = 0;
      HGLRC modernContext = wglCreateContextAttribsARB(windowDC, shareContext, attribs);
      if(modernContext) {
        wglMakeCurrent(windowDC, modernContext);
        wglDeleteContext(openGLRC);
        openGLRC = modernContext;
        #ifdef DEBUG
        loadDebugFunctions();
        #endif
        if(!loadGLCoreFunctions()) {
          InvalidCodePath;
        }
        glEnable(GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        loadGLWinFunctions();
        float maxTextureDim;
      }
      else {
        glEnable(DEBUG_OUTPUT_SYNCHRONOUS);
        DWORD errorCode = GetLastError();
        LPVOID lpMsgBuf;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, 500, (LPTSTR)&lpMsgBuf, 0, NULL);
        MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
        LocalFree(lpMsgBuf);
        InvalidCodePath;
      }
    }
    //Yay, we did it
  }
  else {
    //context failed
    InvalidCodePath;
  }
  ReleaseDC(window, windowDC);
  globalGLInfo = checkOpenGLExtensions();
}

//Takes two strings, one for the vert shader and one for the fragment shader, 
//returns the ID number for the linked shader
GLuint compileShader(const char* vertSrc, const char* fragSrc) {
  GLuint vertex, fragment, ID;
  int success;
     
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertSrc, NULL);
  glCompileShader(vertex);
  // print compile errors if any
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    char buffer[512];
    glGetShaderInfoLog(vertex, 512, NULL, buffer);
    OutputDebugString(buffer);
    InvalidCodePath;
  }
  
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragSrc, NULL);
  glCompileShader(fragment);
  // print compile errors if any
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    char buffer[512];
    glGetShaderInfoLog(fragment, 512, NULL, buffer);
    OutputDebugString(buffer);
    InvalidCodePath;
  }

  // shader Program
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  // print linking errors if any
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if(!success)
  {
    char buffer[512];
    glGetProgramInfoLog(ID, 512, NULL, buffer);
    OutputDebugString(buffer);
    InvalidCodePath;
  }
    
  // delete the shaders as they're linked into our program now and no longer necessery
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  return ID;
}