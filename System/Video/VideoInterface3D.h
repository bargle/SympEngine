#ifndef _VIDEO_INTERFACE_3D_BASE_H_
#define _VIDEO_INTERFACE_3D_BASE_H_

#include <list>
#include <vector>
#include <string>
//FIXME: this can not use GLuint if we want to shield the game from glut.
#define GL_GLEXT_PROTOTYPES

#ifdef __APPLE__

#ifdef __MACH__
	#include <OpenGL/OpenGL.h>
	
	//#if CGL_VERSION_1_3
	//	#include <OpenGL/gl3.h>
	//	#include <OpenGL/gl3ext.h>
	//#else
		#include <OpenGL/gl.h>
		#include <OpenGL/glu.h>
		#define glBindVertexArray glBindVertexArrayAPPLE 
		#define glGenVertexArrays glGenVertexArraysAPPLE
		#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
	//#endif
#else
//#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#if 0
	#include <OpenGL/OpenGL.h>
	
	//#if CGL_VERSION_1_3
	//	#include <OpenGL/gl3.h>
	//	#include <OpenGL/gl3ext.h>
	//#else
		#include <OpenGL/gl.h>
		#include <OpenGL/glu.h>
		#define glBindVertexArray glBindVertexArrayAPPLE 
		#define glGenVertexArrays glGenVertexArraysAPPLE
		#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
	//#endif
#endif
#endif

#else

	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GL/glu.h>

#endif
#if 0
/* INLINE THIS */ unsigned int MAKE_FOURCC( char a, char b, char c, char d ) { 
	return ( ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | ( d ) );
}
#endif
#define _MAKE_FOURCC( a, b, c, d ) ( ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | ( d ) )

typedef unsigned int INTERNAL_TYPE;

typedef GLuint HTEXTURE;

enum TextureFormat_t {
	TEXTURE_FORMAT_RGB = 0,
	TEXTURE_FORMAT_RGBA,
	TEXTURE_FORMAT_COUNT
};

enum ImageType_e {
	IMAGE_TYPE_RAW = 0,
	IMAGE_TYPE_PNG,
	IMAGE_TYPE_MAX
};

struct ImageData{
	ImageType_e type;
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
    char **data_ptrptr;
};

struct Texture_t{
	std::string name;
	HTEXTURE hTex;
	ImageData image;
};

typedef std::list<Texture_t> TextureList;

struct Shader_t{
	Shader_t() : iType( _MAKE_FOURCC( 'S', 'H', 'D', 'R' ) ){
		//iType = MAKE_FOURCC( 'S', 'H', 'D', 'R' );
	}
	INTERNAL_TYPE	iType;
	GLuint			program;
	GLuint			vertex_shader;
	GLuint			fragment_shader;
	GLint			uniform_mvp; // Model View Projection matrix ID
	GLint			tex;
	GLuint			vaoName;
};

typedef void* HSHADER; //opaque handle

typedef std::list<Shader_t> ShaderList;

//TODO add materials...

struct BitmapFont_t {
	std::string name;
	int width;
	int height;
	float char_width;
	float char_height;
	HTEXTURE hTex;
	int columns;
	int rows;
};

typedef std::vector<BitmapFont_t> FontList;
typedef unsigned int HFONT2D;

struct Vert3d_t
{
	Vector3d vPos;
	Vector2d vTexCoords;
	ColorRGBA vColor;
};

/*
struct Vert3dBaked_t
{
	float pos[4];
	float tex[2];
	unsigned char padding1[2];
	unsigned char color[4];
	unsigned char padding2[2];
};
 */
struct Vert3dBaked_t
{
	float pos[3];
	float tex[2];
	unsigned char color[4];
};

enum VertexAttributes{
   ATTRIB_POSITION,
   ATTRIB_TEX,
   ATTRIB_COLOR,
   NUM_ATTRIBUTES };

class ViewPort {
public:
	ViewPort(): 
	yaw(0.0f),
	pitch(0.0f),
	vPos(0.0f, 0.0f, 0.0f),
	vDir(0.0f, 0.0f, 0.0f),
	flags(0)
	{
		rRotation.Identity();
	}

	float yaw;
	float pitch;
	Vector3d vPos;
	Vector3d vDir;
	Matrix4x4 rRotation;
	unsigned int flags;
};
#if defined(_MSC_VER)
	#include "win32/VideoInterface3D_Impl.h"
#elif defined(__IOS)
    #include "ios/VideoInterface3D_Impl.h"
#elif defined( __APPLE__ ) && defined( __MACH__ )
	#include "osx/VideoInterface3D_Impl.h"
#elif defined(__GNUC__)
	#include "unix/VideoInterface3D_Impl.h"
#else
#error platform not supported!	
#endif

#endif