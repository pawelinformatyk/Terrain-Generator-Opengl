#include "glew.h"
#include "freeglut.h"
#include <gl\gl.h> 

class Texture
{
private:
	GLuint ID;

public :

	GLuint getID()const{return ID; };
	Texture( const char* file_name );
};