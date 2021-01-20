#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <vector>

#include "GL\glew.h"
#include "GL\freeglut.h"
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

int display_mode = 2;

int mouse_positionX;
int mouse_positionY;
int mbutton;
double cameraX, cameraZ, cameraD, previous_cameraX, previous_cameraZ, previous_cameraD;

glm::mat4 P;
glm::vec3 light_position( 8500, 13000, 40000 );
float scale = 0.000125f;
float rotation;
GLfloat ad;//variable to control animation

GLfloat height_max;
GLint step;
GLint vertices_size;
GLint indices_triangle_strip_size;
GLint indices_triangles_size;

GLuint shader_default, shader_light, shader_height, shader_texture;//shader program
unsigned int VAO_triangle_strip,VAO_triangles, VAO_light;
GLuint texture;


void DrawWireframe()
{
	glUseProgram( shader_default );

	GLuint MVP_id = glGetUniformLocation( shader_default, "MVP" );
	GLuint objectColor_id = glGetUniformLocation( shader_default, "objectColor" );
	GLuint lightColor_id = glGetUniformLocation( shader_default, "lightColor" );
	GLuint lightPos_id = glGetUniformLocation( shader_default, "lightPos" );
	GLuint viewPos_id = glGetUniformLocation( shader_default, "viewPos" );

	glm::mat4 MV = glm::mat4( 1.0f );
	MV = glm::translate( MV, glm::vec3( 0, 0, cameraD - 4.4 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraZ + 25 ), glm::vec3( 1, 0, 0 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraX + 180 + rotation ), glm::vec3( 0, 1, 0 ) );
	MV = glm::scale( MV, glm::vec3( 0.0001, scale, 0.0001 ) );
	glm::mat4 MVP = P * MV;

	glUniformMatrix4fv( MVP_id, 1, GL_FALSE, &(MVP[ 0 ][ 0 ]) );
	glUniform3f( objectColor_id, 0, 1, 0 );
	glUniform3f( lightColor_id, 0, 0, 0 );
	glUniform3f( lightPos_id, 0, 0, 0 );
	glUniform3f( viewPos_id, 0, 0, 0 );

	glBindVertexArray( VAO_triangles );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glDrawElements( GL_TRIANGLES, indices_triangles_size, GL_UNSIGNED_INT, 0 );
}

void DrawModelWithLighting()
{
	glUseProgram( shader_default );

	GLuint MVP_id = glGetUniformLocation( shader_default, "MVP" );
	GLuint objectColor_id = glGetUniformLocation( shader_default, "objectColor" );
	GLuint lightColor_id = glGetUniformLocation( shader_default, "lightColor" );
	GLuint lightPos_id = glGetUniformLocation( shader_default, "lightPos" );
	GLuint viewPos_id = glGetUniformLocation( shader_default, "viewPos" );

	glm::mat4 MV = glm::mat4( 1.0f );
	MV = glm::translate( MV, glm::vec3( 0, 0, cameraD - 4.4 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraZ + 25 ), glm::vec3( 1, 0, 0 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraX + 180 + rotation ), glm::vec3( 0, 1, 0 ) );
	MV = glm::scale( MV, glm::vec3( 0.0001, scale, 0.0001 ) );
	glm::mat4 MVP = P * MV;

	glUniformMatrix4fv( MVP_id, 1, GL_FALSE, &(MVP[ 0 ][ 0 ]) );
	glUniform3f( objectColor_id, 0, 1, 0 );
	glUniform3f( lightColor_id, 1, 1, 1 );
	glUniform3f( lightPos_id, light_position.x, light_position.y / 2, light_position.z );
	glUniform3f( viewPos_id, 0, 10000, 500000 );

	glBindVertexArray( VAO_triangle_strip );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glDrawElements( GL_TRIANGLE_STRIP, indices_triangle_strip_size, GL_UNSIGNED_INT, 0 );


	glUseProgram( shader_light );

	GLuint MVPlamp_id = glGetUniformLocation( shader_light, "MVP" );

	MV = glm::translate( MV, glm::vec3( light_position.x, light_position.y * 10, light_position.z ) );
	MV = glm::scale( MV, glm::vec3( 1000, 1000, 1000 ) );
	MV = glm::scale( MV, glm::vec3( 0.45, 0.45, 0.45 ) );
	MV = glm::rotate( MV, (float)glm::radians( 45.f ), glm::vec3( 0, 1, 0 ) );
	MVP = P * MV;

	glUniformMatrix4fv( MVPlamp_id, 1, GL_FALSE, &(MVP[ 0 ][ 0 ]) );

	glBindVertexArray( VAO_light );
	glDrawArrays( GL_TRIANGLES, 0, 36 );
}

void DrawHeightModel()
{
	glUseProgram( shader_height );

	GLuint MVP_id = glGetUniformLocation( shader_height, "MVP" );
	GLuint max_id = glGetUniformLocation( shader_height, "maximum" );

	glm::mat4 MV = glm::mat4( 1.0f );
	MV = glm::translate( MV, glm::vec3( 0, 0, cameraD - 4.4 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraZ + 25 ), glm::vec3( 1, 0, 0 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraX + 180 + rotation ), glm::vec3( 0, 1, 0 ) );
	MV = glm::scale( MV, glm::vec3( 0.0001, scale, 0.0001 ) );
	glm::mat4 MVP = P * MV;

	glUniformMatrix4fv( MVP_id, 1, GL_FALSE, &(MVP[ 0 ][ 0 ]) );
	glUniform1f( max_id, height_max );

	glBindVertexArray( VAO_triangle_strip );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glDrawElements( GL_TRIANGLE_STRIP, indices_triangle_strip_size, GL_UNSIGNED_INT, 0 );
}

void DrawModelWithTexture()
{
	glUseProgram( shader_texture );

	GLuint MVP_id = glGetUniformLocation( shader_texture, "MVP" );
	GLuint objectColor_id = glGetUniformLocation( shader_texture, "objectColor" );
	GLuint lightColor_id = glGetUniformLocation( shader_texture, "lightColor" );
	GLuint lightPos_id = glGetUniformLocation( shader_texture, "lightPos" );
	GLuint viewPos_id = glGetUniformLocation( shader_texture, "viewPos" );
	GLuint size_id = glGetUniformLocation( shader_texture, "terrain_size" );
	GLuint step_id = glGetUniformLocation( shader_texture, "terrain_step" );

	glm::mat4 MV = glm::mat4( 1.0f );
	MV = glm::translate( MV, glm::vec3( 0, 0, cameraD - 4.4 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraZ + 25 ), glm::vec3( 1, 0, 0 ) );
	MV = glm::rotate( MV, (float)glm::radians( cameraX + 180 + rotation ), glm::vec3( 0, 1, 0 ) );
	MV = glm::scale( MV, glm::vec3( 0.0001, scale, 0.0001 ) );
	glm::mat4 MVP = P * MV;

	glUniformMatrix4fv( MVP_id, 1, GL_FALSE, &(MVP[ 0 ][ 0 ]) );
	glUniform3f( objectColor_id, 0.5, 0.5, 0.5 );
	glUniform3f( lightColor_id, 1, 1, 1 );
	glUniform3f( lightPos_id, light_position.x, light_position.y, light_position.z );
	glUniform3f( viewPos_id, 0, 10000, 500000 );
	glUniform1i( size_id, vertices_size / 2 );
	glUniform1i( step_id, step );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );
	GLuint uniform_tex = glGetUniformLocation( shader_texture, "tex" );
	glUniform1i( uniform_tex, 0 );

	glBindVertexArray( VAO_triangle_strip );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glDrawElements( GL_TRIANGLE_STRIP, indices_triangle_strip_size, GL_UNSIGNED_INT, 0 );


	glUseProgram( shader_light );

	GLuint MVPlamp_id = glGetUniformLocation( shader_light, "MVP" );

	MV = glm::translate( MV, glm::vec3( light_position.x, light_position.y * 10, light_position.z ) );
	MV = glm::scale( MV, glm::vec3( 1000, 1000, 1000 ) );
	MV = glm::scale( MV, glm::vec3( 0.45, 0.45, 0.45 ) );
	MV = glm::rotate( MV, (float)glm::radians( 45.f ), glm::vec3( 0, 1, 0 ) );
	MVP = P * MV;

	glUniformMatrix4fv( MVPlamp_id, 1, GL_FALSE, &(MVP[ 0 ][ 0 ]) );

	glBindVertexArray( VAO_light );
	glDrawArrays( GL_TRIANGLES, 0, 36 );
}

void Draw( void )
{
	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	switch( display_mode )
	{
		case 1:
			DrawWireframe();
			break;
		case 2:
			DrawModelWithLighting();
			break;
		case 3:
			DrawHeightModel();
			break;
		case 4:
			DrawModelWithTexture();
			break;
	}

	glFlush();
	glutSwapBuffers();
}

void mysz( int button, int state, int x, int y )
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
void mysz_ruch( int x, int y )
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
void klawisz( GLubyte key, int x, int y )
{
	float step = 400;
	switch( key )
	{
		case 27:    /* Esc - koniec */
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
			light_position.y += step / 2;
			break;
		case 'e':
			light_position.y -= step / 2;
			break;
		case '1'://siatka trojkatow
			display_mode = 1;
			break;
		case '2'://model z oswietleniem
			display_mode = 2;
			break;
		case '3'://kolory zaleznosc od wysokosci 
			display_mode = 3;
			break;
		case '4'://tekstura
			display_mode = 4;
			break;
		case '[':
			scale -= 0.00001f;
			break;
		case ']':
			scale += 0.00001f;
			break;
		case ' ':
			ad = (ad == 0.3f) ? 0 : 0.3f;
			break;
	}
}
void rozmiar( int width, int height )
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

	glewInit(); //init rozszerzeszeń OpenGL z biblioteki GLEW

	glutDisplayFunc( Draw );			// def. funkcji rysuj¦cej
	glutIdleFunc( idle );			// def. funkcji rysuj¦cej w czasie wolnym procesoora (w efekcie: ci¦gle wykonywanej)
	glutReshapeFunc( rozmiar ); // def. obs-ugi zdarzenia resize (GLUT)
	glutTimerFunc( 10,timer, 0 );
	glutKeyboardFunc( klawisz );		// def. obsługi klawiatury
	glutMouseFunc( mysz ); 		// def. obsługi zdarzenia przycisku myszy (GLUT)
	glutMotionFunc( mysz_ruch ); // def. obsługi zdarzenia ruchu myszy (GLUT)

	glEnable( GL_DEPTH_TEST );


	std::vector<glm::vec3>vertices = CreateTerrainFromFile( "resources/tatry.txt" );
	std::vector<GLuint> indices_triangle_strip = BuildIndicesForTriangleStrip( vertices.size() / 2 );
	std::vector<GLuint> indices_triangles = BuildIndicesForTriangles( vertices.size() / 2 );
	height_max = FindMaxHeight( vertices );
	step = GLint(vertices[ 2 ].x - vertices[ 0 ].x);
	indices_triangle_strip_size = indices_triangle_strip.size();
	indices_triangles_size = indices_triangles.size();
	vertices_size = vertices.size();

	shader_default = loadShaders( "shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl" );
	shader_light = loadShaders( "shaders/light_vshader.glsl", "shaders/light_fshader.glsl" );
	shader_height = loadShaders( "shaders/height_vshader.glsl", "shaders/height_fshader.glsl" );
	shader_texture = loadShaders( "shaders/vertex_shader.glsl", "shaders/texture_fshader.glsl" );

	texture = WczytajTeksture( "resources/tatry3.bmp" );
	if( texture == -1 )
	{
		MessageBox( NULL, "Nie znaleziono pliku z teksturą", "Problem", MB_OK | MB_ICONERROR );
		exit( 0 );
	}

	unsigned int VBO, ebo1,ebo2, VBO_light_source;

	glGenVertexArrays( 1, &VAO_triangle_strip );
	glGenVertexArrays( 1, &VAO_triangles );

	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( glm::vec3 ), &vertices.data()[ 0 ], GL_STATIC_DRAW );
	glGenBuffers( 1, &ebo1 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo1 );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices_triangle_strip.size() * sizeof( GLuint ), &indices_triangle_strip.data()[ 0 ], GL_STATIC_DRAW );
	glGenBuffers( 1, &ebo2 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo2 );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices_triangles.size() * sizeof( GLuint ), &indices_triangles.data()[ 0 ], GL_STATIC_DRAW );

	glBindVertexArray( VAO_triangle_strip );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo1 );
	//position
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void*)0 );
	//normal
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void*)(3 * sizeof( float )) );

	glBindVertexArray( VAO_triangles );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo2 );
	//position
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void*)0 );
	//normal
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void*)(3 * sizeof( float )) );

	float cube[] = {
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};//light source

	glGenVertexArrays( 1, &VAO_light );
	glBindVertexArray( VAO_light );

	glGenBuffers( 1, &VBO_light_source );
	glBindBuffer( GL_ARRAY_BUFFER, VBO_light_source );
	glBufferData( GL_ARRAY_BUFFER, sizeof( cube ), cube, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	glEnableVertexAttribArray( 0 );


	glutMainLoop();

	glDeleteVertexArrays( 1, &VAO_triangle_strip );
	glDeleteVertexArrays( 1, &VAO_triangles );
	glDeleteVertexArrays( 1, &VAO_light );
	glDeleteBuffers( 1, &VBO );
	glDeleteBuffers( 1, &ebo1 );
	glDeleteBuffers( 1, &ebo2 );
	glDeleteBuffers( 1, &VBO_light_source );

	return 0;
}
