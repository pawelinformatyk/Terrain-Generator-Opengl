#pragma once

#include <iostream>
#include <string.h>

#include "glew.h"
#include "freeglut.h"
#include "glm/glm.hpp"

class Shader
{
private:
	unsigned int ID;

public:

	Shader( const char* vertex_path, const char* fragment_path,const char* geometry_path=NULL );
	
    unsigned int getID()const{ return ID; };
    void use()
    {
        glUseProgram( ID );
    }
    void setBool( const std::string& name, bool value ) const
    {
        glUniform1i( glGetUniformLocation( ID, name.c_str() ), (int)value );
    }
    void setInt( const std::string& name, int value ) const
    {
        glUniform1i( glGetUniformLocation( ID, name.c_str() ), value );
    }
    void setFloat( const std::string& name, float value ) const
    {
        glUniform1f( glGetUniformLocation( ID, name.c_str() ), value );
    }
    void setVec2( const std::string& name, const glm::vec2& value ) const
    {
        glUniform2fv( glGetUniformLocation( ID, name.c_str() ), 1, &value[ 0 ] );
    }
    void setVec2( const std::string& name, float x, float y ) const
    {
        glUniform2f( glGetUniformLocation( ID, name.c_str() ), x, y );
    }
    void setVec3( const std::string& name, const glm::vec3& value ) const
    {
        glUniform3fv( glGetUniformLocation( ID, name.c_str() ), 1, &value[ 0 ] );
    }
    void setVec3( const std::string& name, float x, float y, float z ) const
    {
        glUniform3f( glGetUniformLocation( ID, name.c_str() ), x, y, z );
    }
    void setVec4( const std::string& name, const glm::vec4& value ) const
    {
        glUniform4fv( glGetUniformLocation( ID, name.c_str() ), 1, &value[ 0 ] );
    }
    void setVec4( const std::string& name, float x, float y, float z, float w )
    {
        glUniform4f( glGetUniformLocation( ID, name.c_str() ), x, y, z, w );
    }
    void setMat2( const std::string& name, const glm::mat2& mat ) const
    {
        glUniformMatrix2fv( glGetUniformLocation( ID, name.c_str() ), 1, GL_FALSE, &mat[ 0 ][ 0 ] );
    }
    void setMat3( const std::string& name, const glm::mat3& mat ) const
    {
        glUniformMatrix3fv( glGetUniformLocation( ID, name.c_str() ), 1, GL_FALSE, &mat[ 0 ][ 0 ] );
    }
    void setMat4( const std::string& name, const glm::mat4& mat ) const
    {
        glUniformMatrix4fv( glGetUniformLocation( ID, name.c_str() ), 1, GL_FALSE, &mat[ 0 ][ 0 ] );
    }

private:
	/*
	* Returns a string containing the text in
	* a vertex/fragment shader source file.
	*/
	static char* shaderLoadSource( const char* file_path );
	/*
	* Returns a shader object containing a shader
	* compiled from the given GLSL shader file.
	*/
	static GLuint shaderCompileFromFile( GLenum type, const char* file_path );
	/*
	* Compiles and attaches a shader of the
	* given type to the given program object.
	*/
	void shaderAttachFromFile( GLuint program, GLenum type, const char* file_path );
	int loadShaders( const char* vertex_path, const char* fragment_path, const char* geometry_path = NULL );


};