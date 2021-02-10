#pragma once

#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>

#include "glew.h"
#include "freeglut.h"
#include "glm/vec3.hpp" 
#include "glm/vec4.hpp" 

std::vector<glm::vec3> CreateTerrainFromFile( std::string );
float FindMaxHeight( std::vector<glm::vec3>& ver );
float FindMinHeight( std::vector<glm::vec3>& ver );
std::vector<GLuint> BuildIndicesForTriangleStrip( GLuint size );
std::vector<GLuint> BuildIndicesForTriangles( GLuint size );