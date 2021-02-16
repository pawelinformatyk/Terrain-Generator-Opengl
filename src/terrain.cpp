#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>

#include "terrain.h"
#include "noise.h"

Terrain::Terrain( std::string file_name )
{
	buildHeightMapFromFile( file_name );
	buildMesh();
	buildIndices();
	
	setup();
}

Terrain::Terrain( int size )
{
	buildHeightMapRandom(size);
	buildMesh();
	buildIndices();

	setup();
}

Terrain::~Terrain()
{
	glDeleteVertexArrays( 1, &VAO);
	glDeleteBuffers( 1, &VBO );
	glDeleteBuffers( 1, &EBO );
}

void Terrain::buildHeightMapFromFile( std::string file_name )
{
	std::ifstream file( file_name );
	if( !file.is_open() )
	{
		std::cout << "File " << file_name << " not found\n";
		exit( 1 );
	}

	for( int i = 0; 1; i++ )
	{
		if( file.eof() )break;

		float y;
		file >> y;
		file >> y;
		file >> y;

		height_map.push_back( y );
	}

	file.close();

	auto input = std::minmax_element( height_map.begin(), height_map.end() );

	height_max = *input.second;
	height_min = *input.first;
}

void Terrain::buildHeightMapRandom( int size )
{
	height_map.reserve( size );


	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen( rd() ); // seed the generator
	std::uniform_real_distribution<> distr( 0.01, 0.03 ); // define the range,i like this range

	float seed = float( distr( gen ) );

	step = (int)sqrt( size );
	float start = -1000.f + 1000.f / step;
	float z = start;

	for( int i = 0; i < step; i++, z += 2000.f / step )
	{
		float x = start;
		for( int j = 0; j < step; j++, x += 2000.f / step )
		{
			height_map.push_back( (PerlinNoise::getOctavePerlin( x * seed, z * seed, 0, 4, 0.1f, 1, 0.25f )) );
		}
	}

	//map values 
	float output_end = 1700.f;
	float output_start = 300.f;

	float slope = (output_end - output_start)/2;

	for( auto& v : height_map )
	{
		v = output_start + floor( slope * (v)+0.5f );
	}

	auto input = std::minmax_element( height_map.begin(), height_map.end() );

	height_max = *input.second;
	height_min = *input.first;
}

void Terrain::buildMesh()
{
	/*
	Building grid from x- to x+. z- to z+
	^
	|
	x->*/

	vertices.reserve( 2 * height_map.size() );
	
	step = (int)sqrt( height_map.size() );
	int start = -1*(height_map.size() / 2) + step / 2;//even points on either sides of axis 
	float zi = float(start);
	unsigned int ij = 0;

	for( int i = 0; i < step; i++, zi += step )
	{
		float xi = float(start);
		for( int j = 0; j < step; j++, xi += step )
		{
			vertices.emplace_back( glm::vec3(xi, height_map[ ij++ ], zi), calculateNormal( j, i ) );
		}
	}
}

void Terrain::buildIndices()
{
	/*https://www.learnopengles.com/tag/triangle-strips/

	e.g: size=9 step 3

	v7 -> v8 -> v9
	^	  ^		^
	|	  |	    |
	v4 -> v5 -> v6
	^	  ^		^
	|	  |	    |
	v1 -> v2 -> v3

					 degenerate
	v1,v4,v2,v5,v3,v6,v6,v4,v4,v7,v5...
	*/

	for( GLuint i = 0;; i++ )
	{
		indices.push_back( i );
		indices.push_back( i + step );

		if( (i + step) == vertices.size() )break;
		if( (i + step + 1) % step != 0 )continue;

		indices.push_back( i + step );
		indices.push_back( i + 1 );
	}
}

void Terrain::setup()
{
	glGenVertexArrays( 1, &VAO );
	glGenBuffers( 1, &VBO );
	glGenBuffers( 1, &EBO );

	glBindVertexArray( VAO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );

	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex), &vertices.data()[ 0 ], GL_STATIC_DRAW );
	
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( GLuint ), &indices.data()[ 0 ], GL_STATIC_DRAW );

	//position
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0 );
	//normal
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,norm) );

	glBindVertexArray( 0 );
}

glm::vec3 Terrain::calculateNormal( int x, int z )
{
		/*
	Calculate normals from surrounding points and step in the grid
	normal = weighted average of the surrounding triangles' normal vectors
	//https://stackoverflow.com/questions/6656358/calculating-normals-in-a-triangle-mesh/21660173#21660173


	TERRAIN
	|   \|/   |
..--+----U----+--..
	|   /|\   |
	|  / | \  |
  \ | / 1|2 \ | /
   \|/   |   \|/
..--L----P----R--...
   /|\   |   /|\
  / | \ 4|3 / | \
	|  \ | /  |
	|   \|/   |
..--+----D----+--..
	|   /|\   |
	*/

	float hl, hr, hd, hu;
	hl = hr = hd = hu = height_map[ x + step * z ];

	//calculate height of surrounding points
	if( x == 0 && z == 0 )
	{
		hr = height_map[ 1 ];
		hu = height_map[ step ];
	}
	else if( x == 0 && z == step - 1 )
	{
		hd = height_map[ step * (step - 2) ];
		hr = height_map[ step * (step - 1) + 1 ];
	}
	else if( x == step - 1 && z == 0 )
	{
		hl = height_map[ step - 2 ];
		hu = height_map[ 2 * step - 1 ];
	}
	else if( x == step - 1 && z == step - 1 )
	{
		hl = height_map[ step * step - 2 ];
		hd = height_map[ step * (step - 1) - 1 ];
	}
	else if( z == 0 )
	{
		hl = height_map[ x - 1 ];
		hu = height_map[ x + step ];
		hr = height_map[ x + 1 ];
	}
	else if( z == step - 1 )
	{
		hl = height_map[ x - 1 ];
		hd = height_map[ (z - 1) * step + x ];
		hr = height_map[ x + 1 ];
	}
	else if( x == 0 )
	{
		hu = height_map[ step * (z + 1) ];
		hd = height_map[ step * (z - 1) ];
		hr = height_map[ step * z + 1 ];
	}
	else if( x == step - 1 )
	{
		hu = height_map[ step * (z + 1) ];
		hd = height_map[ step * (z - 1) ];
		hl = height_map[ step * z - 1 ];
	}
	else
	{
		hl = height_map[ x - 1 + step * z ];
		hr = height_map[ x + 1 + step * z ];
		hd = height_map[ (z + 1) * step + x ];
		hu = height_map[ (z - 1) * step + x ];
	}

	//this is basically sum of normals of adjecent triangles 
	return  glm::vec3( (hl - hr) / step, 2.0f, -(hd - hu) / step );
}

void Terrain::draw()
{
	glBindVertexArray( VAO );
	glPolygonMode( GL_FRONT_AND_BACK, polygon_mode );
	glDrawElements( GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );
}

void Terrain::setPolygonMode( GLenum mode )
{
	this->polygon_mode=mode;
}

float Terrain::getMaxHeight()
{
	return height_max;
}

float Terrain::getMinHeight()
{
	return height_min;
}

int Terrain::getSizeIndices()
{
	return indices.size();
}

int Terrain::getSizeVertices()
{
	return vertices.size();
}

int Terrain::getStep()
{
	return step;
}

