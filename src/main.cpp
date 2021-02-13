#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <vector>

#include "glew.h"
#include "freeglut.h"
#include "glm/vec3.hpp" 
#include "glm/vec4.hpp" 
#include "glm/mat4x4.hpp" 
#include "glm/gtc/matrix_transform.hpp" 

#include "shaderLoader.h" 
#include "tekstura.h"
#include "terrain.h"



//control variables 
int screen_width = 1920/2;
int screen_height = 1080/2;

int mouse_positionX;
int mouse_positionY;
int mbutton;
double cameraX, cameraZ, cameraD, previous_cameraX, previous_cameraZ, previous_cameraD;

glm::mat4 P;
glm::vec3 light_position(1);
float scale = 1.f;
float rotation=30;
GLfloat ad;//variable to control animation

Terrain* terrain;
Model display_model;

GLuint shader_default, shader_light, shader_height, shader_texture;//shader program
GLuint texture_id;

void Draw()
{
	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	terrain->setPolygonMode( GL_FILL );

	//Set model,view,projection
	float height_max = terrain->getMaxHeight();

	glm::mat4 MV = glm::mat4( 1.0f );
	MV = glm::translate( MV, glm::vec3( 0, -.5f, cameraD - 4 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraZ + 25 ), glm::vec3( 1, 0, 0 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraX + 180 + rotation ), glm::vec3( 0, 1, 0 ) );
	MV = glm::scale( MV, glm::vec3( 4. / terrain->getSizeVertices(), 1 / (5 * height_max - terrain->getMinHeight()) * scale, 4. / terrain->getSizeVertices() ) );
	glm::mat4 MVP = P * MV;

	glm::vec3 object_color( 0.23f, 0.31f, 0);
	glm::vec3 light_color( 1, 1, 1);
	glm::vec3 light_pos( terrain->getSizeVertices() * light_position.x, 10 * height_max * scale * light_position.y, terrain->getSizeVertices() * light_position.z);
	glm::vec3 view_pos( 0, 10000, 500000 );

	GLuint shader;

	//set variables/uniforms for choosen display model 
	switch( display_model )
	{
		default:
			shader = shader_default;
			break;

		case model_height:
			shader = shader_height;
			glUniform1f(  glGetUniformLocation( shader_height, "maximum" ), height_max );
			break;

		case model_texture:
			shader = shader_texture;

			object_color = glm::vec3( 0.5f, 0.5f, 0.5f );
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, texture_id );
			
			glUniform1i( glGetUniformLocation( shader, "terrain_size" ), terrain->getSizeVertices() );
			glUniform1i( glGetUniformLocation( shader, "terrain_step" ), terrain->getStep());
			glUniform1i( glGetUniformLocation( shader, "tex" ), 0 );
			break;

		case model_points:
			shader = shader_default;

			terrain->setPolygonMode( GL_POINT );
			glPointSize( 2.5f );

			light_color = glm::vec3( 0, 0, 0 );
			light_pos = glm::vec3( 0, 0, 0 );
			view_pos = glm::vec3( 0, 0, 0 );
			break;
	}
	
	glUseProgram( shader );

	glUniformMatrix4fv( glGetUniformLocation( shader, "MVP" ), 1, GL_FALSE, &(MVP[ 0 ][ 0 ]) );
	glUniform3f( glGetUniformLocation( shader, "objectColor" ), object_color.x, object_color.y, object_color.z);
	glUniform3f( glGetUniformLocation( shader, "lightColor" ), light_color.x,light_color.y, light_color.z);
	glUniform3f( glGetUniformLocation( shader, "lightPos" ), light_pos.x, light_pos.y, light_pos.z );
	glUniform3f( glGetUniformLocation( shader, "viewPos" ), view_pos.x, view_pos.y, view_pos.z );

	terrain->draw();

	glFlush();
	glutSwapBuffers();
}

void MouseClick( int button, int state, int x, int y )
{
	mbutton = button;
	switch( state )
	{
		case GLUT_UP:
			break;
		case GLUT_DOWN:
			mouse_positionX = x;
			mouse_positionY = y;
			previous_cameraX = cameraX;
			previous_cameraZ = cameraZ;
			previous_cameraD = cameraD;
			break;
	}
}
void MouseMovement( int x, int y )
{
	if( mbutton == GLUT_LEFT_BUTTON )
	{
		cameraX = previous_cameraX - (mouse_positionX - x) * 0.1;
		cameraZ = previous_cameraZ - (mouse_positionY - y) * 0.1;
	}
	if( mbutton == GLUT_RIGHT_BUTTON )
	{
		cameraD = previous_cameraD + (mouse_positionY - y) * 0.1;
	}

}
void Keys( GLubyte key, int x, int y )
{
	switch( key )
	{
		case 27:    /* Esc - end */
			exit( 1 );
			break;
		case 'w':
			light_position.z +=0.01f * scale;
			break;
		case 's':
			light_position.z -= 0.01f * scale;
			break;
		case 'a':
			light_position.x += 0.01f * scale;
			break;
		case 'd':
			light_position.x -= 0.01f * scale;
			break;
		case 'q':
			light_position.y += 0.05f * scale;
			break;
		case 'e':
			light_position.y -= 0.05f*scale;
			break;
		case '1':
			display_model = model_base;
			break;
		case '2':
			display_model = model_texture;
			break;
		case '3':
			display_model =model_height;
			break;
		case '4':
			display_model = model_points;
			break;
		case '[':
			scale -= 0.1f;
			break;
		case ']':
			scale += 0.1f;
			break;
		case ' ':
			ad = (ad == 0.3f) ? 0 : 0.3f;
			break;
	}
}
void ScreenSize( int width, int height )
{
	screen_width = width;
	screen_height = height;

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport( 0, 0, screen_width, screen_height );

	P = glm::perspective( glm::radians( 60.0f ), (GLfloat)screen_width / (GLfloat)screen_height, 0.01f, 1000.0f );

	glutPostRedisplay();
}
void idle()
{

	glutPostRedisplay();
}
void timer( int t )
{
	if( rotation >= 360 )
		rotation = 0;

	rotation += ad;

	glutTimerFunc( 10, timer, 0 );
}

int main( int argc, char** argv )
{
	/*Controls : 
	
	display modes 1-wireframe,2-green model with light,3-model color depends on height,4- terrain with texture

	rotate, zoom with mouse

	qa - change height of light source
	wsad - change position of light source

	[] increase scale 

	space = start/stop animation 
	*/

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( screen_width, screen_height );
	glutInitWindowPosition( 0, 0 );
	glutCreateWindow( "Terrain Generator" );

	glewInit(); 

	glutDisplayFunc( Draw );		
	glutIdleFunc( idle );			
	glutReshapeFunc( ScreenSize ); 
	glutTimerFunc( 10,timer, 0 );
	glutKeyboardFunc( Keys );	
	glutMouseFunc( MouseClick ); 		
	glutMotionFunc( MouseMovement ); 

	glEnable( GL_DEPTH_TEST );


	terrain = new Terrain( "resources/tatry.txt" );

	shader_default = loadShaders( "src/shaders/vertex_shader.glsl", "src/shaders/fragment_shader.glsl" );
	shader_light = loadShaders( "src/shaders/light_vshader.glsl", "src/shaders/light_fshader.glsl" );
	shader_height = loadShaders( "src/shaders/height_vshader.glsl", "src/shaders/height_fshader.glsl" );
	shader_texture = loadShaders( "src/shaders/vertex_shader.glsl", "src/shaders/texture_fshader.glsl" );

	texture_id = WczytajTeksture( "resources/tatry3.bmp" );
	if( texture_id == -1 )
	{
		MessageBox( NULL, "Texture file not found", "Problem", MB_OK | MB_ICONERROR );
		exit( 0 );
	}


	glutMainLoop();

	delete terrain;

	return 0;
}
