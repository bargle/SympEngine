#include "core_defs.h"
#include "VideoInterface3D.h"

//#define GL_GLEXT_PROTOTYPES
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glew.h>

//#define GL_GLEXT_PROTOTYPES
//#include <GL/glext.h>

#include <algorithm>
#include <string.h>
#include <png.h>

//temp
#include "../Platform/platform.h"

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
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			str = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_STACK_OVERFLOW:
			str = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			str = "GL_STACK_UNDERFLOW";
			break;
		case GL_TABLE_TOO_LARGE:
			str = "GL_TABLE_TOO_LARGE";
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
    unsigned short int planes;          // number of planes in image (must be 1) 
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // temporary color storage for bgr-rgb conversion.
    
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

int ImageLoadPNG_3(const char *filename, ImageData *image, bool& hasAlpha) {
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	int color_type, interlace_type;
	FILE *fp;
	
	if ((fp = fopen(filename, "rb")) == NULL)
	
		return false;
	
	
	
	/* Create and initialize the png_struct
	
	 * with the desired error handler
	
	 * functions.  If you want to use the
	
	 * default stderr and longjump method,
	
	 * you can supply NULL for the last
	
	 * three parameters.  We also supply the
	
	 * the compiler header file version, so
	
	 * that we know if the application
	
	 * was compiled with a compatible version
	
	 * of the library.  REQUIRED
	
	 */
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	
	
	if (png_ptr == NULL) {
	
		fclose(fp);
	
		return 0;
	
	}
	
	
	
	/* Allocate/initialize the memory
	
	 * for image information.  REQUIRED. */
	
	info_ptr = png_create_info_struct(png_ptr);
	
	if (info_ptr == NULL) {
	
		fclose(fp);
	
		png_destroy_read_struct(&png_ptr, NULL, NULL);
	
		return 0;
	
	}
	
	
	
	/* Set error handling if you are
	
	 * using the setjmp/longjmp method
	
	 * (this is the normal method of
	
	 * doing things with libpng).
	
	 * REQUIRED unless you  set up
	
	 * your own error handlers in
	
	 * the png_create_read_struct()
	
	 * earlier.
	
	 */
	
	if (setjmp(png_jmpbuf(png_ptr))) {
	
		/* Free all of the memory associated
	
		 * with the png_ptr and info_ptr */
	
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	
		fclose(fp);
	
		/* If we get here, we had a
	
		 * problem reading the file */
	
		return 0;
	
	}
	
	
	
	/* Set up the output control if
	
	 * you are using standard C streams */
	
	png_init_io(png_ptr, fp);
	
	
	
	/* If we have already
	
	 * read some of the signature */
	
	png_set_sig_bytes(png_ptr, sig_read);
	
	
	
	/*
	
	 * If you have enough memory to read
	
	 * in the entire image at once, and
	
	 * you need to specify only
	
	 * transforms that can be controlled
	
	 * with one of the PNG_TRANSFORM_*
	
	 * bits (this presently excludes
	
	 * dithering, filling, setting
	
	 * background, and doing gamma
	
	 * adjustment), then you can read the
	
	 * entire image (including pixels)
	
	 * into the info structure with this
	
	 * call
	
	 *
	
	 * PNG_TRANSFORM_STRIP_16 |
	
	 * PNG_TRANSFORM_PACKING  forces 8 bit
	
	 * PNG_TRANSFORM_EXPAND forces to
	
	 *  expand a palette into RGB
	
	 */
	
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

    image->sizeX = png_get_image_width( png_ptr,  info_ptr);
    image->sizeY = png_get_image_height( png_ptr,  info_ptr);

    png_byte color = png_get_color_type( png_ptr,  info_ptr);
	
	switch ( color ) {
	
		case PNG_COLOR_TYPE_RGBA:
	
			hasAlpha = true;
	
			break;
	
		case PNG_COLOR_TYPE_RGB:
	
			hasAlpha = false;
	
			break;
	
		default:
	
			//std::cout << "Color type " << info_ptr->color_type << " not supported" << std::endl;
			printf("color type not supported\n");
	
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	
			fclose(fp);
	
			return 0;
	
	}
	
	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	
	image->data = (char*) malloc(row_bytes * image->sizeY);
	
	
	
	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
	
	
	
	for (int i = 0; i < image->sizeY; i++) {
	
		// note that png is ordered top to
	
		// bottom, but OpenGL expect it bottom to top
	
		// so the order or swapped
		memcpy(image->data+(row_bytes * (/*image->sizeY-1-*/i)), row_pointers[i], row_bytes);
	}
	
	
	
	/* Clean up after the read,
	
	 * and free any memory allocated */
	
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	
	
	
	/* Close the file */
	
	fclose(fp);
	
	
	
	/* That's it */
	
	return 1;

}

int ImageLoadPNG_2(const char *filename, ImageData *image, int width, int height) {
   FILE         *infile;         /* PNG file pointer */
   png_structp   png_ptr;        /* internally used by libpng */
   png_infop     info_ptr;       /* user requested transforms */

   char         *image_data;      /* raw png image data */
   char         sig[8];           /* PNG signature array */
   /*char         **row_pointers;   */

   int           bit_depth;
   int           color_type;

   png_uint_32 _width;            /* PNG image width in pixels */
   png_uint_32 _height;           /* PNG image height in pixels */
   unsigned int rowbytes;         /* raw bytes at row n in image */

   image_data = NULL;
   int i;
   png_bytepp row_pointers = NULL;

   /* Open the file. */
   infile = fopen(filename, "rb");
   if (!infile) {
      return 0;
   }

   /*
    * 		13.3 readpng_init()
    */
	
   /* Check for the 8-byte signature */
   fread(sig, 1, 8, infile);

   if (!png_check_sig((unsigned char *) sig, 8)) {
      fclose(infile);
      return 0;
   }
 
   /* 
    * Set up the PNG structs 
    */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (!png_ptr) {
      fclose(infile);
      return 4;    /* out of memory */
   }
 
   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
      png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
      fclose(infile);
      return 4;    /* out of memory */
   }
   
  
  /*
   * block to handle libpng errors, 
   * then check whether the PNG file had a bKGD chunk
   */
   if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      fclose(infile);
      return 0;
   }

  /* 
   * takes our file stream pointer (infile) and 
   * stores it in the png_ptr struct for later use.
   */
   /* png_ptr->io_ptr = (png_voidp)infile;*/
   png_init_io(png_ptr, infile);
   
  /*
   * lets libpng know that we already checked the 8 
   * signature bytes, so it should not expect to find 
   * them at the current file pointer location
   */
   png_set_sig_bytes(png_ptr, 8);
   
   /* Read the image info.*/

  /*
   * reads and processes not only the PNG file's IHDR chunk 
   * but also any other chunks up to the first IDAT 
   * (i.e., everything before the image data).
   */

   /* read all the info up to the image data  */
   png_read_info(png_ptr, info_ptr);

   png_get_IHDR(png_ptr, info_ptr, &_width, &_height, &bit_depth, &color_type, NULL, NULL, NULL);

   image->sizeX = _width;
   image->sizeY = _height;
   
   /* Set up some transforms. */
   if (color_type & PNG_COLOR_MASK_ALPHA) {
      png_set_strip_alpha(png_ptr);
   }
   if (bit_depth > 8) {
      png_set_strip_16(png_ptr);
   }
   if (color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
      png_set_gray_to_rgb(png_ptr);
   }
   if (color_type == PNG_COLOR_TYPE_PALETTE) {
      png_set_palette_to_rgb(png_ptr);
   }
   
    if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
    	//png_set_palette_to_rgb(PNG_reader);
    	png_set_palette_to_rgb(png_ptr);
    }

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    {
    	png_set_tRNS_to_alpha(png_ptr);
    }
    else
    {
    	//png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
    }
   
   /* Update the png info struct.*/
   png_read_update_info(png_ptr, info_ptr);

   /* Rowsize in bytes. */
   rowbytes = png_get_rowbytes(png_ptr, info_ptr);


   /* Allocate the image_data buffer. */
   int data_size = rowbytes * height;
   if ((image_data = (char *) malloc( data_size ))==NULL) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return 4;
    }

   if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image_data);
        image_data = NULL;
        return 4;
    }
   

    /* set the individual row_pointers to point at the correct offsets */

    for (i = 0;  i < height;  ++i) {
        row_pointers[/*height - 1 -*/ i] = (png_byte*)(image_data + (i * rowbytes) );
    }


    /* now we can go ahead and just read the whole image */
    png_read_image(png_ptr, row_pointers);

    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */

   //*image_data_ptr = image_data;
   image->data = (char *) malloc( data_size );
   memcpy( image->data, image_data, data_size );
     
   /* Clean up. */
   free(row_pointers);

   /* Clean up. */
   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   fclose(infile);

   return 1;
}

int ImageLoadPNG(const char *filename, ImageData *image, int width, int height) {
   FILE         *infile;         /* PNG file pointer */
   png_structp   png_ptr;        /* internally used by libpng */
   png_infop     info_ptr;       /* user requested transforms */

   char         *image_data;      /* raw png image data */
   char         sig[8];           /* PNG signature array */
   /*char         **row_pointers;   */

   int           bit_depth;
   int           color_type;

   png_uint_32 _width;            /* PNG image width in pixels */
   png_uint_32 _height;           /* PNG image height in pixels */
   unsigned int rowbytes;         /* raw bytes at row n in image */

   image_data = NULL;
   int i;
   png_bytepp row_pointers = NULL;

   /* Open the file. */
   infile = fopen(filename, "rb");
   if (!infile) {
      return 0;
   }

   /*
    * 		13.3 readpng_init()
    */
	
   /* Check for the 8-byte signature */
   fread(sig, 1, 8, infile);

   if (!png_check_sig((unsigned char *) sig, 8)) {
      fclose(infile);
      return 0;
   }
 
   /* 
    * Set up the PNG structs 
    */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (!png_ptr) {
      fclose(infile);
      return 4;    /* out of memory */
   }
 
   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
      png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
      fclose(infile);
      return 4;    /* out of memory */
   }
   
  
  /*
   * block to handle libpng errors, 
   * then check whether the PNG file had a bKGD chunk
   */
   if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      fclose(infile);
      return 0;
   }

  /* 
   * takes our file stream pointer (infile) and 
   * stores it in the png_ptr struct for later use.
   */
   /* png_ptr->io_ptr = (png_voidp)infile;*/
   png_init_io(png_ptr, infile);
   
  /*
   * lets libpng know that we already checked the 8 
   * signature bytes, so it should not expect to find 
   * them at the current file pointer location
   */
   png_set_sig_bytes(png_ptr, 8);
   
   /* Read the image info.*/

  /*
   * reads and processes not only the PNG file's IHDR chunk 
   * but also any other chunks up to the first IDAT 
   * (i.e., everything before the image data).
   */

   /* read all the info up to the image data  */
   png_read_info(png_ptr, info_ptr);

   png_get_IHDR(png_ptr, info_ptr, &_width, &_height, &bit_depth, &color_type, NULL, NULL, NULL);

   image->sizeX = _width;
   image->sizeY = _height;
   
   /* Set up some transforms. */
   bool alpha = false;
   if (color_type & PNG_COLOR_MASK_ALPHA) {
      //png_set_strip_alpha(png_ptr);
      alpha = true;
   }

   if (bit_depth > 8) {
      png_set_strip_16(png_ptr);
   }
   if (color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
      png_set_gray_to_rgb(png_ptr);
   }
   if (color_type == PNG_COLOR_TYPE_PALETTE) {
      png_set_palette_to_rgb(png_ptr);
   }

   if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
   {
   	   png_set_tRNS_to_alpha(png_ptr);
   }
   else
   {
   	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
   }
   
   
	/* Update the png info struct.*/
	png_read_update_info(png_ptr, info_ptr);
	
	png_read_update_info(png_ptr, info_ptr);
	
	
	png_byte* PNG_image_buffer;
	int data_size = 0;
	if ( alpha ) {
		data_size = 4 * image->sizeX * image->sizeY;
		PNG_image_buffer = (png_byte*)malloc( data_size ); 
	} else {
		data_size = 3 * image->sizeX * image->sizeY;
		PNG_image_buffer = (png_byte*)malloc( data_size );
	}
	
	png_byte** PNG_rows = (png_byte**)malloc(image->sizeY * sizeof(png_byte*));
	
	unsigned int row;
	for (row = 0; row < image->sizeY; ++row)
	{
		if( alpha ) {
			PNG_rows[image->sizeY - 1 - row] = PNG_image_buffer + (row * 4 * image->sizeX);
			//data_size += 4 * image->sizeX;
	    } else {
			PNG_rows[image->sizeY - 1 - row] = PNG_image_buffer + (row * 3 * image->sizeX);
	    }
	}
	
	png_read_image(png_ptr, PNG_rows);
	
	image->data = (char *) malloc( data_size );
	memcpy( image->data, PNG_image_buffer, data_size );
	
	free(PNG_rows);
	
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	
	fclose(infile);

   return 1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	
	//hardcoded since everything is a quad currently
	numIdxBaked = 6;
	idxBaked = new unsigned short[numIdxBaked];

	idxBaked[0] = 3;
	idxBaked[1] = 1;
	idxBaked[2] = 0;
	idxBaked[3] = 3;
	idxBaked[4] = 2;
	idxBaked[5] = 1;

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
	
	glEnableVertexAttribArray( ATTRIB_POSITION );
	glVertexAttribPointer( ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0 /*sizeof(Vert3d_t)*/, &vertsBaked[0].pos );
    glEnableVertexAttribArray( ATTRIB_TEX );
	glVertexAttribPointer( ATTRIB_TEX, 2, GL_FLOAT, GL_FALSE, 0/*sizeof(Vert3d_t)*/, &vertsBaked[0].tex );
    glEnableVertexAttribArray( ATTRIB_COLOR );
    glVertexAttribPointer( ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0/*sizeof(Vert3d_t)*/, &vertsBaked[0].color ); //should be changed to unsigned chars
	
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
	glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo);

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

	glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
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

	#if 0
	glutInitWindowSize(nWidth,nHeight);//define the window size
    glutInitWindowPosition(10,50);//Position the window
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);//Define the drawing mode
    glutCreateWindow("SympEngine");//Create our window
    #endif
    
    Platform::createWindow("SympEngine", nWidth, nHeight, nDepth, bFullscreen );
    
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
			/*
			for ( int y = 0; y < (*iterTex).image.sizeY; y++) {
				free((*iterTex).image.data_ptrptr[y]);
			}
			free((*iterTex).image.data_ptrptr);
			*/
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

    glViewport(0,0,m_nWidth,m_nHeight);
   
    //TODO: need to regen the perspective if the height/width changes
    gluPerspective(45.0f,(GLfloat)m_nWidth/(GLfloat)m_nHeight,0.1f,1000.0f);	// Calculate The Aspect Ratio Of The WindowgluPerspective(45.0f,(GLfloat)m_nWidth/(GLfloat)m_nHeight,0.1f,1000.0f);	// Calculate The Aspect Ratio Of The Window
    
	return RESULT_OK;
}

HTEXTURE IVideoInterface3D::CreateTexture(const char* name, TextureFormat_t format){
	
    char filename[2048] = {'\0'};
    strncpy( filename, "../Resources/", 2047 );
    strncat(filename, name, 2047);
    strncat(filename, ".png", 2047);
    
	//FIXME: search our list first to see if it's already loaded... setup an std::map
	TextureList::iterator iter = m_Textures.begin();
	while(iter != m_Textures.end()) {
		std::string test_name = filename;
		if( (*iter).name == test_name ) {
			return (*iter).hTex;
		}
		++iter;
	}
	
	Texture_t tex;
	//else load it...
	bool hasAlpha = false;
	if (ImageLoadPNG_3(filename, &(tex.image), hasAlpha)) { //FIXME: load via PNGs and DON'T force 128x128 RAW RGB textures...
		// Create Texture
		glGenTextures(1, &(tex.hTex));
		glBindTexture(GL_TEXTURE_2D, tex.hTex);   // 2d texture (x and y size)

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale linearly when image smaller than texture

		// 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
		// border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
		if ( hasAlpha ) {
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tex.image.sizeX, tex.image.sizeY, GL_RGBA, GL_UNSIGNED_BYTE, tex.image.data);
		} else {
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, tex.image.sizeX, tex.image.sizeY, GL_RGB, GL_UNSIGNED_BYTE, tex.image.data);
		}
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.image.sizeX, tex.image.sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, tex.image.data);
    	glBindTexture(GL_TEXTURE_2D, 0);   // 2d texture (x and y size)	

    	tex.name = filename;
    	m_Textures.push_back(tex);
    	printf("load (%s)\n", name);
    }else {
    	//error...
    	printf("can not load (%s)\n", filename);
    }

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

HSHADER	IVideoInterface3D::CreateShaderProgram( const char* name, const char* vs, const char* fs ) {
	Shader_t shader_program;
	GLint logLength;
			
	//create program
	shader_program.program = glCreateProgram();
	
	//glBindAttribLocation( shader_program.program, POS_ATTRIB_IDX, "inPosition" );
	
	//create vert shader
	if ( vs != NULL && vs[0] != '\0' ) {
		shader_program.vertex_shader = glCreateShader( GL_VERTEX_SHADER );
		if ( shader_program.vertex_shader != 0 ) {
			int compileStatus;
			glShaderSource( shader_program.vertex_shader, 1, (const char **)&vs, NULL );
			glCompileShader( shader_program.vertex_shader );
			glGetShaderiv( shader_program.vertex_shader, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus );
			if ( !compileStatus ) {
				printf( "ERROR! Could not compile vertex shader (%s)\n", name );
				
				char* logBuffer;
				logBuffer = (char*)malloc( 16*1024 );
				glGetShaderInfoLog( shader_program.vertex_shader, 16*1024, NULL, logBuffer );
				printf( "%s\n", logBuffer );
				free( logBuffer );
	
				glDeleteShader( shader_program.vertex_shader );
				glDeleteProgram( shader_program.program );
	
				return NULL;
			}
			glAttachShader( shader_program.program, shader_program.vertex_shader );
		}
	}

	//create fragment shader
	if ( fs != NULL && fs[0] != '\0' ) {
		shader_program.fragment_shader = glCreateShader( GL_FRAGMENT_SHADER );
		if ( shader_program.fragment_shader != 0 ) {
			int compileStatus;
			glShaderSource( shader_program.fragment_shader, 1, (const char **)&fs, NULL );
			glCompileShader( shader_program.fragment_shader );
			glGetShaderiv( shader_program.fragment_shader, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus );
			if ( !compileStatus ) {
				printf( "ERROR! Could not compile fragment shader (%s)\n", name );
				
				char* logBuffer;
				logBuffer = (char*)malloc( 16*1024 );
				glGetShaderInfoLog( shader_program.fragment_shader, 16*1024, NULL, logBuffer );
				printf( "%s\n", logBuffer );
				free( logBuffer );
	
				glDeleteShader( shader_program.fragment_shader );
				glDeleteProgram( shader_program.program );
	
				return NULL;
			}
			glAttachShader( shader_program.program, shader_program.fragment_shader );
		}
	}
	
	//link program
	glLinkProgram( shader_program.program );
	glGetProgramiv( shader_program.program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(shader_program.program, logLength, &logLength, log);
		printf("Program link log:\n%s\n", log);
		free(log);
	}
	
	GLint status;
	glGetProgramiv( shader_program.program, GL_LINK_STATUS, &status);
	if (status == 0)
	{
		printf("Failed to link program\n");
		//return 0;
	}	
	
	//shader_program.uniform_mvp = glGetUniformLocation(shader_program.program , "modelViewProjectionMatrix");
	//if(shader_program.uniform_mvp < 0) {
	//	printf( "Error: failed to acquire uniform MVP\n" );
	//}
	//
	//GLint samplerLoc = glGetUniformLocation(shader_program.program, "diffuseTex");
	

	
	shader_program.tex = glGetUniformLocation(shader_program.program , "diffuseTex");
	if(shader_program.tex < 0) {
		printf( "Error: failed to acquire uniform diffuseTex\n" );
	} else {
		// Indicate that the diffuse texture will be bound to texture unit 0
		GLint unit = 0;
		glUniform1i(shader_program.tex, unit);
	}
	
	glBindAttribLocation( shader_program.program, ATTRIB_POSITION, 	"inPosition" );
	glBindAttribLocation( shader_program.program, ATTRIB_TEX, 		"inTexcoord" );
	glBindAttribLocation( shader_program.program, ATTRIB_COLOR, 	"inColor" );
	//push it back...
	
	printf("Shader linked...\n");
		
	glValidateProgram( shader_program.program );
	glGetProgramiv( shader_program.program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(shader_program.program, logLength, &logLength, log);
		printf("Program validate log:\n%s\n", log);
		free(log);
	}
	
	//glGenVertexArrays(1, &shader_program.vaoName);
	//glBindVertexArray(shader_program.vaoName);
	
	
	m_Shaders.push_front(shader_program);
	//delte it for now...
	//glDeleteShader( shader_program.vertex_shader );
	//glDeleteProgram( shader_program.program );
	return (HSHADER)&(*m_Shaders.begin());
}

GEN_RESULT IVideoInterface3D::DeleteShaderProgram( HSHADER hShader ) {
	const GLubyte* str = glGetString( GL_SHADING_LANGUAGE_VERSION );
	printf("OpenGL GLSL Version: %s\n", str);
	
	//find it in the list, remove it...
	
	return RESULT_ERROR;
}

GEN_RESULT IVideoInterface3D::InstallShaderProgram( HSHADER hShader ) {
	if ( hShader != NULL ) { 
		// TODO: validate four cc
		glUseProgram( ((Shader_t*)hShader)->program );
	}
	
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::RenderPrim2D(Prim_t *pPrim){
	//FIXME: Assume all prims are quads for now...

	if(!pPrim || (pPrim->count < 4) ) {
		return RESULT_ERROR;
	}

	if(pPrim->hTex) {
		glBindTexture(GL_TEXTURE_2D, pPrim->hTex );   // choose the texture to use.
	}

	glPushMatrix();
            /* Set coordinate system. */
    gluOrtho2D(0, m_nWidth, m_nHeight, 0);

	glTranslatef(pPrim->vPos.x, pPrim->vPos.y, pPrim->vPos.z); // (the model matrix is multiplied by the translation matrix)
	
#if 1

	glBegin(GL_QUADS);

	for(int i = 0; i < 4; ++i) {
		glColor3f(pPrim->verts[i].vColor.r, pPrim->verts[i].vColor.g, pPrim->verts[i].vColor.b);
        glTexCoord2f(pPrim->verts[i].vTexCoords.x, pPrim->verts[i].vTexCoords.y);
        glVertex2d(pPrim->verts[i].vPos.x, pPrim->verts[i].vPos.y);
    }
    
    glEnd();

#else
	glBegin(GL_TRIANGLES);
	glColor3f(pPrim->verts[0].vColor.r, pPrim->verts[0].vColor.g, pPrim->verts[0].vColor.b);
    glTexCoord2f(pPrim->verts[0].vTexCoords.x, pPrim->verts[0].vTexCoords.y);
    glVertex2d(pPrim->verts[0].vPos.x, pPrim->verts[0].vPos.y);
    
	glColor3f(pPrim->verts[1].vColor.r, pPrim->verts[1].vColor.g, pPrim->verts[1].vColor.b);
    glTexCoord2f(pPrim->verts[1].vTexCoords.x, pPrim->verts[1].vTexCoords.y);
    glVertex2d(pPrim->verts[1].vPos.x, pPrim->verts[1].vPos.y);

	glColor3f(pPrim->verts[3].vColor.r, pPrim->verts[3].vColor.g, pPrim->verts[3].vColor.b);
    glTexCoord2f(pPrim->verts[3].vTexCoords.x, pPrim->verts[3].vTexCoords.y);
    glVertex2d(pPrim->verts[3].vPos.x, pPrim->verts[3].vPos.y);    
	glEnd();
	
	glBegin(GL_TRIANGLES);
	glColor3f(pPrim->verts[3].vColor.r, pPrim->verts[3].vColor.g, pPrim->verts[3].vColor.b);
    glTexCoord2f(pPrim->verts[3].vTexCoords.x, pPrim->verts[3].vTexCoords.y);
    glVertex2d(pPrim->verts[3].vPos.x, pPrim->verts[3].vPos.y);
    
	glColor3f(pPrim->verts[1].vColor.r, pPrim->verts[1].vColor.g, pPrim->verts[1].vColor.b);
    glTexCoord2f(pPrim->verts[1].vTexCoords.x, pPrim->verts[1].vTexCoords.y);
    glVertex2d(pPrim->verts[1].vPos.x, pPrim->verts[1].vPos.y);

	glColor3f(pPrim->verts[2].vColor.r, pPrim->verts[2].vColor.g, pPrim->verts[2].vColor.b);
    glTexCoord2f(pPrim->verts[2].vTexCoords.x, pPrim->verts[2].vTexCoords.y);
    glVertex2d(pPrim->verts[2].vPos.x, pPrim->verts[2].vPos.y);    
	glEnd();
#endif

	glPopMatrix();
	
	return RESULT_OK;
}

//currently only render quads...
GEN_RESULT IVideoInterface3D::RenderPrim2DVAO(Prim_t *pPrim) {
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

	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::RenderPrim3D(Prim_t *pPrim) {
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
	
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::RenderPrim3DVBO(Prim_t *pPrim) {
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
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vert3dBaked_t), BUFFER_OFFSET( 0 ));
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vert3dBaked_t), BUFFER_OFFSET( 16 ));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vert3dBaked_t), BUFFER_OFFSET( 26 ));

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
	
	return RESULT_OK;
}

HFONT2D IVideoInterface3D::CreateHFont(const char* name, int width, int height, float char_width, float char_height, int rows, int columns) {

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
}

GEN_RESULT IVideoInterface3D::RenderText(HFONT2D hFont, const char* text, float x, float y) {
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
	
			RenderPrim2D(&prim);
		}
	}
	
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::Start3D(){
	glEnable(GL_CULL_FACE);

	if(m_ViewPort) {
		if(m_ViewPort->flags & (1<<1)) {
			glPolygonMode(GL_FRONT, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT, GL_FILL);
		}
	}

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

	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::Start2D(){
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::BeginScene() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Clear the screen
	glEnable(GL_TEXTURE_2D);			// Enable Texture Mapping

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set the blending function for translucency (note off at init time)
    glClearColor(0.25f, 0.25f, 0.25f, 0.0f);	// This Will Clear The Background Color To Grey
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);                       // type of depth test to do.
    glEnable(GL_DEPTH_TEST);                    // enables depth testing.
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
	
	return RESULT_OK;
}

GEN_RESULT IVideoInterface3D::EndScene() {
	//glutSwapBuffers();
	Platform::swapBuffers();
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

