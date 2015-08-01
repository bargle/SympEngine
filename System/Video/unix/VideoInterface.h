#ifndef _VIDEO_INTERFACE_H_
#define _VIDEO_INTERFACE_H_

#include <list>
typedef void* HSURFACE;
typedef void* HFONT_;

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
	virtual GEN_RESULT DrawText(HFONT_ hFont, const char* szText, int x, int y, uint32 nARGBColor);
	virtual GEN_RESULT SetSurfacePixel(HSURFACE hSurface, int x, int y, uint32 uColor);
	virtual HSURFACE GetBackBuffer(){ return NULL; }

	virtual HFONT_ CreateFont(const char* szName, EFontStyle eStyle, int nHeight);
	virtual GEN_RESULT ReleaseFont(HFONT_ hFont);
	virtual GEN_RESULT GetStringSize(HFONT_ hFont, const char* szText, VectorInt2d &vSize);

protected:

	int	g_iBpp;
	uint32 g_nWidth;
	uint32 g_nHeight;
	bool g_bFullscreen;

	HFONT_ g_hFont;

	typedef std::list<HFONT_> Fonts;
	Fonts g_Fonts;

	RectInt2D m_Viewport;
};

#endif
