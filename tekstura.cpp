/*
* Wczytywanie tekstury
*/

#include "tekstura.h"
GLuint  num_texture=-1;

GLuint   WczytajTeksture (const char *filename)
{
    // Pomocnicze
	int i, j=0;
    FILE *l_file;
    unsigned char *l_texture; 
    BITMAPFILEHEADER fileheader; 
    BITMAPINFOHEADER infoheader;
    RGBTRIPLE rgb;
    glGenTextures(1, &num_texture);

	fopen_s(&l_file, filename, "rb");
    if( l_file==NULL) return (-1);
    // Nag³ówek:
	fread(&fileheader, sizeof(fileheader), 1, l_file); 
    fseek(l_file, sizeof(fileheader), SEEK_SET); 
    fread(&infoheader, sizeof(infoheader), 1, l_file); 
	// Alokacja pamiêci
    l_texture = (byte *) malloc(infoheader.biWidth * infoheader.biHeight * 4);
    memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);
 
    // Wczytywanie pikseli
	for (i=0; i < infoheader.biWidth*infoheader.biHeight; i++)
    {            
            fread(&rgb, sizeof(rgb), 1, l_file); 
            l_texture[j+0] = rgb.rgbtRed; 
            l_texture[j+1] = rgb.rgbtGreen; 
            l_texture[j+2] = rgb.rgbtBlue; 
            l_texture[j+3] = 255; // Alpha
            j += 4; 
    }
	fclose(l_file); 
     
    glBindTexture(GL_TEXTURE_2D, num_texture); // Zg³oszenie tekstury (poprzez jej unikatowe ID)
    // Ustawienie parametrów rekstury
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Tiling tekstury, gdy za ma³a
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 


    // Stwórz taka teksture w OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoheader.biWidth, infoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

    free(l_texture); 

    return (num_texture); // Zwróæ identyfikator OpenGL ID tekstury 
}

