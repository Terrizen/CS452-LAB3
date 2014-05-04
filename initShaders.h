#ifndef INITSHADERS_H_
#define INITSHADERS_H_
#endif

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/gl.h"
#include "GL/glu.h"

#include <vector>
#include <cstdio>
#include <iostream>
using namespace std;

GLuint createShader(GLenum type, const GLchar* shade_source);
const GLchar* inputShader(const char* file_name);
GLuint createProgram(const vector<GLuint> shade_list);

typedef struct{
  GLenum type;
  const char* file_name;
} ShaderInfo;


void initShaders(ShaderInfo* shaders){
  
  ShaderInfo* shade=shaders;
  vector<GLuint> Shade_list;
  
  while(shade->type != GL_NONE){
    Shade_list.push_back(createShader(shade->type,inputShader(shade->file_name)));
    ++shade;
  }
  
  GLuint program=createProgram(Shade_list);
  
  glUseProgram(program);
}


const GLchar* inputShader(const char* file_name){

  FILE* fshade = fopen(file_name, "rb");
  
  if(!fshade){
    fprintf(stderr,"unable to open file '%s'\n",file_name);
    return NULL;
  }
  
  fseek(fshade, 0, SEEK_END);
  long file_size=ftell(fshade);
  fseek(fshade, 0, SEEK_SET);
  
  
  GLchar* shading_source= new GLchar[file_size+1];
  fread(shading_source, 1, file_size, fshade);
  
  
  if(ftell(fshade) == 0){
    fprintf(stderr, "File '%s' is empty.\n",file_name);
    return NULL;
  }

  fclose(fshade);
  
  shading_source[file_size] = 0;
  
  return const_cast<const GLchar*>(shading_source);
  
}


GLuint createShader(GLenum type, const GLchar* shade_source){
  
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &shade_source, NULL);
  
  glCompileShader(shader);
  
  GLint compileStatus;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
  
  if(!compileStatus){
    GLint log_size;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
    
    GLchar* info_log = new GLchar[log_size+1];
    glGetShaderInfoLog(shader,log_size,&log_size,info_log);
    
    const char *shade_info= NULL;
    switch(type){
      case GL_VERTEX_SHADER: shade_info = "vertex"; break;
      case GL_GEOMETRY_SHADER_EXT: shade_info = "geometric"; break;
      case GL_FRAGMENT_SHADER: shade_info = "fragment"; break;
    }
    fprintf(stderr,"\nCompile failure in %u shader: %s\n Error message:\n%s\n",type,shade_info,info_log);
    delete[] info_log;
  }
  return shader;
}

GLuint createProgram(const vector<GLuint> shade_list){

  GLuint program = glCreateProgram();
  
  for(GLuint i=0;i<shade_list.size();i++){glAttachShader(program,shade_list[i]);}
  
  glBindAttribLocation(program, 0, "in_position");
  glBindAttribLocation(program, 1, "in_color");
  glLinkProgram(program);
  
  GLint link_status;
  glGetProgramiv(program, GL_LINK_STATUS, &link_status);
  
  if(!link_status){
    GLint log_size;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
    
    GLchar *info_log = new GLchar[log_size+1];
    glGetProgramInfoLog(program,log_size,&log_size,info_log);
    
    fprintf(stderr,"\nShader linking failed: %s\n",info_log);
    delete[] info_log;
    
    for(GLuint i=0;i<shade_list.size();i++){glDeleteShader(shade_list[i]);}
  }
  return program;
}
