#ifndef _VIDEO_INTERFACE_MMX_H_
#define _VIDEO_INTERFACE_MMX_H_

#include "VideoInterface.h"

class IVideoInterface_MMX : public IVideoInterface
{
public:
	IVideoInterface_MMX();
	virtual ~IVideoInterface_MMX();

	//virtual GEN_RESULT Init(void* hWnd, uint32 nWidth, uint32 nHeight, uint32 nDepth, bool bFullscreen);
	//virtual GEN_RESULT Term();

	//virtual GEN_RESULT BeginScene();
	//virtual GEN_RESULT EndScene();

	// Surface functions
	//virtual GEN_RESULT CreateSurface(HSURFACE &hSurface, int x, int y);
	//virtual GEN_RESULT CreateSurfaceFromRaw(HSURFACE &hSurface, int x, int y, const char* szFilename);
	//virtual GEN_RESULT ValidateSurface(HSURFACE hSurface);
	//virtual GEN_RESULT DrawSurface(HSURFACE hSurface, int x, int y, Rect2D *pRect = NULL);
	virtual GEN_RESULT DrawSurfaceAlpha(HSURFACE hSurface, int x, int y, float fAlpha, Rect2D *pRect = NULL);
	//virtual GEN_RESULT ReleaseSurface(HSURFACE hSurface);

	//virtual GEN_RESULT DrawText(const char* szText, int x, int y);

protected:
};

#endif
