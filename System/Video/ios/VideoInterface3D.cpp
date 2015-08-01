#include "core_defs.h"
#include "VideoInterface3D.h"
#include <algorithm>
#include <string.h>

//temp
#include "../Platform/platform.h"

#include "imageUtil.h"

//FIXME, tmp code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Image type - contains height, width, and data */
#include <stdio.h>
//typedef struct ImageData ImageData;

static const char * GetGLErrorString(GLenum error)
{
	const char *str;
	switch( error )
	{
		case GL_NO_ERROR:
			str = "GL_NO_ERROR";
			break;
		case GL_INVALID_ENUM:
			str = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			str = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			str = "GL_INVALID_OPERATION";
			break;		
		case GL_OUT_OF_MEMORY:
			str = "GL_OUT_OF_MEMORY";
			break;
		case GL_STACK_OVERFLOW:
			str = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			str = "GL_STACK_UNDERFLOW";
			break;
		default:
			str = "(ERROR: Unknown Error Enum)";
			break;
	}
	return str;
}



int ImageLoadRAW(const char *filename, ImageData *image, int width, int height) {
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    /*
    unsigned short int planes;          // number of planes in image (must be 1) 
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // temporary color storage for bgr-rgb conversion.
    */
    image->type = IMAGE_TYPE_RAW;

    size = width * height * 3;
    
    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL)
    {
	printf("File Not Found : %s\n",filename);
	return 0;
    }
    
    image->sizeX = width;
    image->sizeY = height;

    image->data = (char *) malloc(size);
    if (image->data == NULL) {
	printf("Error allocating memory for color-corrected image data");
	return 0;	
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
	printf("Error reading image data from %s.\n", filename);
	return 0;
    }

    fclose(file);
    return 1;
}

demoImage* ImageLoadIOS(const char *filename, ImageData *image) {
    /*
     typedef struct demoImageRec
     {
     GLubyte* data;
     
     GLsizei size;
     
     GLuint width;
     GLuint height;
     GLenum format;
     GLenum type;
     
     GLuint rowByteSize;
     
     } demoImage;
     
     demoImage* imgLoadImage(const char* filepathname, int flipVertical);
     
     void imgDestroyImage(demoImage* image);

     */
    
    /*	ImageType_e type;
     unsigned long sizeX;
     unsigned long sizeY;
     char *data;
     char **data_ptrptr;*/
    
    demoImage* pImg = imgLoadImage( filename, 0 );
    if ( pImg ) {
        image->data = (char*)pImg->data;
        image->sizeX = (unsigned long)pImg->width;
        image->sizeY = (unsigned long)pImg->height;
        image->type = IMAGE_TYPE_PNG;
        return pImg;
    } 
    
    return NULL;
}
  
void ImageDeleteIOS( demoImage* pImg ) {
    imgDestroyImage( pImg );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Prim_t::~Prim_t() {
	
	if( vbo ) {
		glDeleteBuffers( 1, &vbo );
	}
	
	if( ibo ) {
		glDeleteBuffers( 1, &ibo );
	}
	
	if ( vertsBaked ) {
		delete [] vertsBaked;
		vertsBaked = NULL;
	}

	if ( idxBaked ) {
		delete [] idxBaked;
		idxBaked = NULL;
	}
	
}

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
void Prim_t::Bake() {
    /*
     Vector3d vPos;
     HTEXTURE hTex;
     unsigned long count;
     Vert3d_t *verts;
     unsigned long numVertsBaked;
     Vert3dBaked_t *vertsBaked;
     */
    
	if( numVertsBaked ) {
		numVertsBaked = 0;
	}
    
	if ( vertsBaked ) {
		delete [] vertsBaked;
		vertsBaked = NULL;
	}
    
	if ( idxBaked ) {
		delete [] idxBaked;
		idxBaked = NULL;
	}
	
	numVertsBaked = count;
	vertsBaked = new Vert3dBaked_t[count];
	
	numIdxBaked = (numVertsBaked / 4) * 6;
	idxBaked = new unsigned short[numIdxBaked];

	for ( int i = 0; i < numVertsBaked;  i += 6 ) {
		/*
        idxBaked[i + 0] = i + 3;
		idxBaked[i + 1] = i + 1;
		idxBaked[i + 2] = i + 0;
		idxBaked[i + 3] = i + 3;
		idxBaked[i + 4] = i + 2;
		idxBaked[i + 5] = i + 1;
        */
        idxBaked[i + 0] = i + 2;
		idxBaked[i + 1] = i + 1;
		idxBaked[i + 2] = i + 0;
		idxBaked[i + 3] = i + 2;
		idxBaked[i + 4] = i + 0;
		idxBaked[i + 5] = i + 3;
	}
    
	//transfer vertex data to baked array.
	for ( int i  = 0; i < count; ++i ) {
		vertsBaked[i].pos[0] = verts[i].vPos.x;
		vertsBaked[i].pos[1] = verts[i].vPos.y;
		vertsBaked[i].pos[2] = verts[i].vPos.z;
        
		vertsBaked[i].tex[0] = verts[i].vTexCoords.x;
		vertsBaked[i].tex[1] = verts[i].vTexCoords.y;
        
		vertsBaked[i].color[0] = (unsigned char)(255.0f * verts[i].vColor.r);
		vertsBaked[i].color[1] = (unsigned char)(255.0f * verts[i].vColor.g);
		vertsBaked[i].color[2] = (unsigned char)(255.0f * verts[i].vColor.b);
		vertsBaked[i].color[3] = (unsigned char)(255.0f * verts[i].vColor.a);
	}
	
	//create VBO
	if( vbo ) {
		glDeleteBuffers( 1, &vbo );
	}
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vert3dBaked_t) * numVertsBaked, vertsBaked, GL_STATIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vert3dBaked_t) * numVertsBaked, vertsBaked); // Actually upload the data
	
    /*
	glEnableVertexAttribArray( ATTRIB_POSITION );
	glVertexAttribPointer( ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vert3d_t), &vertsBaked[0].pos );
    glEnableVertexAttribArray( ATTRIB_TEX );
	glVertexAttribPointer( ATTRIB_TEX, 2, GL_FLOAT, GL_FALSE, sizeof(Vert3d_t), &vertsBaked[0].tex );
    glEnableVertexAttribArray( ATTRIB_COLOR );
    glVertexAttribPointer( ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vert3d_t), &vertsBaked[0].color ); //should be changed to unsigned chars
     */
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
	glVertexPointer(3, GL_FLOAT, 0, &vertsBaked[0].pos );
    glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, 0, &vertsBaked[0].tex );
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, &vertsBaked[0].color );

	
	//create ibo
	if( ibo ) {
		glDeleteBuffers( 1, &ibo );
	}
	glGenBuffers(1, &ibo); // Generate buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Bind the element array buffer
	// Upload the index array, this can be done the same way as above (with NULL as the data, then a
	// glBufferSubData call, but doing it all at once for convenience)
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * numIdxBaked, idxBaked, GL_STATIC_DRAW);
    
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Prim_t::BakeVAO() {
}

void Prim_t::FastBake() {
	//create VBO
	if( vbo ) {
		glDeleteBuffers( 1, &vbo );
	}
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vert3dBaked_t) * numVertsBaked, NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vert3dBaked_t) * numVertsBaked, vertsBaked); // Actually upload the data
    
	//create ibo
	if( ibo ) {
		glDeleteBuffers( 1, &ibo );
	}
	glGenBuffers(1, &ibo); // Generate buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Bind the element array buffer
	// Upload the index array, this can be done the same way as above (with NULL as the data, then a
	// glBufferSubData call, but doing it all at once for convenience)
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * numIdxBaked, idxBaked, GL_DYNAMIC_DRAW);
    
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IVideoInterface3D::IVideoInterface3D():
m_nHeight(0),
m_nWidth(0),
m_ViewPort(NULL)
{
}

IVideoInterface3D::~IVideoInterface3D(){
	Term();
}

GEN_RESULT IVideoInterface3D::Init(void* hWnd, uint32 nWidth, uint32 nHeight, uint32 nDepth, bool bFullscreen){
    //    Platform::createWindow("SympEngine", nWidth, nHeight, nDepth, bFullscreen );
    
    glClearColor(0.25,0.25,0.25,0);//Define our background color

    m_nWidth = nWidth;
    m_nHeight = nHeight;

    //glutFullScreen();

    const GLubyte* str = glGetString( GL_VERSION );
	printf("OpenGL Version: %s\n", str);

	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::Term() {

	//unbind..
	glBindTexture(GL_TEXTURE_2D, 0);   // 2d texture (x and y size)

	TextureList::iterator iterTex = m_Textures.begin();
	while( iterTex != m_Textures.end() ){
		
		if ( (*iterTex).image.type == IMAGE_TYPE_RAW ) {
			free((*iterTex).image.data);
		} else if ( (*iterTex).image.type == IMAGE_TYPE_PNG ) {
			free((*iterTex).image.data);
		}
		glDeleteTextures( 1, &((*iterTex).hTex) );
		++iterTex;
	} 

	// Font textures are deleted by the above flushing of textures.

	//close the window...
	Platform::closeWindow();
	
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::Resize(uint32 nWidth, uint32 nHeight){
	m_nWidth = nWidth;
    m_nHeight = nHeight;
	return RESULT_OK;
}

HTEXTURE IVideoInterface3D::CreateTexture(const char* name) {
    TextureList::iterator iter = m_Textures.begin();
	while(iter != m_Textures.end()) {
		std::string test_name = name;
		if( (*iter).name == test_name ) {
			return (*iter).hTex;
		}
		++iter;
	}
	
	Texture_t tex;
	//else load it...
    
    ImageData imgdata;
    demoImage* pImg = ImageLoadIOS( name, &imgdata );
    if ( pImg == NULL ) {
        return (HTEXTURE)0;
    }
    tex.image.sizeX = imgdata.sizeX;
    tex.image.sizeY = imgdata.sizeY;
    
	// Create Texture
	glGenTextures(1, &(tex.hTex));
	glBindTexture(GL_TEXTURE_2D, tex.hTex);   // 2d texture (x and y size)
        
	// 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
	// border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.

    tex.image.data = (char*)malloc( imgdata.sizeX * imgdata.sizeY * 4 );
    memcpy( tex.image.data, imgdata.data, imgdata.sizeX * imgdata.sizeY * 4 );
    ImageDeleteIOS( pImg );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.image.sizeX, tex.image.sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.image.data);
 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smaller than texture
    
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.image.sizeX, tex.image.sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, tex.image.data);
	glBindTexture(GL_TEXTURE_2D, 0);   // 2d texture (x and y size)	
    
	tex.name = name;
	m_Textures.push_back(tex);
	//printf("load (%s)\n", name);
    
    
    return tex.hTex;

}

HTEXTURE IVideoInterface3D::CreateTextureFromMemory(const char* name, uint8* buffer, int width, int height, TextureFormat_t format) {
	//FIXME: search our list first to see if it's already loaded... setup an std::map
	TextureList::iterator iter = m_Textures.begin();
	while(iter != m_Textures.end()) {
		std::string test_name = name;
		if( (*iter).name == test_name ) {
			return (*iter).hTex;
		}
		++iter;
	}
	
	Texture_t tex;
    tex.image.sizeX = width;
    tex.image.sizeY = height;
	//else load it...
    
    glEnable(GL_TEXTURE_2D);

	// Create Texture
	glGenTextures(1, &(tex.hTex));
	glBindTexture(GL_TEXTURE_2D, tex.hTex);   // 2d texture (x and y size)

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale linearly when image smaller than texture
    
	// 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
	// border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
	switch ( format ) {
	case TEXTURE_FORMAT_RGBA:
		tex.image.data = (char*)malloc( width * height * 4 );
		memcpy( tex.image.data, buffer, width * height * 4 );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.image.sizeX, tex.image.sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.image.data);
        break;
	case TEXTURE_FORMAT_RGB:
	default:
		tex.image.data = (char*)malloc( width * height * 3 );
		memcpy( tex.image.data, buffer, width * height * 3 );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.image.sizeX, tex.image.sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, tex.image.data);
		break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.image.sizeX, tex.image.sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, tex.image.data);
	glBindTexture(GL_TEXTURE_2D, 0);   // 2d texture (x and y size)	

	tex.name = name;
	m_Textures.push_back(tex);
	//printf("load (%s)\n", name);


    return tex.hTex;
}

GEN_RESULT IVideoInterface3D::DeleteTexture(HTEXTURE hTexture){
	TextureList::iterator iter = m_Textures.begin();
	while(iter != m_Textures.end()) {
		if( (*iter).hTex == hTexture ) {
			free((*iter).image.data);
			glDeleteTextures( 1, &((*iter).hTex) );
			m_Textures.erase(iter);
			break;
		}
		++iter;
	}
	
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::RenderPrim2D(Prim_t *pPrim){
	//FIXME: Assume all prims are quads for now...

	if(!pPrim || (pPrim->count < 4) ) {
		return RESULT_ERROR;
	}

	glPushMatrix();
    /* Set coordinate system. */
    glOrthof(0.0f, (float)m_nWidth, 0.0f, (float)m_nHeight, -1.0f, 1.0f);
	glTranslatef(pPrim->vPos.x, pPrim->vPos.y, pPrim->vPos.z); // (the model matrix is multiplied by the translation matrix)

	if(pPrim->hTex) {
		glBindTexture(GL_TEXTURE_2D, pPrim->hTex );   // choose the texture to use.
        //glActiveTexture(GL_TEXTURE0);
        
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vert3d_t), &pPrim->verts[0].vTexCoords );
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
    
    //iOS requires triangles. Quads are not supported.
    
	glVertexPointer(3, GL_FLOAT, sizeof(Vert3d_t), &pPrim->verts[0].vPos);
    glEnableClientState(GL_VERTEX_ARRAY);
    //glClientActiveTexture(GL_TEXTURE0);
	//glTexCoordPointer(2, GL_FLOAT, sizeof(Vert3d_t), &pPrim->verts[0].vTexCoords );
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glColorPointer(4, GL_FLOAT, sizeof(Vert3d_t), &pPrim->verts[0].vColor);
	glEnableClientState(GL_COLOR_ARRAY);

    //
    //glDrawArrays(GL_TRIANGLES, 0, pPrim->count);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, pPrim->idx);
    //
    glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glPopMatrix();
	
    if(pPrim->hTex) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    
	return RESULT_OK;
}

//currently only render quads...
GEN_RESULT IVideoInterface3D::RenderPrim2DVAO(Prim_t *pPrim) {
#if 0
	const static GLubyte indexArray[] = { 
		3,
		1,
		0,
		3,
		2,
		1
	};
	
	glBindBuffer(GL_ARRAY_BUFFER, pPrim->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pPrim->ibo);
	
	//glBindVertexArray(pPrim->vbo);
	
	if(pPrim->hTex) {
		glBindTexture(GL_TEXTURE_2D, pPrim->hTex );   // choose the texture to use.
	}
	
	Shader_t* hShader = (Shader_t*)&(*m_Shaders.begin());
	//glBindVertexArray( hShader->vaoName );
	
	glVertexAttribPointer( ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vert3d_t), &pPrim->vertsBaked[0].pos );
    glEnableVertexAttribArray( ATTRIB_POSITION );
	glVertexAttribPointer( ATTRIB_TEX, 2, GL_FLOAT, GL_FALSE, sizeof(Vert3d_t), &pPrim->vertsBaked[0].tex );
    glEnableVertexAttribArray( ATTRIB_TEX );
    glVertexAttribPointer( ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vert3d_t), &pPrim->vertsBaked[0].color ); //should be changed to unsigned chars
    glEnableVertexAttribArray( ATTRIB_COLOR );
 
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indexArray);
    
    GLenum result = glGetError();
    if ( result != GL_NO_ERROR ) {
    	printf( "GL ERROR: %d (0x%x) = %s\n", result, result, GetGLErrorString( result ) );
    	return RESULT_ERROR;
    }
#endif
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::RenderPrim3D(Prim_t *pPrim) {
#if 0
	if(!pPrim || (pPrim->count < 4) ) {
		return RESULT_ERROR;
	}

	if(pPrim->hTex) {
		glBindTexture(GL_TEXTURE_2D, pPrim->hTex );   // choose the texture to use.
	}

	glPushMatrix();
            /* Set coordinate system. */
	glTranslatef(pPrim->vPos.x, pPrim->vPos.y, pPrim->vPos.z); // (the model matrix is multiplied by the translation matrix)

	glBegin(GL_QUADS);

	for(int i = 0; i < 4; ++i) {
		glColor3f(pPrim->verts[i].vColor.r, pPrim->verts[i].vColor.g, pPrim->verts[i].vColor.b);
        glTexCoord2f(pPrim->verts[i].vTexCoords.x, pPrim->verts[i].vTexCoords.y);
        glVertex3f(pPrim->verts[i].vPos.x, pPrim->verts[i].vPos.y, pPrim->verts[i].vPos.z);
    }

	glEnd();

	glPopMatrix();
#endif
	
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::RenderPrim3DVBO(Prim_t *pPrim) {
//#if 0
	if(!pPrim || (pPrim->numIdxBaked < 6) ) {
		return RESULT_ERROR;
	}

	if( (pPrim->vbo == 0) || (pPrim->ibo == 0) ) {
		return RESULT_ERROR;
	}

	if(pPrim->hTex) {
		glBindTexture(GL_TEXTURE_2D, pPrim->hTex );   // choose the texture to use.
	}

//	glPushMatrix();
            /* Set coordinate system. */
//	glTranslatef(pPrim->vPos.x, pPrim->vPos.y, pPrim->vPos.z); // (the model matrix is multiplied by the translation matrix)

	glBindBuffer(GL_ARRAY_BUFFER, pPrim->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pPrim->ibo);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

    /*
	glVertexPointer(3, GL_FLOAT, sizeof(Vert3dBaked_t), BUFFER_OFFSET( 0 ));
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vert3dBaked_t), BUFFER_OFFSET( 16 ));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vert3dBaked_t), BUFFER_OFFSET( 26 ));
     */
    
    /*
    const GLsizeiptr vertex_size = 3 * sizeof(GLfloat);
    const GLsizeiptr tex_size = 2 * sizeof(GLfloat);
    //const GLsizeiptr color_size = 4 * sizeof(GLubyte);
    
	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)((char*)NULL) );//&vertsBaked[0].pos);
    glEnableClientState(GL_VERTEX_ARRAY);
    glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)((char*)NULL+vertex_size) );//&vertsBaked[0].tex );
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)((char*)NULL+vertex_size+tex_size) );//&vertsBaked[0].color);
	glEnableClientState(GL_COLOR_ARRAY);
     */
	glVertexPointer(3, GL_FLOAT, sizeof(Vert3dBaked_t), (void*)offsetof(Vert3dBaked_t,pos) );
    glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vert3dBaked_t), (void*)offsetof(Vert3dBaked_t,tex) );
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vert3dBaked_t), (void*)offsetof(Vert3dBaked_t,color) );
    
	glDrawElements(GL_TRIANGLES, pPrim->numIdxBaked, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

	    GLenum result = glGetError();
    if ( result != GL_NO_ERROR ) {
    	//printf( "GL ERROR: %d (0x%x)\n", result, result );
    }
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
//	glPopMatrix();
//#endif
	return RESULT_OK;
}

HFONT2D IVideoInterface3D::CreateHFont(const char* name, int width, int height, float char_width, float char_height, int rows, int columns) {
	//return 1;
	//#if 0 //stub for now...

	//load the texture.
	BitmapFont_t font;

	font.name = name;
	font.width = width;
	font.height = height;
	font.char_width = char_width;
	font.char_height = char_height;
	//font.hTex = ;
	font.columns = columns;
	font.rows = rows;

	//create tex
	font.hTex = CreateTexture( name );

	if ( font.hTex ) {
		m_Fonts.push_back(font);
		return 1; // TODO: have this return a real handle at some point
	}

	return 0;
	//#endif
}

GEN_RESULT IVideoInterface3D::RenderText(HFONT2D hFont, const char* text, float x, float y) {
//#if 0 //stub it out for now...
	
	//loop through it, make quads, setup UVs and make the render calls
	
	//For now, we'll just assume we're using the first bitmap font
	
	FontList::iterator iter =  m_Fonts.begin();
	if ( iter != m_Fonts.end() ) {
		BitmapFont_t *pFont = &(*iter);

		int len = strlen( text );
		
		Vert3d_t	aVerts[4];
		Prim_t		prim;
		float gridX, gridY;

		for( int i = 0; i < len; ++i ) {
			// let's just draw out the first char in the array...
			prim.vPos.Set( x + (12.8f * i), y, 0.0f);
			prim.hTex = pFont->hTex;
			prim.count = 4;
			prim.verts = (Vert3d_t*)&aVerts;//(Vert3d_t*)malloc(sizeof(Vert3d_t) * 4);
			
			switch( toupper(text[i]) )
			{
			case '0': {
				gridX = 0;
				gridY = 0;
			} break;
			case '1': {
				gridX = 1;
				gridY = 0;
			} break;
			case '2': {
				gridX = 2;
				gridY = 0;
			} break;
			case '3': {
				gridX = 3;
				gridY = 0;
			} break;
			case '4': {
				gridX = 4;
				gridY = 0;
			} break;
			case '5': {
				gridX = 5;
				gridY = 0;
			} break;
			case '6': {
				gridX = 6;
				gridY = 0;
			} break;
			case '7': {
				gridX = 7;
				gridY = 0;
			} break;
			case '8': {
				gridX = 8;
				gridY = 0;
			} break;
			case '9': {
				gridX = 9;
				gridY = 0;
			} break;
			
			case 'A': {
				gridX = 0;
				gridY = 1;
			} break;
			case 'B': {
				gridX = 1;
				gridY = 1;
			} break;
			case 'C': {
				gridX = 2;
				gridY = 1;
			} break;	
			case 'D': {
				gridX = 3;
				gridY = 1;
			} break;
			case 'E': {
				gridX = 4;
				gridY = 1;
			} break;
			case 'F': {
				gridX = 5;
				gridY = 1;
			} break;
			case 'G': {
				gridX = 6;
				gridY = 1;
			} break;
			case 'H': {
				gridX = 7;
				gridY = 1;
			} break;
			case 'I': {
				gridX = 8;
				gridY = 1;
			} break;
			case 'J': {
				gridX = 9;
				gridY = 1;
			} break;
			
			case 'K': {
				gridX = 0;
				gridY = 2;
			} break;
			case 'L': {
				gridX = 1;
				gridY = 2;
			} break;
			case 'M': {
				gridX = 2;
				gridY = 2;
			} break;
			case 'N': {
				gridX = 3;
				gridY = 2;
			} break;
			case 'O': {
				gridX = 4;
				gridY = 2;
			} break;
			case 'P': {
				gridX = 5;
				gridY = 2;
			} break;
			case 'Q': {
				gridX = 6;
				gridY = 2;
			} break;
			case 'R': {
				gridX = 7;
				gridY = 2;
			} break;
			case 'S': {
				gridX = 8;
				gridY = 2;
			} break;
			case 'T': {
				gridX = 9;
				gridY = 2;
			} break;
			case 'U': {
				gridX = 0;
				gridY = 3;
			} break;	
			case 'V': {
				gridX = 1;
				gridY = 3;
			} break;
			case 'W': {
				gridX = 2;
				gridY = 3;
			} break;
			case 'X': {
				gridX = 3;
				gridY = 3;
			} break;
			case 'Y': {
				gridX = 4;
				gridY = 3;
			} break;
			case 'Z': {
				gridX = 5;
				gridY = 3;
			} break;	
			//:.-+
			case ':': {
				gridX = 6;
				gridY = 3;
			} break;
			case '.': {
				gridX = 7;
				gridY = 3;
			} break;
			case '-': {
				gridX = 8;
				gridY = 3;
			} break;
			case '+': {
				gridX = 9;
				gridY = 3;
			} break;
			//!@#$%^&*()
			case '!': {
				gridX = 0;
				gridY = 4;
			} break;
			case '@': {
				gridX = 1;
				gridY = 4;
			} break;
			case '#': {
				gridX = 2;
				gridY = 4;
			} break;
			case '$': {
				gridX = 3;
				gridY = 4;
			} break;
			case '%': {
				gridX = 4;
				gridY = 4;
			} break;
			case '^': {
				gridX = 5;
				gridY = 4;
			} break;
			case '&': {
				gridX = 6;
				gridY = 4;
			} break;
			case '*': {
				gridX = 7;
				gridY = 4;
			} break;
			case '(': {
				gridX = 8;
				gridY = 4;
			} break;
			case ')': {
				gridX = 9;
				gridY = 4;
			} break;
			default:
				gridX = 0;
				gridY = 5;
				break;
			}
	
            /*
			prim.verts[0].vPos.Set(0.0f, 0.0f, 0.0f);
			prim.verts[0].vTexCoords.Set(gridX * 0.1f, gridY * 0.171875f);
			prim.verts[0].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	
			prim.verts[1].vPos.Set(12.8f, 0.0f, 0.0f);
			prim.verts[1].vTexCoords.Set((gridX+1) * 0.1f, gridY * 0.171875f);
			prim.verts[1].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	
			prim.verts[2].vPos.Set(12.8f, 22.0f, 0.0f);
			prim.verts[2].vTexCoords.Set((gridX+1) * 0.1f, (gridY + 1) * 0.171875f);
			prim.verts[2].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	
			prim.verts[3].vPos.Set(0.0f, 22.0f, 0.0f);
			prim.verts[3].vTexCoords.Set(gridX * 0.1f, (gridY + 1) * 0.171875f);
			prim.verts[3].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
            */
            
			prim.verts[0].vPos.Set(0.0f, 0.0f, 0.0f);
			prim.verts[0].vTexCoords.Set(gridX * 0.1f, (gridY + 1) * 0.171875f);
			prim.verts[0].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
            
			prim.verts[1].vPos.Set(12.8f, 0.0f, 0.0f);
			prim.verts[1].vTexCoords.Set((gridX+1) * 0.1f, (gridY + 1) * 0.171875f);
			prim.verts[1].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
            
			prim.verts[2].vPos.Set(12.8f, 22.0f, 0.0f);
            prim.verts[2].vTexCoords.Set((gridX+1) * 0.1f, gridY * 0.171875f);
			prim.verts[2].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
            
			prim.verts[3].vPos.Set(0.0f, 22.0f, 0.0f);
			prim.verts[3].vTexCoords.Set(gridX * 0.1f, gridY * 0.171875f);
			prim.verts[3].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
            
            prim.idx[0] = 3;
            prim.idx[1] = 1;
            prim.idx[2] = 0;
            prim.idx[3] = 3;
            prim.idx[4] = 2;
            prim.idx[5] = 1;
	
			RenderPrim2D(&prim);
		}
	}
//#endif
	return RESULT_OK;
}

/////////////////////////////////
void __gluMakeIdentityf(GLfloat m[16])
{
    m[0+4*0] = 1.0f; m[0+4*1] = 0.0f; m[0+4*2] = 0.0f; m[0+4*3] = 0.0f;
    m[1+4*0] = 0.0f; m[1+4*1] = 1.0f; m[1+4*2] = 0.0f; m[1+4*3] = 0.0f;
    m[2+4*0] = 0.0f; m[2+4*1] = 0.0f; m[2+4*2] = 1.0f; m[2+4*3] = 0.0f;
    m[3+4*0] = 0.0f; m[3+4*1] = 0.0f; m[3+4*2] = 0.0f; m[3+4*3] = 1.0f;
}

void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
    GLfloat m[4][4];
    GLfloat sine, cotangent, deltaZ;
    GLfloat radians = fovy / 2.0f * 3.14f / 180.0f;
    
    deltaZ = zFar - zNear;

    sine = sin(radians);
    if ((deltaZ == 0.0f) || (sine == 0.0f) || (aspect == 0.0f))
    {
        return;
    }
    cotangent = cos(radians) / sine;
    
    __gluMakeIdentityf(&m[0][0]);
    m[0][0] = cotangent / aspect;
    m[1][1] = cotangent;
    m[2][2] = -(zFar + zNear) / deltaZ;
    m[2][3] = -1.0f;
    m[3][2] = -2.0f * zNear * zFar / deltaZ;
    m[3][3] = 0.0f;
    glMultMatrixf(&m[0][0]);
}
/////////////////////////////////


GEN_RESULT IVideoInterface3D::Start3D(){
	glEnable(GL_CULL_FACE);
//#if 0
    /*
	if(m_ViewPort) {
		if(m_ViewPort->flags & (1<<1)) {
			glPolygonMode(GL_FRONT, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT, GL_FILL);
		}
	}
    */

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix

    gluPerspective(60.0f,(GLfloat)m_nWidth/(GLfloat)m_nHeight,0.1f,1000.0f);	// Calculate The Aspect Ratio Of The Window
    glMatrixMode(GL_MODELVIEW);

    if(m_ViewPort) {
    	glLoadIdentity();
    	//glLoadMatrixf(m_ViewPort->rRotation.m);
    	glRotatef(360.0f - m_ViewPort->pitch, 1.0f, 0.0f,0);
    	glRotatef(360.0f - m_ViewPort->yaw,0,1.0f,0);
    	glTranslatef(m_ViewPort->vPos.x, m_ViewPort->vPos.y, m_ViewPort->vPos.z);
    }
//#endif
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::Start2D(){
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	//glBlendFunc(GL_SRC_ALPHA, GL_SRC_ONE_MINUS_ALPHA);
	//glEnable(GL_BLEND);
	
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::BeginScene() {

	glClear(GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/);//Clear the screen
	glEnable(GL_TEXTURE_2D);			// Enable Texture Mapping

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set the blending function for translucency (note off at init time)
    glClearColor(0.25f, 0.25f, 0.25f, 0.0f);	// This Will Clear The Background Color To Grey
    //glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);                       // type of depth test to do.
    glEnable(GL_DEPTH_TEST);                    // enables depth testing.
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
    
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::EndScene() {
	//glutSwapBuffers();
	//Platform::swapBuffers();
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::SetViewport(ViewPort* vp) {
	m_ViewPort = vp;
	return RESULT_OK;
}

ViewPort* IVideoInterface3D::GetViewport() {
	return m_ViewPort;
}

GEN_RESULT	IVideoInterface3D::EnableBlend( bool blend ) {
	if ( blend ) {
		glEnable( GL_BLEND );
	} else {
		glDisable( GL_BLEND );
	}
	return RESULT_OK;
}

