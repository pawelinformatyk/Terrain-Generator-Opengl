#include <iostream>
#include <time.h>

#include "glew.h"
#include "freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" 

#include "shader.h" 
#include "texture.h"
#include "terrain.h"
#include "skybox.h"


//control variables 
int screen_width = 800*2;
int screen_height = 600*2;

int mouse_positionX;
int mouse_positionY;
int mbutton;
double cameraX, cameraZ, cameraD, previous_cameraX, previous_cameraZ, previous_cameraD;
glm::vec3 light_position(-2,-2,-2);
float scale = 1.f;
float rotation=0;
GLfloat ad;//variable to control animation

glm::mat4 P;//projection

enum class Choice : int
{
	texture, step_gradient, blank, normals, gradient, terrain_water,skybox
};
Choice display;
std::vector<Shader>shaders;

Terrain* terrain;
Skybox* skybox;


void Draw()
{
	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//Set model,view,projection matrix 
	glm::mat4 M = glm::mat4( 1.0f );
	float scale_xz = 4.f / terrain->getSizeVertices();
	float scale_y = scale / (3 * terrain->getMaxHeight() - terrain->getMinHeight());
	M = glm::scale( M, glm::vec3( scale_xz, scale_y, scale_xz ) );

	glm::mat4 V = glm::mat4( 1.0f );
	V = glm::translate( V, glm::vec3( 0, 0, cameraD - 4 ) );
	V = glm::rotate( V, (float)glm::radians( cameraZ +25 ), glm::vec3( 1, 0, 0 ) );
	V = glm::rotate( V, (float)glm::radians( cameraX + 180 + rotation ), glm::vec3( 0, 1, 0 ) );

	glm::mat4 MVP = P * V * M;

	//position of camera
	//distance between 0,0,0 to camera(view pos) 
	const unsigned int r = unsigned int(terrain->getSizeVertices() / 2 + 1 / scale_xz);
	float xn = float( r * glm::sin( glm::radians( 90 - cameraZ-25  ) ) *
		glm::cos( glm::radians( cameraX  -90+ rotation ) ) );
	float zn=float( r* glm::sin( glm::radians( 90 - cameraZ  -25) ) *
		glm::sin( glm::radians( cameraX  -90+ rotation ) ) );
	float yn = float(r* glm::cos( glm::radians( 90 - cameraZ -25 ) ));
	glm::vec3 view_pos( xn, yn, zn );

	Shader* shader;
	terrain->setPolygonMode( GL_FILL );

	//draw terrain
	//set variables/uniforms for choosen display model 
	switch( display )
	{
		case Choice::normals:
			//draw terrain's normals on top of default model
			shader = &shaders[ static_cast<int>(Choice::normals) ];
			shader->use();

			shader->setMat4( "projection", P );
			shader->setMat4( "VM", V * M );

			terrain->draw( *shader );
			//draw default model

		case Choice::texture:
			shader = &shaders[ static_cast<int>(Choice::texture) ];
			shader->use();

			shader->setMat4( "MVP", MVP );
			shader->setVec3( "viewPos", view_pos );
			shader->setInt( "terrain_size", terrain->getSizeVertices() );
			shader->setInt( "terrain_step", terrain->getStep() );
			shader->setFloat( "material.shininess", 8 );
			shader->setVec3( "light.ambient", glm::vec3( 0.1f ) );
			shader->setVec3( "light.diffuse", glm::vec3( 0.5f ) );
			shader->setVec3( "light.specular", glm::vec3( 1.f ) );
			shader->setVec3( "light.direction", light_position );

			break;

		case Choice::step_gradient:
			shader = &shaders[ static_cast<int>(Choice::step_gradient) ];
			shader->use();

			shader->setMat4( "MVP", MVP );
			shader->setFloat( "maximum", terrain->getMaxHeight() );
			break;

		case Choice::gradient:
			shader = &shaders[ static_cast<int>(Choice::gradient) ];
			shader->use();

			shader->setMat4( "MVP", MVP );
			shader->setFloat( "maximum", terrain->getMaxHeight() );
			shader->setFloat( "minimum", terrain->getMinHeight() );
			break;

		case Choice::terrain_water:
			shader = &shaders[ static_cast<int>(Choice::terrain_water) ];
			shader->use();

			shader->setFloat( "maximum", terrain->getMaxHeight() );
			shader->setFloat( "minimum", terrain->getMinHeight() );
			shader->setMat4( "MVP", MVP );
			shader->setVec3( "viewPos", view_pos );
			shader->setVec3( "light.ambient", glm::vec3( 0.1f ) );
			shader->setVec3( "light.diffuse", glm::vec3( 0.5f ) );
			shader->setVec3( "light.specular", glm::vec3( 1.f ) );
			shader->setVec3( "light.direction", light_position );

			break;

		case Choice::blank:
			shader = &shaders[ static_cast<int>(Choice::blank) ];
			shader->use();

			terrain->setPolygonMode( GL_POINT );
			glPointSize( 2.5f );

			shader->setMat4( "MVP", MVP );
			break;

		default:
			shader = &shaders[ static_cast<int>(Choice::blank) ];
			shader->use();
			break;

	}

	terrain->draw( *shader );

	//draw skybox
	shader = &shaders[ static_cast<int>(Choice::skybox) ];
	shader->use();
	shader->setInt( "skybox", 0 );

	V = glm::mat4( 1.0f );
	V = glm::rotate( V, (float)glm::radians( cameraZ + 25 ), glm::vec3( 1, 0, 0 ) );
	V = glm::rotate( V, (float)glm::radians( cameraX + rotation ), glm::vec3( 0, 1, 0 ) );

	shader->setMat4( "VP", P * V );

	skybox->Draw();

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
			light_position.z -= step;
			break;
		case 's':
			light_position.z += step;
			break;
		case 'a':
			light_position.x -= step;
			break;
		case 'd':
			light_position.x += step;
			break;
		case 'r':
			light_position.y -= step;
			break;
		case 'f':
			light_position.y += step;
			break;
		case '1':
			display = Choice::texture;
			break;
		case '2':
			display = Choice::blank;
			break;
		case '3':
			display = Choice::normals;
			break;
		case '4':
			display = Choice::step_gradient;
			break;
		case '5':
			display = Choice::gradient;
			break;
		case '6':
			display = Choice::terrain_water;
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

//	draw_texture, draw_step_gradient, blank, draw_normals, draw_gradient, draw_terrain_water
	shaders.emplace_back( "resources/shaders/vertexshader.glsl", "resources/shaders/texture_fs.glsl" );
	shaders.emplace_back( "resources/shaders/vertexshader.glsl", "resources/shaders/stepGradient_fs.glsl" );
	shaders.emplace_back( "resources/shaders/blank_vs.glsl", "resources/shaders/blank_fs.glsl" );
	shaders.emplace_back( "resources/shaders/normals_vs.glsl", "resources/shaders/normals_fs.glsl", "resources/shaders/normals_gs.glsl" );
	shaders.emplace_back( "resources/shaders/vertexshader.glsl", "resources/shaders/gradient_fs.glsl" );
	shaders.emplace_back( "resources/shaders/vertexshader.glsl", "resources/shaders/terrainWater_fs.glsl" );
	shaders.emplace_back( "resources/shaders/skybox_vs.glsl", "resources/shaders/skybox_fs.glsl" );

	Terrain terr( "resources/tatry.txt", "resources/textures/tatry3.bmp", "resources/textures/tatry3gray.bmp" );
	terrain =&terr;

	std::vector<std::string> faces
	{
		"resources/textures/skybox/right.jpg",
		 "resources/textures/skybox/left.jpg",
		"resources/textures/skybox/top.jpg",
		"resources/textures/skybox/bottom.jpg",
		"resources/textures/skybox/front.jpg",
		 "resources/textures/skybox/back.jpg"
	};
	//std::vector<std::string> faces
	//{
	//	"resources/textures/skybox_posz/posx.jpg",
	//	 "resources/textures/skybox_posz/negx.jpg",
	//	"resources/textures/skybox_posz/posy.jpg",
	//	"resources/textures/skybox_posz/negy.jpg",
	//	"resources/textures/skybox_posz/posz.jpg",
	//	 "resources/textures/skybox_posz/negz.jpg"
	//};
	Skybox skbox( faces );
	skybox = &skbox;

	glutMainLoop();

	return 0;
}
