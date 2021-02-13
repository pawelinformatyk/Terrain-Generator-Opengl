#pragma once

#include "glew.h"
#include "freeglut.h"
#include "glm/glm.hpp"

class PerlinNoise
{
private:

	int repeat=0;
	const int permutation[ 256 ] = { 151,160,137,91,90,15,					// Hash lookup table as defined by Ken Perlin.  This is a randomly
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,	// arranged array of all numbers from 0-255 inclusive.
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};
	int* perm;

	float lerp( float a, float b, float x )
	{
		return a + x * (b - a);
	}
	float fade( float t )
	{
		return ((6 * t - 15) * t + 10) * t * t * t;
	}
	float grad( int hash, float x, float y,float z )
	{
		int h = hash & 15;									// Take the hashed value and take the first 4 bits of it (15 == 0b1111)
		float u = h < 8 /* 0b1000 */ ? x : y;				// If the most significant bit (MSB) of the hash is 0 then set u = x.  Otherwise y.

		float v;											// In Ken Perlin's original implementation this was another conditional operator (?:).  I
															// expanded it for readability.

		if( h < 4 /* 0b0100 */ )								// If the first and second significant bits are 0 set v = y
			v = y;
		else if( h == 12 /* 0b1100 */ || h == 14 /* 0b1110*/ )// If the first and second significant bits are 1 set v = x
			v = x;
		else 												// If the first and second significant bits are not equal (0/1, 1/0) set v = z
			v = z;

		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v); // Use the last 2 bits to decide if u and v are positive or negative.  Then return their addition.
	}

public:

	PerlinNoise()
	{
		perm = new int[ 512 ];
		for( int x = 0; x < 512; x++ )
		{
			perm[ x ] = permutation[ x % 256 ];
		}
	}
	~PerlinNoise()
	{
		delete[] perm;
	}

	float getOctavePerlin( float x, float y, float z, int octaves, float freq = 1,float persistence=0.5f )
	{
		float total = 0;
		float frequency = freq;
		float amplitude = 1;
		float maxValue = 0;			// Used for normalizing result to 0.0 - 1.0
		for( int i = 0; i < octaves; i++ )
		{
			total += getNoise( x * frequency, y * frequency, z * frequency ) * amplitude;

			maxValue += amplitude;

			amplitude *= persistence;
			frequency *= 2;
		}

		return total / maxValue;
	}
	float getNoise( float x, float y, float z )
	{
		int xi = int( x ) & 255;
		int yi = int( y ) & 255;
		int zi = (int)z & 255;

		float xf = x - int( x );
		float yf = y - int( y );
		float zf = z - int( z );
		float u = fade( xf );
		float v = fade( yf );
		float w = fade( zf );

		int aaa = perm[ perm[ perm[ xi ] + yi ] + zi ];
		int aba = perm[ perm[ perm[ xi ] +  yi +1 ] + zi ];
		int aab = perm[ perm[ perm[ xi ] + yi ] +  zi+1 ];
		int abb = perm[ perm[ perm[ xi ] + yi+1 ] + zi +1 ];
		int baa = perm[ perm[ perm[  xi +1 ] + yi ] + zi ];
		int bba = perm[ perm[ perm[ xi + 1 ] + yi +1 ] + zi ];
		int bab = perm[ perm[ perm[ xi + 1 ] + yi ] + zi +1 ];
		int bbb = perm[ perm[ perm[ xi + 1 ] + yi +1] + zi +1 ];


		float x1 = lerp( grad( aaa, xf, yf, zf ),				// The gradient function calculates the dot product between a pseudorandom
						 grad( baa, xf - 1, yf, zf ),				// gradient vector and the vector from the input coordinate to the 8
						 u );										// surrounding points in its unit cube.
		float x2 = lerp( grad( aba, xf, yf - 1, zf ),				// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
						 grad( bba, xf - 1, yf - 1, zf ),				// values we made earlier.
						 u );
		float y1 = lerp( x1, x2, v );

		x1 = lerp( grad( aab, xf, yf, zf - 1 ),
				   grad( bab, xf - 1, yf, zf - 1 ),
				   u );
		x2 = lerp( grad( abb, xf, yf - 1, zf - 1 ),
				   grad( bbb, xf - 1, yf - 1, zf - 1 ),
				   u );
		float y2 = lerp( x1, x2, v );

		return (lerp( y1, y2, w ) + 1) / 2;
	}
};

