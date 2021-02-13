#pragma once

#include <vector>

#include "glew.h"
#include "freeglut.h"
#include "glm/glm.hpp" 

enum Model 
{
	model_base, model_texture, model_height, model_points, model_wireframe
};

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 norm;

	Vertex( glm::vec3 p, glm::vec3 n ) :pos( p ), norm( n ) {};
};

class Terrain
{
private:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<float> height_map;

	unsigned int VAO, VBO, EBO;

	GLenum polygon_mode=GL_FILL;
	float max_height;
	float min_height;
	int step;

	void buildIndices();
	void buildMesh();
	void setup();
	void setHeightMapFromFile(std::string file_name);
	void setHeightMapRandom(int size);

	glm::vec3 calculateNormal( int x, int z );

public:
	Terrain( std::string file_name );
	Terrain( int size, bool random_generate = false );
	~Terrain();

	void draw();

	void setPolygonMode(GLenum mode);//sets what to draw, GL_FILL or GL_POINTS
	float getMaxHeight();
	float getMinHeight();
	int getSizeIndices();
	int getSizeVertices();
	int getStep();
};





