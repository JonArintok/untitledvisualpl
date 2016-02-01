
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#define  GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
typedef unsigned int uint;

#define CHECK_SDL_GL_ERRORS true

#if CHECK_SDL_GL_ERRORS
  void glec(const int line, const char *file) {
    GLenum GLstatus;
    while ((GLstatus = glGetError()) != GL_NO_ERROR) {
      printf("OpenGL error: %i on line %i in %s\n", GLstatus, line, file);
    }
  }
  #define _glec glec(__LINE__, __FILE__);
  void sdlec(int line, const char *file) {
    const char *error = SDL_GetError();
    if (!error || !error[0]) return;
  	printf("SDL error at line %i in %s :\n%s", line, file, error);
    SDL_ClearError();
    exit(-10);
  }
  #define _sdlec sdlec(__LINE__, __FILE__);
#else
  #define _glec
  #define _sdlec
#endif


#include <sys/stat.h>
int getFileSize(const char *restrict path) {
  struct stat st;
  stat(path, &st);
  return st.st_size;
}

void print_CouldNotOpenFile(const char *path) {
  printf("error: could not open file \"%s\"\n", path);
}
int stringFromFile(const char *restrict path, char *dest, uint32_t maxWrite) {
  if (!dest || !path || maxWrite < 1) return 0;
  FILE *fp = fopen(path, "r");
  if (!fp) {
    print_CouldNotOpenFile(path);
    return 0;
  }
  int c, i = 0;
  for (; i < maxWrite; i++) {
    c = fgetc(fp);
    if (c == EOF) {
      dest[i] = '\0';
      break;
    }
    dest[i] = c;
  }
  fclose(fp);
  return i;
}


GLuint createShaderProgram(
  const char *vertPath, 
  const char *fragPath, 
  const char *progName
) {
  int vertSourceSize, fragSourceSize, textBufSize = 1024;
  
  vertSourceSize = getFileSize(vertPath);
  if (!vertSourceSize) {
    print_CouldNotOpenFile(vertPath);
    return 0;
  }
  if (vertSourceSize > textBufSize) textBufSize = vertSourceSize + 1;
  
  fragSourceSize = getFileSize(fragPath);
  if (!fragSourceSize) {
    print_CouldNotOpenFile(fragPath);
    return 0;
  }
  if (fragSourceSize > textBufSize) textBufSize = fragSourceSize + 1;
  
  char *textBuf = malloc(textBufSize);
  GLint success;
  const char *compileErrorString = "error compiling shader \"%s\":\n%s\n";
  
  GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);_glec
  stringFromFile(vertPath, textBuf, textBufSize);
  glShaderSource(vertShader, 1, (const GLchar * const*)&textBuf, NULL);_glec
  glCompileShader(vertShader);_glec
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);_glec
  if (!success) {
    glGetShaderInfoLog(vertShader, textBufSize, NULL, textBuf);_glec
    printf(compileErrorString, vertPath, textBuf);
    return 0;
  }
  
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);_glec
  stringFromFile(fragPath, textBuf, textBufSize);
  glShaderSource(fragShader, 1, (const GLchar * const*)&textBuf, NULL);_glec
  glCompileShader(fragShader);_glec
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);_glec
  if (!success) {
    glGetShaderInfoLog(fragShader, textBufSize, NULL, textBuf);_glec
    printf(compileErrorString, fragPath, textBuf);
    return 0;
  }
  
  GLuint shaderProgram = glCreateProgram();_glec
  glAttachShader(shaderProgram, vertShader);_glec
  glAttachShader(shaderProgram, fragShader);_glec
  glLinkProgram(shaderProgram);_glec
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);_glec
  if (!success) {
    glGetShaderInfoLog(shaderProgram, textBufSize, NULL, textBuf);_glec
    printf("error linking shader program \"%s\":\n%s\n", progName, textBuf);
    return 0;
  }
  glValidateProgram(shaderProgram);_glec
  glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);_glec
  if (!success) {
    glGetShaderInfoLog(shaderProgram, textBufSize, NULL, textBuf);_glec
    printf("error: invalid shader program \"%s\":\n%s\n", progName, textBuf);
    return 0;
  }
  
  free(textBuf);
  return shaderProgram;
}


int main(int argc, char *argv[]) {
	uint32_t videoSizeX = 1280;//pixels
	uint32_t videoSizeY =  800;
	
	SDL_Window    *window    = NULL;
	SDL_GLContext  GLcontext = NULL;
	SDL_Init(SDL_INIT_VIDEO);_sdlec
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	window = SDL_CreateWindow(
		"GraphPunk",               //const char* title,
		SDL_WINDOWPOS_UNDEFINED,   //int         x,
		SDL_WINDOWPOS_UNDEFINED,   //int         y,
		videoSizeX,                //int         w,
		videoSizeY,                //int         h,
		SDL_WINDOW_OPENGL          //Uint32      flags
	);_sdlec
	GLcontext = SDL_GL_CreateContext(window);_sdlec
  SDL_GL_SetSwapInterval(1);_sdlec
  
  glewExperimental = GL_TRUE;
  {
  	GLenum r = glewInit();
    if (r != GLEW_OK) {
      printf("GLEW error: %s\n", glewGetErrorString(r));
      return 1;
    }
    // There's an OpenGL error 1280 here for some reason, just flush it...
    while (glGetError() != GL_NO_ERROR) {};
  }
  //printf("OpenGL version: %s\n\n", glGetString(GL_VERSION));_glec
	
  
  GLuint vao;
  glGenVertexArrays(1, &vao);_glec
  glBindVertexArray(vao);_glec
  
  glClearColor(0.0,0.2,0.3,1.0);_glec
  
  float vertices[] = {
    +0.0f, +0.5f,
    +0.5f, -0.5f,
    -0.5f, -0.5f
  };
  GLuint vbo;
  glGenBuffers(1, &vbo);_glec
  glBindBuffer(GL_ARRAY_BUFFER, vbo);_glec
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(vertices),
    vertices,
    GL_STATIC_DRAW
  );_glec
  
  
  GLuint shaderProgram = createShaderProgram(
    "src/vert.glsl", 
    "src/frag.glsl", 
    "shaderProgram"
  );
  if (!shaderProgram) return __LINE__;
  glUseProgram(shaderProgram);_glec
  
  
  GLint attrib_pos = glGetAttribLocation(shaderProgram, "position");_glec
  glVertexAttribPointer(attrib_pos, 2, GL_FLOAT, GL_FALSE, 0, 0);_glec
  glEnableVertexAttribArray(attrib_pos);_glec
  
  
  int curFrame = 0;
  bool running = true;
	while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT: running = false; break;
        case SDL_MOUSEMOTION:
          break;
        case SDL_MOUSEBUTTONDOWN:
          switch (event.button.button) {
            case SDL_BUTTON_LEFT:  break;
          }
          break;
        case SDL_MOUSEBUTTONUP:
          switch (event.button.button) {
            case SDL_BUTTON_LEFT:  break;
          }
          break;
      }
    }
  	
    glClear(GL_COLOR_BUFFER_BIT);_glec
		glDrawArrays(GL_TRIANGLES, 0, 3);_glec
		
		
		SDL_GL_SwapWindow(window);_sdlec
		
		SDL_Delay(16);
    curFrame++;
	}
	
	SDL_GL_DeleteContext(GLcontext);_sdlec
	SDL_Quit();_sdlec
	return 0;
}