#include "core_defs.h"
#include "VideoInterface_MMX.h"
#include "performance.h"
#include <mmintrin.h>

IVideoInterface_MMX::IVideoInterface_MMX():
IVideoInterface()
{
	
}

IVideoInterface_MMX::~IVideoInterface_MMX()
{
	IVideoInterface::Term();
}


GEN_RESULT IVideoInterface_MMX::DrawSurfaceAlpha(HSURFACE hSurface, int x, int y, float fAlpha, Rect2D *pRect)
{
#pragma message("FIXME: IVideoInterface_MMX::DrawSurfaceAlpha ignoring pRect...")

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

	CPerformanceCounter counter;
	counter.Start();

#define REDMASK (0x00FF0000)
#define GREENMASK (0x0000FF00)
#define BLUEMASK (0x000000FF)
#define GET_MASK(a, b) (a & b)
#define GET_MASK_ASM(a, b, c) \
	__asm mov         ecx, dword ptr [ a ] \
	__asm and         ecx,b  \
	__asm mov         dword ptr [ c ],ecx


	//get the dest pixel starting point
	uint32 *pDest = (uint32*)descBack.lpSurface;
	pDest += (y * nDestWidth) + x;

	//get the source pixel starting point
	uint32 *pSrc = (uint32*)desc.lpSurface;

	// MMX
	__int64 i64Alpha;
	i64Alpha = nAlpha;
	i64Alpha |= ( i64Alpha << 16 ) | ( i64Alpha << 32 );


		//uint32 *lpbTarget = (uint32*)descBack.lpSurface;
		//pDest += (y * nDestWidth) + x;

		//get the source pixel starting point
		//uint32 *lpbSource = (uint32*)desc.lpSurface;

		nWidth /= 2;
		//__asm push edi 
		//__asm push esi

		//__asm mov edi, pDest		
		//__asm mov esi, pSrc

		// Load	the alpha value into an mmx register.
		__asm movq mm5, i64Alpha
			//__asm push eax


	bool bOdd = false;
	int nSkipped = 0;

	for(int vertical = 0; vertical < nHeight; ++vertical)
	{
		for(int horiz = 0; horiz < nWidth; ++horiz)
		{
			int a = horiz;

			// Setup our source a dest pixels...
			//__asm mov edi, pDest		
			//__asm mov esi, pSrc


			// Are we skipping the first pixel?
			bool bSkipOne = false;
			if(*pSrc == (0xFF000000 | ddck.dwColorSpaceLowValue) )
			{
				bSkipOne = true;
			}

			// Are we skipping the second pixel?
			bool bSkipTwo = false;
			if(*(pSrc+1) == (0xFF000000 | ddck.dwColorSpaceLowValue) )
			{
				bSkipTwo = true;
			}

			// Are we skiping both?
			if(bSkipOne && bSkipTwo)
			{
				pDest +=2;
				pSrc  +=2;

				continue;
			}

			/*
			mm0:	target pixel one
			mm1:	source pixel one
			mm2:	target pixel two
			mm3:	source pixel two
			mm4:	working register
			mm5:	alpha value
			mm6:	original target
			mm7:	original source
			*/
			//__asm pxor		mm0, mm0				// Clear mm4 so we can unpack easily.
			//__asm pxor		mm1, mm1				// Clear mm4 so we can unpack easily.
			//__asm pxor		mm2, mm2				// Clear mm4 so we can unpack easily.
			//__asm pxor		mm3, mm3				// Clear mm4 so we can unpack easily.
			//__asm pxor		mm4, mm4				// Clear mm4 so we can unpack easily.
			//__asm pxor		mm5, mm5				// Clear mm4 so we can unpack easily.
			//__asm pxor		mm6, mm6				// Clear mm4 so we can unpack easily.
			//__asm pxor		mm7, mm7				// Clear mm4 so we can unpack easily.


			__asm mov edi, pDest		
			__asm mov esi, pSrc

			// Move the source and target pixels into the regsiters
			__asm movq		mm6, [edi]				// Load the target pixels.
			__asm pxor		mm4, mm4				// Clear mm4 so we can unpack easily.

			//Source 1 & 2
			__asm movq		mm7, [esi]				// Load the source pixels.

			//Dest 1
			__asm movq		mm0, mm6				// Create copy one of the target.

			//Dest 2
			__asm movq		mm2, mm6				// Create copy two of the target.


			// Unpack the target
			__asm punpcklbw	mm0, mm4				// Unpack the first target copy.
			

			//Source 1
			__asm movq		mm1, mm7				// Create copy one of the source.
			__asm psrlq		mm2, 32					// Move the high order dword of target two. 

			__asm punpcklbw	mm2, mm4				// Unpack the second target copy. 
			__asm movq		mm3, mm7				// Create copy two of the source.

			__asm psrlq		mm3, 32					// Move the high order dword of source two.
			__asm punpcklbw	mm1, mm4				// Unpack the first source copy.

			__asm punpcklbw	mm3, mm4				// Unpack the second source copy.
			__asm pslld		mm7, 8					// Shift away original source highest bytes.

			__asm movq		mm4, mm0				// Save target one.
			__asm pmullw	mm0, mm5				// Multiply target one with alpha.

			__asm pmullw	mm1, mm5				// Multiply source one with alpha.
			__asm psrld		mm7, 8					// Complete high order byte clearance.	

			__asm psrlw		mm1, 8					// Divide source one by 256.
			__asm nop

			__asm psrlw		mm0, 8					// Divide target one by 256.
			__asm nop

			__asm psubw		mm1, mm0				// Calculate source one minus target one.
			__asm nop

			__asm paddw		mm1, mm4				// Add the former target one to the result.
			__asm nop

			__asm movq		mm4, mm2				// Save target two.
			__asm pmullw	mm2, mm5				// Multiply target two with alpha.

			__asm pmullw	mm3, mm5				// Multiply source two with alpha.
			__asm nop

			__asm psrlw		mm2, 8					// Divide target two by 256.
			__asm nop

			__asm psrlw		mm3, 8					// Divide source two by 256.
			__asm nop

			__asm psubw		mm3, mm2				// Calculate source two minus source one.
			__asm nop

			__asm paddw		mm3, mm4				// Add the former target two to the result.

			__asm pxor		mm4, mm4				// Clear mm4 so we can pack easily.

			__asm packuswb	mm1, mm4				// Pack the new target one.
			__asm packuswb	mm3, mm4				// Pack the new target two.

			__asm psllq		mm3, 32					// Shift up the new target two.
			__asm pcmpeqd	mm4, mm7				// Create a color key mask.

			__asm por		mm1, mm3				// Combine the new targets.
			__asm pand		mm6, mm4				// Keep old target where color key applies.

			__asm pandn		mm4, mm1				// Clear new target where color key applies.
			__asm nop

			__asm por		mm6, mm4				// Assemble the new target value.
			__asm nop

			__asm movq		[edi], mm6				// Write back the new target value.

			pDest += 2;
			pSrc += 2;
		}

		pDest += nDestSkip;

		//if the source is smaller than the destination surface.
		if(nSrcWidth < nDestWidth)
		{	
			pDest += ((descBack.dwWidth - x) - desc.dwWidth) + x;
		}

		pSrc += nSrcSkip;
	}


	_mm_empty();


	counter.Stop();
	float fDelta = counter.TimeInMilliseconds();
	//DEBUG_LOG(if(fp) {fprintf(fp, "[%06d] - %f\n", ++g_nLogCounter, fDelta);})

		g_pDDSBack->Unlock(NULL);
	pSurface->Unlock(NULL);

	return RESULT_OK;
}
