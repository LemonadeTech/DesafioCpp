//
//  tga.h
//  ProjetoSDL
//
//  Copyright © 2015 Lemonade. All rights reserved.
//

#ifndef tga_h
#define tga_h

#ifdef __APPLE__
#include <unistd.h>
#endif


///////////////////////////////////////////////////////
// Macros for big/little endian happiness
// These are intentionally written to be easy to understand what they
// are doing... no flames please on the inefficiency of these.
#ifdef __BIG_ENDIAN__
///////////////////////////////////////////////////////////
// This function says, "this pointer is a little endian value"
// If the value must be changed it is... otherwise, this
// function is defined away below (on Intel systems for example)
inline void LITTLE_ENDIAN_WORD(void *pWord)
{
    unsigned char *pBytes = (unsigned char *)pWord;
    unsigned char temp;
    
    temp = pBytes[0];
    pBytes[0] = pBytes[1];
    pBytes[1] = temp;
}

///////////////////////////////////////////////////////////
// This function says, "this pointer is a little endian value"
// If the value must be changed it is... otherwise, this
// function is defined away below (on Intel systems for example)
inline void LITTLE_ENDIAN_DWORD(void *pWord)
{
    unsigned char *pBytes = (unsigned char *)pWord;
    unsigned char temp;
    
    // Swap outer bytes
    temp = pBytes[3];
    pBytes[3] = pBytes[0];
    pBytes[0] = temp;
    
    // Swap inner bytes
    temp = pBytes[1];
    pBytes[1] = pBytes[2];
    pBytes[2] = temp;
}
#else
// Define them away on little endian systems
#define LITTLE_ENDIAN_WORD
#define LITTLE_ENDIAN_DWORD
#endif


// Define targa header. This is only used locally.
#pragma pack(1)
typedef struct
{
    GLbyte      identsize;              // Size of ID field that follows header (0)
    GLbyte      colorMapType;           // 0 = None, 1 = paletted
    GLbyte      imageType;              // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
    unsigned short      colorMapStart;          // First colour map entry
    unsigned short      colorMapLength;         // Number of colors
    unsigned char       colorMapBits;   // bits per palette entry
    unsigned short      xstart;                 // image x origin
    unsigned short      ystart;                 // image y origin
    unsigned short      width;                  // width in pixels
    unsigned short      height;                 // height in pixels
    GLbyte      bits;                   // bits per pixel (8 16, 24, 32)
    GLbyte      descriptor;             // image descriptor
} TGAHEADER;
#pragma pack(8)

GLbyte *gltReadTGABits(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat, GLbyte *pData)
{
    FILE *pFile;                        // File pointer
    TGAHEADER tgaHeader;                // TGA file header
    unsigned long lImageSize;           // Size in bytes of image
    short sDepth;                       // Pixel depth;
    GLbyte      *pBits = NULL;          // Pointer to bits
    
    // Default/Failed values
    *iWidth = 0;
    *iHeight = 0;
    *eFormat = GL_RGB;
    *iComponents = GL_RGB;
    
    // Attempt to open the file
    pFile = fopen(szFileName, "rb");
    if(pFile == NULL)
        return NULL;
    
    // Read in header (binary)
    fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1, pFile);
    
    // Do byte swap for big vs little endian
#ifdef __APPLE__
    LITTLE_ENDIAN_WORD(&tgaHeader.colorMapStart);
    LITTLE_ENDIAN_WORD(&tgaHeader.colorMapLength);
    LITTLE_ENDIAN_WORD(&tgaHeader.xstart);
    LITTLE_ENDIAN_WORD(&tgaHeader.ystart);
    LITTLE_ENDIAN_WORD(&tgaHeader.width);
    LITTLE_ENDIAN_WORD(&tgaHeader.height);
#endif
    
    
    // Get width, height, and depth of texture
    *iWidth = tgaHeader.width;
    *iHeight = tgaHeader.height;
    sDepth = tgaHeader.bits / 8;
    
    // Put some validity checks here. Very simply, I only understand
    // or care about 8, 24, or 32 bit targa's.
    if(tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32)
        return NULL;
    
    // Calculate size of image buffer
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
    
    // Allocate memory and check for success
    if(pData == NULL)
        pBits = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
    else
        pBits = pData;
    
    // Read in the bits
    // Check for read error. This should catch RLE or other
    // weird formats that I don't want to recognize
    if(fread(pBits, lImageSize, 1, pFile) != 1)
    {
        if(pBits != NULL)
            free(pBits);
        return NULL;
    }
    
    // Set OpenGL format expected
    switch(sDepth)
    {
#ifndef OPENGL_ES
        case 3:     // Most likely case
            *eFormat = GL_BGR;
            *iComponents = GL_RGB;
            break;
#endif
        case 4:
            *eFormat = GL_BGRA;
            *iComponents = GL_RGBA;
            break;
        case 1:
            *eFormat = GL_LUMINANCE;
            *iComponents = GL_LUMINANCE;
            break;
        default:        // RGB
                        // If on the iPhone, TGA's are BGR, and the iPhone does not
                        // support BGR without alpha, but it does support RGB,
                        // so a simple swizzle of the red and blue bytes will suffice.
                        // For faster iPhone loads however, save your TGA's with an Alpha!
#ifdef OPENGL_ES
            for(int i = 0; i < lImageSize; i+=3)
            {
                GLbyte temp = pBits[i];
                pBits[i] = pBits[i+2];
                pBits[i+2] = temp;
            }
#endif
            break;
    }
    
    // Done with File
    fclose(pFile);
    
    // Return pointer to image data
    return pBits;
}


#endif /* tga_h */
