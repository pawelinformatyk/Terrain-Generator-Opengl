#pragma once

#include <iostream>
#include <array>
#include <string>

#include "glew.h"
#include "freeglut.h"
#include "glm/glm.hpp" 

#include "stb_image.h"

class Skybox
{
public:
	Skybox( std::array<std::string,6>&faces )
	{
		ID = loadCubemap( faces );

		glGenVertexArrays( 1, &VAO);
		glGenBuffers( 1, &VBO );
		glBindVertexArray( VAO );
		glBindBuffer( GL_ARRAY_BUFFER, VBO );
		glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), &vertices, GL_STATIC_DRAW );
		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), (void*)0 );
	}
	~Skybox()
	{
		glDeleteVertexArrays( 1, &VAO );
		glDeleteBuffers( 1, &VBO );
	}
	void Draw()
	{
		glDepthFunc( GL_LEQUAL );  // change depth function so depth test passes when values are equal to depth buffer's content

		glBindVertexArray( VAO );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_CUBE_MAP, ID );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glDrawArrays( GL_TRIANGLES, 0, 36 );
		glBindVertexArray( 0 );

		glDepthFunc( GL_LESS ); // set depth function back to default
	}

private:
	unsigned int ID;
	std::vector<std::string> faces;
	unsigned int VAO, VBO;
	const float vertices[108]={
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int loadCubemap( std::array<std::string, 6>& faces )
	{
		unsigned int textureID;
		glGenTextures( 1, &textureID );
		glBindTexture( GL_TEXTURE_CUBE_MAP, textureID );

		int width, height, nrChannels;
		for( unsigned int i = 0; i < faces.size(); i++ )
		{
			unsigned char* data = stbi_load( faces[ i ].c_str(), &width, &height, &nrChannels, 0 );
			if( data )
			{
				glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
				stbi_image_free( data );
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[ i ] << std::endl;
				stbi_image_free( data );
			}
		}
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

		return textureID;
	}
};

