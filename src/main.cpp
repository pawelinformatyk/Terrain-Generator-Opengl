#include <iostream>

#include "glew.h"
#include "freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" 

#include "shader.h" 
#include "texture.h"
#include "terrain.h"


//control variables 
int screen_width = 1920;
int screen_height = 1080;

int mouse_positionX;
int mouse_positionY;
int mbutton;
double cameraX, cameraZ, cameraD, previous_cameraX, previous_cameraZ, previous_cameraD;

glm::mat4 P;
glm::vec3 light_position(1,5,1);
float scale = 1.f;
float rotation=30;
GLfloat ad;//variable to control animation

enum class Model
{
	model_base, model_texture, model_height, model_points, model_wireframe
};
Model display_model;

Terrain* terrain;
Shader* shader_def;
Shader* shader_heig;
Shader* shader_tex;
Texture* texture;


void Draw()
{
	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	terrain->setPolygonMode( GL_FILL );
	
	Shader* shader;

	//Set model,view,projection

	glm::mat4 MV = glm::mat4( 1.0f );
	MV = glm::translate( MV, glm::vec3( 0, 0, cameraD - 4 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraZ + 25 ), glm::vec3( 1, 0, 0 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraX +180+ rotation ), glm::vec3( 0, 1, 0 ) );
	float xz_scale = 4.f / terrain->getSizeVertices();
	MV = glm::scale( MV, glm::vec3( xz_scale, scale / ( 3*terrain->getMaxHeight() - terrain->getMinHeight() ), xz_scale ) );

	glm::mat4 MVP = P * MV;

	glm::vec3 object_color( 0.20f, 0.40f, 0);
	glm::vec3 light_color( 1, 1, 1);
	glm::vec3 light_pos( terrain->getSizeVertices() * light_position.x, terrain->getSizeVertices()/10.f * light_position.y, terrain->getSizeVertices() * light_position.z );
	glm::vec3 view_pos( 0, 0, 0 );

	//set variables/uniforms for choosen display model 
	switch( display_model )
	{
		case Model::model_height:
			shader = shader_heig;
			shader->setFloat( "maximum", terrain->getMaxHeight() );
			break;

		case Model::model_texture:
			shader = shader_tex;

			object_color = glm::vec3( 0.5f, 0.5f, 0.5f );
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, texture->getID() );
			
			shader->setInt( "terrain_size", terrain->getSizeVertices() );
			shader->setInt( "terrain_step", terrain->getStep() );
			shader->setInt( "tex", 0 );
			break;

		case Model::model_points:
			shader = shader_def;

			terrain->setPolygonMode( GL_POINT );
			glPointSize( 2.5f );

			light_color = glm::vec3( 0, 0, 0 );
			light_pos = glm::vec3( 0, 0, 0 );
			view_pos = glm::vec3( 0, 0, 0 );
			break;

		default:
			shader = shader_def;
			break;
	}
	
	shader->use();

	shader->setMat4( "MVP", MVP );
	shader->setVec3( "objectColor", object_color );
	shader->setVec3( "lightColor", light_color );
	shader->setVec3( "lightPos", light_pos );
	shader->setVec3( "viewPos", view_pos );

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
		cameraX = previous_cameraX - ((double)mouse_positionX - x) * 0.1;
		cameraZ = previous_cameraZ - ((double)mouse_positionY - y) * 0.1;
	}
	if( mbutton == GLUT_RIGHT_BUTTON )
	{
		cameraD = previous_cameraD + ((double)mouse_positionY - y) * 0.1;
	}

}
void Keys( GLubyte key, int x, int y )
{
	float step = 0.1f;
	switch( key )
	{
		case 27:    /* Esc - end */
			exit( 1 );
			break;
		case 'w':
			light_position.z += step;
			break;
		case 's':
			light_position.z -= step;
			break;
		case 'a':
			light_position.x += step;
			break;
		case 'd':
			light_position.x -= step;
			break;
		case 'q':
			light_position.y += step*5;
			break;
		case 'e':
			light_position.y -= step*5;
			break;
		case '1':
			display_model = Model::model_base;
			break;
		case '2':
			display_model = Model::model_texture;
			break;
		case '3':
			display_model = Model::model_height;
			break;
		case '4':
			display_model = Model::model_points;
			break;
		case '[':
			scale -= step;
			break;
		case ']':
			scale += step;
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

	shader_def = new Shader( "resources/shaders/vertex_shader.glsl", "resources/shaders/fragment_shader.glsl" );
	shader_heig = new Shader( "resources/shaders/height_vshader.glsl", "resources/shaders/height_fshader.glsl" );
	shader_tex = new Shader( "resources/shaders/vertex_shader.glsl", "resources/shaders/texture_fshader.glsl" );
	texture = new Texture( "resources/tatry3.bmp" );

	//terrain = new Terrain( 160000 );
	terrain = new Terrain( "resources/tatry.txt" );

	
	glutMainLoop();

	delete terrain;
	delete shader_def;
	delete shader_heig;
	delete shader_tex;
	delete texture;

	return 0;
}
