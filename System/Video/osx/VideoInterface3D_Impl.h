#ifndef _VIDEO_INTERFACE_3D_H_
#define _VIDEO_INTERFACE_3D_H_

class Prim_t{
public: 
	Prim_t():
	hTex(0),
	count(0),
	verts(NULL),
	numVertsBaked(0),
	vertsBaked(NULL),
	numIdxBaked(0),
	idxBaked(NULL),
	vbo(0),
	ibo(0)
	{}
	
	~Prim_t();

	void Bake();
	void BakeVAO();
	void FastBake();
	
	Vector3d vPos;
	HTEXTURE hTex;
	unsigned long count;
	Vert3d_t *verts;
	unsigned long numVertsBaked;
	Vert3dBaked_t *vertsBaked;
	unsigned long numIdxBaked;
	unsigned short *idxBaked;
	unsigned int vbo;
	unsigned int ibo;
    unsigned short idx[6];
};

class IVideoInterface3D
{
public:
	IVideoInterface3D();
	virtual ~IVideoInterface3D();

	virtual GEN_RESULT	Init(void* hWnd, uint32 nWidth, uint32 nHeight, uint32 nDepth, bool bFullscreen);
	virtual GEN_RESULT	Term();
	virtual GEN_RESULT	Resize(uint32 nWidth, uint32 nHeight);

	virtual HTEXTURE	CreateTexture(const char* name, TextureFormat_t format = TEXTURE_FORMAT_RGB);
	virtual GEN_RESULT	DeleteTexture(HTEXTURE hTexture);
	virtual HSHADER		CreateShaderProgram( const char* name, const char* vs, const char* fs );
	virtual GEN_RESULT	DeleteShaderProgram( HSHADER hShader );	
	virtual GEN_RESULT	InstallShaderProgram( HSHADER hShader );
	virtual GEN_RESULT	RenderPrim2D(Prim_t *pPrim);
	virtual GEN_RESULT	RenderPrim2DVAO(Prim_t *pPrim);
	virtual GEN_RESULT	RenderPrim3D(Prim_t *pPrim);
	virtual GEN_RESULT	RenderPrim3DVBO(Prim_t *pPrim);
	

	virtual HFONT2D		CreateHFont(const char* name, int width, int height, float char_width, float char_height, int rows, int columns);
	virtual GEN_RESULT	RenderText(HFONT2D hFont, const char* text, float x, float y);
	
	virtual GEN_RESULT	Start3D();
	virtual GEN_RESULT	Start2D();
    virtual GEN_RESULT  End2D();
	virtual GEN_RESULT	BeginScene();
	virtual GEN_RESULT	EndScene();
	
	virtual GEN_RESULT	EnableBlend( bool blend );

	virtual GEN_RESULT	SetViewport(ViewPort* vp);
	virtual ViewPort*	GetViewport();
	
protected:
	int m_hWnd;
	int m_nHeight;
	int m_nWidth;
	TextureList m_Textures;
	ShaderList m_Shaders;
	ViewPort *m_ViewPort;
	FontList m_Fonts;
};

#endif
