#pragma once

#include "glew.h"
#include "freeglut.h"
#include <gl\gl.h> 

class Texture
{
public :
	Texture( const char* file_name );
	GLubyte* getPixels()const;
	
	GLuint getID()const{return ID; };
	int getHeight()const{return height;}
	int getWidth()const{return width;}

private:
	int width;
	int height;
	int nrChannels;

	GLuint ID;
};