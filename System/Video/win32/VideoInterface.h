#ifndef _VIDEO_INTERFACE_H_
#define _VIDEO_INTERFACE_H_

#include <ddraw.h>
#include <list>
#include <d3d8.h>
#include <d3dx8.h>

typedef void* HSURFACE;

class AlphaMap
{
public:
	AlphaMap():
	x(0),
	y(0),
	pData(NULL)
	{
	}

	~AlphaMap()
	{
		if(pData)
		{
			delete [] pData;
		}
	}

	int x, y;
	uint8** pData;
};

enum EFontStyle
{
	FONTSTYLE_NORMAL = 0,
	FONTSTYLE_BOLD,
	FONTSTYLE_ITALIC,
	FONTSTYLE_BOLDITALIC
};

class IVideoInterface
{
public:
	IVideoInterface();
	virtual ~IVideoInterface();

	virtual GEN_RESULT Init(void* hWnd, uint32 nWidth, uint32 nHeight, uint32 nDepth, bool bFullscreen);
	virtual GEN_RESULT Term();

	virtual GEN_RESULT BeginScene();
	virtual GEN_RESULT EndScene();

	// Surface functions
	virtual GEN_RESULT CreateSurface(HSURFACE &hSurface, int x, int y);
	virtual GEN_RESULT CreateSurfaceFromRaw(HSURFACE &hSurface, int x, int y, const char* szFilename);
	virtual GEN_RESULT CreateSurfaceFromFile(HSURFACE &hSurface, int *x, int *y, const char* szFilename);
	virtual GEN_RESULT GetSurfaceDims(HSURFACE hSurface, int *x, int *y);
	virtual GEN_RESULT ValidateSurface(HSURFACE hSurface);
	virtual GEN_RESULT DrawSurface(HSURFACE hSurface, int x, int y, RectInt2D *pRect = NULL);
	virtual GEN_RESULT DrawSurfaceAlpha(HSURFACE hSurface, int x, int y, float fAlpha, RectInt2D *pRect = NULL);
	virtual GEN_RESULT ReleaseSurface(HSURFACE hSurface);

	virtual GEN_RESULT DrawText(const char* szText, int x, int y);
	virtual GEN_RESULT DrawText(HFONT hFont, const char* szText, int x, int y, uint32 nARGBColor);
	virtual GEN_RESULT DrawLine(HSURFACE hSurface, Vector2d& vStartPos, Vector2d& vEndPos, uint32 uColor);
	virtual GEN_RESULT SetSurfacePixel(HSURFACE hSurface, int x, int y, uint32 uColor);
	virtual HSURFACE GetBackBuffer(){ return g_pDDSBack; }

	virtual HFONT CreateFont(const char* szName, EFontStyle eStyle, int nHeight);
	virtual GEN_RESULT ReleaseFont(HFONT hFont);
	virtual GEN_RESULT GetStringSize(HFONT hFont, const char* szText, VectorInt2d &vSize);

protected:

	LPDIRECTDRAW7			g_pDirectDraw;
	LPDIRECTDRAWSURFACE7	g_pDDSPrimary;
	LPDIRECTDRAWSURFACE7	g_pDDSBack;
	LPDIRECTDRAWCLIPPER		g_pClipper;    // Clipper for windowed mode
	HWND					g_hWnd;        // Handle of window

	LPDIRECT3D8 m_pD3D;
	LPDIRECT3DDEVICE8 m_pDevice;

	// A surface description structure, used to describe the
	// primary and back surfaces.
	DDSURFACEDESC2 g_ddsd;

	// A surface capabilities structure, used to tell
	// CreateSurface() what kind of surface to create.
	DDSCAPS2 g_ddscaps;

	int	g_iBpp;
	uint32 g_nWidth;
	uint32 g_nHeight;
	bool g_bFullscreen;

	HFONT g_hFont;

	typedef std::list<HFONT> Fonts;
	Fonts g_Fonts;

	RectInt2D m_Viewport;
};

#endif
