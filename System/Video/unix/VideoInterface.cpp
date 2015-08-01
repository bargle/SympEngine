#include "core_defs.h"
#include "VideoInterface.h"
#include "resourcemanager.h"

//#include <ddutil.h>
#include <algorithm>

#define DEBUG_VIDEOSTAT_LOG 0
#if DEBUG_VIDEOSTAT_LOG
namespace
{
	int g_nLogCounter = 0;
}
	#define DEBUG_LOG(x) x
#else
	#define DEBUG_LOG(x)
#endif


IVideoInterface::IVideoInterface():
m_Viewport(0,0,0,0)
{
	g_iBpp = 0;
	g_nWidth = 0;
	g_nHeight = 0;
	g_bFullscreen = false;
}

IVideoInterface::~IVideoInterface()
{
	DEBUG_LOG(fclose(fp);)
	DEBUG_LOG(fp = NULL;)

	Term();

	if(g_hFont)
	{
		//DeleteObject(g_hFont);
		g_hFont = NULL;
	}
}



GEN_RESULT IVideoInterface::Init(void* hWnd, uint32 nWidth, uint32 nHeight, uint32 nDepth, bool bFullscreen)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::Term()
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::BeginScene()
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::EndScene()
{
	return RESULT_OK;	
}

	// Surface functions
GEN_RESULT IVideoInterface::CreateSurface(HSURFACE &hSurface, int x, int y)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::CreateSurfaceFromRaw(HSURFACE &hSurface, int x, int y, const char* szFilename)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::CreateSurfaceFromFile(HSURFACE &hSurface, int *x, int *y, const char* szFilename)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::GetSurfaceDims(HSURFACE hSurface, int *x, int *y)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::ValidateSurface(HSURFACE hSurface)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::DrawSurface(HSURFACE hSurface, int x, int y, RectInt2D *pRect)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::DrawSurfaceAlpha(HSURFACE hSurface, int x, int y, float fAlpha, RectInt2D *pRect)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::ReleaseSurface(HSURFACE hSurface)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::DrawText(const char* szText, int x, int y)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::DrawText(HFONT_ hFont, const char* szText, int x, int y, uint32 nARGBColor)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::SetSurfacePixel(HSURFACE hSurface, int x, int y, uint32 uColor)
{
	return RESULT_OK;
}

HFONT_ IVideoInterface::CreateFont(const char* szName, EFontStyle eStyle, int nHeight)
{
	return NULL;
}

GEN_RESULT IVideoInterface::ReleaseFont(HFONT_ hFont)
{
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::GetStringSize(HFONT_ hFont, const char* szText, VectorInt2d &vSize)
{
	return RESULT_OK;
}
