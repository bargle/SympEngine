#ifndef _VIDEO_INTERFACE_SSE_H_
#define _VIDEO_INTERFACE_SSE_H_

#include "VideoInterface.h"

class IVideoInterface_SSE : public IVideoInterface
{
public:
	IVideoInterface_SSE();
	virtual ~IVideoInterface_SSE();
	virtual GEN_RESULT DrawSurfaceAlpha(HSURFACE hSurface, int x, int y, float fAlpha, RectInt2D *pRect = NULL);

protected:
};

#endif
