#include "terrain.h"

glm::vec3 CalculateNormal( std::vector<glm::vec3>& terr, int x, int z, int step )
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


	//default = P(x,z)
	float hl, hr, hd, hu;
	hl = hr = hd = hu = terr[ x + step * z ].y;

	//calculate height of surrounding points
	if( x == 0 && z == 0 )
	{
		hr = terr[ 1 ].y;
		hu = terr[ step ].y;
	}
	else if( x == 0 && z == step - 1 )
	{
		hd = terr[ step * (step - 2) ].y;
		hr = terr[ step * (step - 1) + 1 ].y;
	}
	else if( x == step - 1 && z == 0 )
	{
		hl = terr[ step - 2 ].y;
		hu = terr[ 2 * step - 1 ].y;
	}
	else if( x == step - 1 && z == step - 1 )
	{
		hl = terr[ step * step - 2 ].y;
		hd = terr[ step * (step - 1) - 1 ].y;
	}
	else if( z == 0 )
	{
		hl = terr[ x - 1 ].y;
		hu = terr[ x + step ].y;
		hr = terr[ x + 1 ].y;
	}
	else if( z == step - 1 )
	{
		hl = terr[ x - 1 ].y;
		hd = terr[ (z - 1) * step + x ].y;
		hr = terr[ x + 1 ].y;
	}
	else if( x == 0 )
	{
		hu = terr[ step * (z + 1) ].y;
		hd = terr[ step * (z - 1) ].y;
		hr = terr[ step * z + 1 ].y;
	}
	else if( x == step - 1 )
	{
		hu = terr[ step * (z + 1) ].y;
		hd = terr[ step * (z - 1) ].y;
		hl = terr[ step * z - 1 ].y;
	}
	else
	{
		hl = terr[ x - 1 + step * z ].y;
		hr = terr[ x + 1 + step * z ].y;
		hd = terr[ (z + 1) * step + x ].y;
		hu = terr[ (z - 1) * step + x ].y;
	}

	//this is basically sum of normals of adjecent triangles 
	return  glm::vec3( -(hl - hr) / step, 2.0f, -(hd - hu) / step );
}

std::vector<glm::vec3> BuildMesh( std::vector<glm::vec3>& height_map )
{
	/*
	Building grid from x+ to x-. z- to z+
	^
	|
	x->*/

	int step = (int)sqrt( height_map.size() );
	int start = -1 * (height_map.size() / 2);

	std::vector<glm::vec3> vertices;
	vertices.reserve( 2 * height_map.size() );

	for( int i = 0, zi = start, z = 0; i < step; i++, zi += step )
	{
		for( int j = 0, xi = -start; j < step; j++, xi -= step, z++ )
		{
			vertices.emplace_back( xi, height_map[ z ].y, zi );
			vertices.emplace_back( CalculateNormal( height_map, j, i, step ) );
		}
	}

	return vertices;
}

std::vector<glm::vec3> CreateHeightMapFromFile( std::string file_name )
{
	/*file contains x,z,y coordinates */

	std::ifstream file( file_name );
	if( !file.is_open() )
	{
		std::cout << "File " << file_name << " not found\n";
		exit( 1 );
	}

	std::vector<glm::vec3> coord{};
	for( int i = 0; 1; i++ )
	{
		if( file.eof() )break;

		float x, y, z;
		file >> x;
		file >> z;
		file >> y;

		coord.emplace_back( x, y, z );
	}

	file.close();

	return coord;
}

std::vector<glm::vec3> CreateTerrainFromFile( std::string file_name )
{
	//return vertices build from heigth map and grid sqrt(height map size) x sqrt(height map size) 
	return BuildMesh( CreateHeightMapFromFile( file_name ) );
}

float FindMaxHeight( std::vector<glm::vec3>& ver )
{
	static float max = 0;

	for( glm::vec3& v : ver )
	{
		if( v.y > max ) max = v.y;
	}

	return max;
}

std::vector<GLuint> BuildIndicesForTriangleStrip( GLuint size )
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

	std::vector<GLuint> ind;
	GLuint step = (GLuint)sqrt( size );

	//first strip 
	for( GLuint i = 1; i < step; i++ )
	{
		ind.push_back( i );
		ind.push_back( i + step - 1 );
	}

	ind.push_back( 2 * step - 1 );
	ind.push_back( 2 * step - 1 );
	ind.push_back( step );

	//rest of strips
	for( GLuint i = step;; i++ )
	{
		ind.push_back( i );
		ind.push_back( i + step );

		if( (i + step) == size )break;
		if( (i + step + 1) % step != 0 )continue;

		ind.push_back( i + step );
		ind.push_back( i + 1 );
	}

	return ind;
}

std::vector<GLuint> BuildIndicesForTriangles( GLuint size )
{
	/*
		^
		|
	v3 ->v4
	^ \	 ^
	|  \ |	    ->>>>>>>
	v1 -> v2

	v1,v2,v3,v3,v2,v4 ->>
	*/

	std::vector<GLuint> ind;
	GLuint step = (GLuint)sqrt( size );

	for( GLuint i = 0; i < step - 1; i++ )
		for( GLuint j = 0; j < step - 1; j++ )
		{
			ind.push_back( j + i * step );
			ind.push_back( j + i * step + 1 );
			ind.push_back( j + (i + 1) * step );
			ind.push_back( j + (i + 1) * step );
			ind.push_back( j + i * step + 1 );
			ind.push_back( j + (i + 1) * step + 1 );
		}

	return ind;
}