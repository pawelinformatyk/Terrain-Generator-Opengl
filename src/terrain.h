#pragma once

#include <vector>

#include "texture.h"
#include "shader.h"
#include "glew.h"
#include "freeglut.h"
#include "glm/glm.hpp" 

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

	const Texture diffuse_map;
	const Texture specular_map;

	unsigned int VAO, VBO, EBO;

	GLenum polygon_mode=GL_FILL;
	float height_max;
	float height_min;
	int step;

	void buildIndices();
	void buildMesh();
	void setup();
	void buildHeightMapFromFile(std::string file_name);
	void buildHeightMapRandom(int size);

	glm::vec3 calculateNormal( int x, int z );

public:
	Terrain( const std::string& vertices_file_path, const std::string& diff_map_path, const std::string& spec_map_path );//terrain from file 
	//Terrain( int size );//random terrain 
	//Terrain( std::string texture,int size );
	~Terrain();

	void draw(Shader& shader);

	void setPolygonMode(GLenum mode);//sets what to draw, GL_FILL or GL_POINTS
	float getMaxHeight();
	float getMinHeight();
	int getSizeIndices();
	int getSizeVertices();
	int getStep();
};





