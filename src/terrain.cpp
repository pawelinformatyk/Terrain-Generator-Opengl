#include "terrain.h"

glm::vec3 CalculateNormal( std::vector<float>& terr, int x, int z, int step )
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
	hl = hr = hd = hu = terr[ x + step * z ];

	//calculate height of surrounding points
	if( x == 0 && z == 0 )
	{
		hr = terr[ 1 ];
		hu = terr[ step ];
	}
	else if( x == 0 && z == step - 1 )
	{
		hd = terr[ step * (step - 2) ];
		hr = terr[ step * (step - 1) + 1 ];
	}
	else if( x == step - 1 && z == 0 )
	{
		hl = terr[ step - 2 ];
		hu = terr[ 2 * step - 1 ];
	}
	else if( x == step - 1 && z == step - 1 )
	{
		hl = terr[ step * step - 2 ];
		hd = terr[ step * (step - 1) - 1 ];
	}
	else if( z == 0 )
	{
		hl = terr[ x - 1 ];
		hu = terr[ x + step ];
		hr = terr[ x + 1 ];
	}
	else if( z == step - 1 )
	{
		hl = terr[ x - 1 ];
		hd = terr[ (z - 1) * step + x ];
		hr = terr[ x + 1 ];
	}
	else if( x == 0 )
	{
		hu = terr[ step * (z + 1) ];
		hd = terr[ step * (z - 1) ];
		hr = terr[ step * z + 1 ];
	}
	else if( x == step - 1 )
	{
		hu = terr[ step * (z + 1) ];
		hd = terr[ step * (z - 1) ];
		hl = terr[ step * z - 1 ];
	}
	else
	{
		hl = terr[ x - 1 + step * z ];
		hr = terr[ x + 1 + step * z ];
		hd = terr[ (z + 1) * step + x ];
		hu = terr[ (z - 1) * step + x ];
	}

	//this is basically sum of normals of adjecent triangles 
	return  glm::vec3( -(hl - hr) / step, 2.0f, -(hd - hu) / step );
}

std::vector<glm::vec3> BuildMesh( std::vector<float>& height_map )
{
	/*
	Building grid from x+ to x-. z- to z+
	^
	|
	x->*/

	int step = (int)sqrt( height_map.size() );
	int start = -1 * (height_map.size() / 2)+step/2;//even points on either sides of axis 

	std::vector<glm::vec3> vertices;
	vertices.reserve( 2 * height_map.size() );

	for( int i = 0, zi = start, z = 0; i < step; i++, zi += step )
	{
		for( int j = 0, xi = -start; j < step; j++, xi -= step, z++ )
		{
			vertices.emplace_back( xi, height_map[ z ], zi );
			vertices.emplace_back( CalculateNormal( height_map, j, i, step ) );
		}
	}

	return vertices;
}

std::vector<float> CreateHeightMapFromFile( std::string file_name )
{
	/*file contains x,z,y coordinates */

	std::ifstream file( file_name );
	if( !file.is_open() )
	{
		std::cout << "File " << file_name << " not found\n";
		exit( 1 );
	}

	std::vector<float> map;
	for( int i = 0; 1; i++ )
	{
		if( file.eof() )break;

		float y;
		file >> y;
		file >> y;
		file >> y;

		map.push_back( y );
	}

	file.close();

	return map;
}

std::vector<glm::vec3> CreateTerrainFromFile( std::string file_name )
{
	//return vertices build from heigth map and grid sqrt(height map size) x sqrt(height map size) 
	return BuildMesh( CreateHeightMapFromFile( file_name ) );
}

float FindMaxHeight( std::vector<glm::vec3>& ver )
{
	float max = ver[ 0 ].y;

	for( size_t i=2;i<ver.size();i+=2)
	{
		if( ver[i].y > max ) max = ver[i].y;
	}

	return max;
}

float FindMinHeight( std::vector<glm::vec3>& ver )
{
	float min = ver[ 0 ].y;

	for( size_t i = 2; i < ver.size(); i += 2 )
	{
		if( ver[ i ].y < min ) min = ver[ i ].y;
	}

	return min;
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
	//for( GLuint i = 1; i < step; i++ )
	//{
	//	ind.push_back( i );
	//	ind.push_back( i + step - 1 );
	//}

	//ind.push_back( 2 * step - 1 );
	//ind.push_back( 2 * step - 1 );
	//ind.push_back( step );

	for( GLuint i = 0;; i++ )
	{
		ind.push_back( i );
		ind.push_back( i + step );

		if( (i + step) == size )break;
		if( (i+step+1) % step != 0 )continue;

		ind.push_back( i + step );
		ind.push_back( i + 1 );
	}

	return ind;//{ 0,3,1,4,2,5, 5,3, 3,6,4,7,5,8 };
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