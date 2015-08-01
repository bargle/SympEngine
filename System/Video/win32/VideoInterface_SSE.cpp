#include "core_defs.h"
#include "VideoInterface_SSE.h"
#include "performance.h"
#include <mmintrin.h>

IVideoInterface_SSE::IVideoInterface_SSE():
IVideoInterface()
{
	
}

IVideoInterface_SSE::~IVideoInterface_SSE()
{
	IVideoInterface::Term();
}

namespace
{
	uint32 nRedDest;
	uint32 nGreenDest;
	uint32 nBlueDest;
	uint32 nRedSrc;
	uint32 nGreenSrc;
	uint32 nBlueSrc;
	uint32 nRedTmp;
	uint32 nGreenTmp;
	uint32 nBlueTmp;
}

GEN_RESULT IVideoInterface_SSE::DrawSurfaceAlpha(HSURFACE hSurface, int x, int y, float fAlpha, RectInt2D *pRect)
{
#pragma message("FIXME: IVideoInterface_MMX::DrawSurfaceAlpha ignoring pRect...")

	if(fAlpha < 0.01f)
	{
		return RESULT_OK;
	}

	if(NULL == hSurface)
	{
		return RESULT_ERROR;
	}

	ValidateSurface(hSurface);

	LPDIRECTDRAWSURFACE7 pSurface = ((LPDIRECTDRAWSURFACE7)hSurface);

	//TEST
	DDCOLORKEY ddck;
	pSurface->GetColorKey( DDCKEY_SRCBLT, &ddck );

	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(LPDDSURFACEDESC2));
	desc.dwSize = sizeof(DDSURFACEDESC2);
	//HRESULT hr = pSurface->GetSurfaceDesc(&desc);
	HRESULT hr = pSurface->Lock(NULL, &desc, DDLOCK_WAIT, NULL);
	if(hr != S_OK)
	{
		pSurface->Unlock(NULL);
		return RESULT_ERROR;
	}

	int nWidth = desc.dwWidth;
	int nHeight = desc.dwHeight;

	DDSURFACEDESC2 descBack;
	ZeroMemory(&descBack, sizeof(LPDDSURFACEDESC2));
	descBack.dwSize = sizeof(DDSURFACEDESC2);
	HRESULT hr1 = g_pDDSBack->Lock(NULL, &descBack, DDLOCK_WAIT, NULL);
	if(hr1 != S_OK)
	{
		g_pDDSBack->Unlock(NULL);
		return RESULT_ERROR;
	}

	//do 32-bit alpha...
	int nSrcWidth = desc.lPitch / sizeof(uint32);
	int nDestWidth = descBack.lPitch / sizeof(uint32);

	int nSrcSkip = (nSrcWidth - desc.dwWidth);
	int nDestSkip = /*x +*/ (nDestWidth - descBack.dwWidth);

	if((DWORD)y > descBack.dwHeight)
	{
		g_pDDSBack->Unlock(NULL);
		pSurface->Unlock(NULL);
		return RESULT_ERROR;
	}

	uint8 nAlpha = (int)(fAlpha * 255.0f);

	//CPerformanceCounter counter;
	//counter.Start();

#define REDMASK (0x00FF0000)
#define GREENMASK (0x0000FF00)
#define BLUEMASK (0x000000FF)
#define GET_MASK(a, b) (a & b)

	//get the dest pixel starting point
	uint32 *pDest = (uint32*)descBack.lpSurface;
	pDest += (y * nDestWidth) + x;

	//get the source pixel starting point
	uint32 *pSrc = (uint32*)desc.lpSurface;

	bool bOdd = false;
	int nSkipped = 0;

	for(int vertical = 0; vertical < nHeight; ++vertical)
	{
		for(int horiz = 0; horiz < (nWidth/2); ++horiz)
		{
#if 0
			if(*pSrc == (0xFF000000 | ddck.dwColorSpaceLowValue))
			{
				++pDest;
				++pSrc;
				continue;
			}			
			// C++

			nRedDest	= GET_MASK(*pDest, REDMASK);
			nGreenDest	= GET_MASK(*pDest, GREENMASK);
			nBlueDest	= GET_MASK(*pDest, BLUEMASK);

			nRedSrc		= GET_MASK(*pSrc, REDMASK);
			nGreenSrc	= GET_MASK(*pSrc, GREENMASK);
			nBlueSrc	= GET_MASK(*pSrc, BLUEMASK);			


			//nRedTmp = (nRedSrc* fAlpha) + (nRedDest*(1.0f-fAlpha));
			//nGreenTmp = (nGreenSrc * fAlpha) + (nGreenDest *(1.0f-fAlpha));
			//nBlueTmp = (nBlueSrc * fAlpha) + (nBlueDest *(1.0f-fAlpha));

			nRedTmp = nRedDest + ( (nAlpha * (nRedSrc - nRedDest)) >> 8);
			nGreenTmp = nGreenDest + ( (nAlpha * (nGreenSrc - nGreenDest)) >> 8);
			nBlueTmp = nBlueDest + ( (nAlpha * (nBlueSrc - nBlueDest)) >> 8);

			//save to the dest surface
			*pDest = (0xFF000000) | GET_MASK(nRedTmp, REDMASK) | GET_MASK(nGreenTmp, GREENMASK) | GET_MASK(nBlueTmp, BLUEMASK);
#else		
			if(*pSrc == (0xFF000000 | ddck.dwColorSpaceLowValue))
			{
				++pDest;
				++pSrc;
			}
			else
			{
				//save to the dest surface
				*pDest = (0xFF000000) | 
					GET_MASK((GET_MASK(*pDest, REDMASK) + ( (nAlpha * (GET_MASK(*pSrc, REDMASK) - GET_MASK(*pDest, REDMASK))) >> 8)), REDMASK) | 
					GET_MASK((GET_MASK(*pDest, GREENMASK) + ( (nAlpha * (GET_MASK(*pSrc, GREENMASK) - GET_MASK(*pDest, GREENMASK))) >> 8)), GREENMASK) | 
					GET_MASK((GET_MASK(*pDest, BLUEMASK) + ( (nAlpha * (GET_MASK(*pSrc, BLUEMASK) -  GET_MASK(*pDest, BLUEMASK))) >> 8)), BLUEMASK);

				++pDest;
				++pSrc;
			}

			if(*pSrc == (0xFF000000 | ddck.dwColorSpaceLowValue))
			{
				++pDest;
				++pSrc;
			}
			else
			{
				//save to the dest surface
				*pDest = (0xFF000000) | 
					GET_MASK((GET_MASK(*pDest, REDMASK) + ( (nAlpha * (GET_MASK(*pSrc, REDMASK) - GET_MASK(*pDest, REDMASK))) >> 8)), REDMASK) | 
					GET_MASK((GET_MASK(*pDest, GREENMASK) + ( (nAlpha * (GET_MASK(*pSrc, GREENMASK) - GET_MASK(*pDest, GREENMASK))) >> 8)), GREENMASK) | 
					GET_MASK((GET_MASK(*pDest, BLUEMASK) + ( (nAlpha * (GET_MASK(*pSrc, BLUEMASK) -  GET_MASK(*pDest, BLUEMASK))) >> 8)), BLUEMASK);

				++pDest;
				++pSrc;
			}

#endif

			

			//++pDest;
			//++pSrc;
		}

		pDest += nDestSkip;

		//if the source is smaller than the destination surface.
		if(nSrcWidth < nDestWidth)
		{	
			pDest += ((descBack.dwWidth - x) - desc.dwWidth) + x;
		}

		pSrc += nSrcSkip;
	}


	//counter.Stop();
	//float fDelta = counter.TimeInMilliseconds();
	//DEBUG_LOG(if(fp) {fprintf(fp, "[%06d] - %f\n", ++g_nLogCounter, fDelta);})

		g_pDDSBack->Unlock(NULL);
	pSurface->Unlock(NULL);

	return RESULT_OK;
}
