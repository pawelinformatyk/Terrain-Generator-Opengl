#include <stdio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "texture.h"
#include "stb_image.h"

Texture::Texture( const char* file_name )
{
/*  GLuint num_texture;
    int i, j = 0;
    FILE* l_file=NULL;
    unsigned char* l_texture=NULL;
    BITMAPFILEHEADER fileheader;
    BITMAPINFOHEADER infoheader;
    RGBTRIPLE rgb;

    glGenTextures( 1, &num_texture );

    fopen_s( &l_file, file_name, "rb" );
    if( !l_file )
    {
        MessageBox( NULL, "Texture file not found", "Problem", MB_OK | MB_ICONERROR );
        exit( 1 );
    }

    fread( &fileheader, sizeof( fileheader ), 1, l_file );
    fseek( l_file, sizeof( fileheader ), SEEK_SET );
    fread( &infoheader, sizeof( infoheader ), 1, l_file );

    l_texture = (byte*)malloc( infoheader.biWidth * infoheader.biHeight * 4 );
    if( !l_texture )
    {
        MessageBox( NULL, "Malloc texture", "Problem", MB_OK | MB_ICONERROR );
        exit( 1 );
    }

    memset( l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4 );

    // loading pixels
    for( i = 0; i < infoheader.biWidth * infoheader.biHeight; i++ )
    {
        fread( &rgb, sizeof( rgb ), 1, l_file );
        l_texture[ j + 0 ] = rgb.rgbtRed;
        l_texture[ j + 1 ] = rgb.rgbtGreen;
        l_texture[ j + 2 ] = rgb.rgbtBlue;
        l_texture[ j + 3 ] = 255; // Alpha
        j += 4;
    }
    fclose( l_file );

    glBindTexture( GL_TEXTURE_2D, num_texture );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // Tiling texture
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );

    // create texture in opengl
    glTexImage2D( GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 4, infoheader.biWidth, infoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, l_texture );

    free( l_texture );

    ID = num_texture;
    */
    glGenTextures( 1, &ID );
    glBindTexture( GL_TEXTURE_2D, ID ); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
                                        // set the texture wrapping parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // set texture filtering parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load( file_name, &width, &height, &nrChannels, 0 );
    
    if( data )
    {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
        glGenerateMipmap( GL_TEXTURE_2D );
    }
    else
    {
        printf("Failed to load texture");
    }

    stbi_image_free( data );
}

