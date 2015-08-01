#include "core_defs.h"
#include "VideoInterface.h"
#include "resourcemanager.h"

//For now, we're just going to use directx (ddraw)
#include <ddraw.h>
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

namespace
{
	bool g_bUseMMX = false;

	// Create color from RGB triple
	unsigned int CreateRGB( int r, int g, int b , int bpp)
	{
		switch (bpp)
		{
		case 8:
			// Here you should do a palette lookup to find the closes match.
			// I'm not going to bother with that. Many modern games no
			// longer support 256-color modes, and neither should you :)
			return 0;
		case 16:
			// Break down r,g,b into 5-6-5 format.
			return ((r/8)<<11) | ((g/4)<<5) | (b/8);
		case 24:
		case 32:
			return (r<<16) | (g<<8) | (b);
		}
		return 0;
	}

	char *DDErrorString(HRESULT hr)
	{
		switch (hr)
		{
		case DDERR_ALREADYINITIALIZED:           return "DDERR_ALREADYINITIALIZED";
		case DDERR_CANNOTATTACHSURFACE:          return "DDERR_CANNOTATTACHSURFACE";
		case DDERR_CANNOTDETACHSURFACE:          return "DDERR_CANNOTDETACHSURFACE";
		case DDERR_CURRENTLYNOTAVAIL:            return "DDERR_CURRENTLYNOTAVAIL";
		case DDERR_EXCEPTION:                    return "DDERR_EXCEPTION";
		case DDERR_GENERIC:                      return "DDERR_GENERIC";
		case DDERR_HEIGHTALIGN:                  return "DDERR_HEIGHTALIGN";
		case DDERR_INCOMPATIBLEPRIMARY:          return "DDERR_INCOMPATIBLEPRIMARY";
		case DDERR_INVALIDCAPS:                  return "DDERR_INVALIDCAPS";
		case DDERR_INVALIDCLIPLIST:              return "DDERR_INVALIDCLIPLIST";
		case DDERR_INVALIDMODE:                  return "DDERR_INVALIDMODE";
		case DDERR_INVALIDOBJECT:                return "DDERR_INVALIDOBJECT";
		case DDERR_INVALIDPARAMS:                return "DDERR_INVALIDPARAMS";
		case DDERR_INVALIDPIXELFORMAT:           return "DDERR_INVALIDPIXELFORMAT";
		case DDERR_INVALIDRECT:                  return "DDERR_INVALIDRECT";
		case DDERR_LOCKEDSURFACES:               return "DDERR_LOCKEDSURFACES";
		case DDERR_NO3D:                         return "DDERR_NO3D";
		case DDERR_NOALPHAHW:                    return "DDERR_NOALPHAHW";
		case DDERR_NOCLIPLIST:                   return "DDERR_NOCLIPLIST";
		case DDERR_NOCOLORCONVHW:                return "DDERR_NOCOLORCONVHW";
		case DDERR_NOCOOPERATIVELEVELSET:        return "DDERR_NOCOOPERATIVELEVELSET";
		case DDERR_NOCOLORKEY:                   return "DDERR_NOCOLORKEY";
		case DDERR_NOCOLORKEYHW:                 return "DDERR_NOCOLORKEYHW";
		case DDERR_NODIRECTDRAWSUPPORT:          return "DDERR_NODIRECTDRAWSUPPORT";
		case DDERR_NOEXCLUSIVEMODE:              return "DDERR_NOEXCLUSIVEMODE";
		case DDERR_NOFLIPHW:                     return "DDERR_NOFLIPHW";
		case DDERR_NOGDI:                        return "DDERR_NOGDI";
		case DDERR_NOMIRRORHW:                   return "DDERR_NOMIRRORHW";
		case DDERR_NOTFOUND:                     return "DDERR_NOTFOUND";
		case DDERR_NOOVERLAYHW:                  return "DDERR_NOOVERLAYHW";
		case DDERR_NORASTEROPHW:                 return "DDERR_NORASTEROPHW";
		case DDERR_NOROTATIONHW:                 return "DDERR_NOROTATIONHW";
		case DDERR_NOSTRETCHHW:                  return "DDERR_NOSTRETCHHW";
		case DDERR_NOT4BITCOLOR:                 return "DDERR_NOT4BITCOLOR";
		case DDERR_NOT4BITCOLORINDEX:            return "DDERR_NOT4BITCOLORINDEX";
		case DDERR_NOT8BITCOLOR:                 return "DDERR_NOT8BITCOLOR";
		case DDERR_NOTEXTUREHW:                  return "DDERR_NOTEXTUREHW";
		case DDERR_NOVSYNCHW:                    return "DDERR_NOVSYNCHW";
		case DDERR_NOZBUFFERHW:                  return "DDERR_NOZBUFFERHW";
		case DDERR_NOZOVERLAYHW:                 return "DDERR_NOZOVERLAYHW";
		case DDERR_OUTOFCAPS:                    return "DDERR_OUTOFCAPS";
		case DDERR_OUTOFMEMORY:                  return "DDERR_OUTOFMEMORY";
		case DDERR_OUTOFVIDEOMEMORY:             return "DDERR_OUTOFVIDEOMEMORY";
		case DDERR_OVERLAYCANTCLIP:              return "DDERR_OVERLAYCANTCLIP";
		case DDERR_OVERLAYCOLORKEYONLYONEACTIVE: return "DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
		case DDERR_PALETTEBUSY:                  return "DDERR_PALETTEBUSY";
		case DDERR_COLORKEYNOTSET:               return "DDERR_COLORKEYNOTSET";
		case DDERR_SURFACEALREADYATTACHED:       return "DDERR_SURFACEALREADYATTACHED";
		case DDERR_SURFACEALREADYDEPENDENT:      return "DDERR_SURFACEALREADYDEPENDENT";
		case DDERR_SURFACEBUSY:                  return "DDERR_SURFACEBUSY";
		case DDERR_CANTLOCKSURFACE:              return "DDERR_CANTLOCKSURFACE";
		case DDERR_SURFACEISOBSCURED:            return "DDERR_SURFACEISOBSCURED";
		case DDERR_SURFACELOST:                  return "DDERR_SURFACELOST";
		case DDERR_SURFACENOTATTACHED:           return "DDERR_SURFACENOTATTACHED";
		case DDERR_TOOBIGHEIGHT:                 return "DDERR_TOOBIGHEIGHT";
		case DDERR_TOOBIGSIZE:                   return "DDERR_TOOBIGSIZE";
		case DDERR_TOOBIGWIDTH:                  return "DDERR_TOOBIGWIDTH";
		case DDERR_UNSUPPORTED:                  return "DDERR_UNSUPPORTED";
		case DDERR_UNSUPPORTEDFORMAT:            return "DDERR_UNSUPPORTEDFORMAT";
		case DDERR_UNSUPPORTEDMASK:              return "DDERR_UNSUPPORTEDMASK";
		case DDERR_VERTICALBLANKINPROGRESS:      return "DDERR_VERTICALBLANKINPROGRESS";
		case DDERR_WASSTILLDRAWING:              return "DDERR_WASSTILLDRAWING";
		case DDERR_XALIGN:                       return "DDERR_XALIGN";
		case DDERR_INVALIDDIRECTDRAWGUID:        return "DDERR_INVALIDDIRECTDRAWGUID";
		case DDERR_DIRECTDRAWALREADYCREATED:     return "DDERR_DIRECTDRAWALREADYCREATED";
		case DDERR_NODIRECTDRAWHW:               return "DDERR_NODIRECTDRAWHW";
		case DDERR_PRIMARYSURFACEALREADYEXISTS:  return "DDERR_PRIMARYSURFACEALREADYEXISTS";
		case DDERR_NOEMULATION:                  return "DDERR_NOEMULATION";
		case DDERR_REGIONTOOSMALL:               return "DDERR_REGIONTOOSMALL";
		case DDERR_CLIPPERISUSINGHWND:           return "DDERR_CLIPPERISUSINGHWND";
		case DDERR_NOCLIPPERATTACHED:            return "DDERR_NOCLIPPERATTACHED";
		case DDERR_NOHWND:                       return "DDERR_NOHWND";
		case DDERR_HWNDSUBCLASSED:               return "DDERR_HWNDSUBCLASSED";
		case DDERR_HWNDALREADYSET:               return "DDERR_HWNDALREADYSET";
		case DDERR_NOPALETTEATTACHED:            return "DDERR_NOPALETTEATTACHED";
		case DDERR_NOPALETTEHW:                  return "DDERR_NOPALETTEHW";
		case DDERR_BLTFASTCANTCLIP:              return "DDERR_BLTFASTCANTCLIP";
		case DDERR_NOBLTHW:                      return "DDERR_NOBLTHW";
		case DDERR_NODDROPSHW:                   return "DDERR_NODDROPSHW";
		case DDERR_OVERLAYNOTVISIBLE:            return "DDERR_OVERLAYNOTVISIBLE";
		case DDERR_NOOVERLAYDEST:                return "DDERR_NOOVERLAYDEST";
		case DDERR_INVALIDPOSITION:              return "DDERR_INVALIDPOSITION";
		case DDERR_NOTAOVERLAYSURFACE:           return "DDERR_NOTAOVERLAYSURFACE";
		case DDERR_EXCLUSIVEMODEALREADYSET:      return "DDERR_EXCLUSIVEMODEALREADYSET";
		case DDERR_NOTFLIPPABLE:                 return "DDERR_NOTFLIPPABLE";
		case DDERR_CANTDUPLICATE:                return "DDERR_CANTDUPLICATE";
		case DDERR_NOTLOCKED:                    return "DDERR_NOTLOCKED";
		case DDERR_CANTCREATEDC:                 return "DDERR_CANTCREATEDC";
		case DDERR_NODC:                         return "DDERR_NODC";
		case DDERR_WRONGMODE:                    return "DDERR_WRONGMODE";
		case DDERR_IMPLICITLYCREATED:            return "DDERR_IMPLICITLYCREATED";
		case DDERR_NOTPALETTIZED:                return "DDERR_NOTPALETTIZED";
		case DDERR_UNSUPPORTEDMODE:              return "DDERR_UNSUPPORTEDMODE";
		case DDERR_NOMIPMAPHW:                   return "DDERR_NOMIPMAPHW";
		case DDERR_INVALIDSURFACETYPE:           return "DDERR_INVALIDSURFACETYPE";
		case DDERR_DCALREADYCREATED:             return "DDERR_DCALREADYCREATED";
		case DDERR_CANTPAGELOCK:                 return "DDERR_CANTPAGELOCK";
		case DDERR_CANTPAGEUNLOCK:               return "DDERR_CANTPAGEUNLOCK";
		case DDERR_NOTPAGELOCKED:                return "DDERR_NOTPAGELOCKED";
		case DDERR_NOTINITIALIZED:               return "DDERR_NOTINITIALIZED";
		}
		return "Unknown Error";
	}

	DWORD ConvertGDIColor(LPDIRECTDRAWSURFACE7 pSurface, COLORREF dwGDIColor )
	{
		if( pSurface == NULL )
			return 0x00000000;

		COLORREF       rgbT;
		HDC            hdc;
		DWORD          dw = CLR_INVALID;
		DDSURFACEDESC2 ddsd;
		HRESULT        hr;

		//  Use GDI SetPixel to color match for us
		if( dwGDIColor != CLR_INVALID && pSurface->GetDC(&hdc) == DD_OK)
		{
			rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
			SetPixel(hdc, 0, 0, dwGDIColor);       // Set our value
			pSurface->ReleaseDC(hdc);
		}

		// Now lock the surface so we can read back the converted color
		ddsd.dwSize = sizeof(ddsd);
		hr = pSurface->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );
		if( hr == DD_OK)
		{
			dw = *(DWORD *) ddsd.lpSurface; 
			if( ddsd.ddpfPixelFormat.dwRGBBitCount < 32 ) // Mask it to bpp
				dw &= ( 1 << ddsd.ddpfPixelFormat.dwRGBBitCount ) - 1;  
			pSurface->Unlock(NULL);
		}

		//  Now put the color that was there back.
		if( dwGDIColor != CLR_INVALID && pSurface->GetDC(&hdc) == DD_OK )
		{
			SetPixel( hdc, 0, 0, rgbT );
			pSurface->ReleaseDC(hdc);
		}

		return dw;    
	}

	HRESULT SetColorKey(LPDIRECTDRAWSURFACE7 pSurface, DWORD dwColorKey )
	{
		if( NULL == pSurface )
			return E_POINTER;

		//m_bColorKeyed = TRUE;

		DDCOLORKEY ddck;
		ddck.dwColorSpaceLowValue  = ConvertGDIColor(pSurface, dwColorKey );
		ddck.dwColorSpaceHighValue = ConvertGDIColor(pSurface, dwColorKey );

		return pSurface->SetColorKey( DDCKEY_SRCBLT, &ddck );
	}


	//TEMP
	DEBUG_LOG(FILE* fp = NULL; )
}

IVideoInterface::IVideoInterface():
m_Viewport(0,0,0,0)
{
	
	// start our font
	LOGFONT logFont;
	ZeroMemory(&logFont, sizeof(LOGFONT));
	strcpy(logFont.lfFaceName, "Arial");
	logFont.lfHeight = 14;
	logFont.lfQuality = ANTIALIASED_QUALITY;
	g_hFont = CreateFontIndirect(&logFont);
	
	g_pDirectDraw	= NULL;
	g_pDDSPrimary	= NULL;
	g_pDDSBack		= NULL;
	g_pClipper = NULL;    // Clipper for windowed mode
	g_hWnd = NULL;        // Handle of window

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
		DeleteObject(g_hFont);
		g_hFont = NULL;
	}
}

GEN_RESULT IVideoInterface::Init(void* hWnd, uint32 nWidth, uint32 nHeight, uint32 nDepth, bool bFullScreen)
{
	if(NULL == hWnd)
	{
		return RESULT_ERROR;
	}

	DEBUG_LOG(fp = fopen("videostats.txt", "w");)

	g_hWnd = (HWND)hWnd;
	g_bFullscreen = bFullScreen;

	if ( FAILED( DirectDrawCreateEx( NULL, ( LPVOID* )&g_pDirectDraw, IID_IDirectDraw7, NULL ) ) )
	{
		return RESULT_ERROR;
	}

	if(bFullScreen)
	{
		if ( FAILED( g_pDirectDraw->SetCooperativeLevel( g_hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) ) )
		{
			return RESULT_ERROR;
		}

		// Set the display mode
		if ( FAILED( g_pDirectDraw->SetDisplayMode( nWidth, nHeight, nDepth, 0, 0 ) ) )
		{
			return RESULT_ERROR;
		}
	}
	else
	{
		if ( FAILED( g_pDirectDraw->SetCooperativeLevel( g_hWnd, DDSCL_NORMAL ) ) )
		{
			return RESULT_ERROR;
		}
	}

	//Create Surfaces
	ZeroMemory( &g_ddsd, sizeof( g_ddsd ) );
	g_ddsd.dwSize = sizeof( g_ddsd );

	if(bFullScreen)
	{
		// DDSD_CAPS makes CreateSurface() check the
		// ddsd.ddsCaps.dwCaps value. DDS_BACKBUFFERCOUNT makes it
		// check the ddsd.dwBackBufferCount value.
		g_ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

		// Make it the primary surface that can be flipped.
		g_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

		// Give it one back buffer.
		g_ddsd.dwBackBufferCount = 1;

		// Create the primary surface using the description in ddsd.
		if ( FAILED( g_pDirectDraw->CreateSurface( &g_ddsd, &g_pDDSPrimary, NULL ) ) )
		{
			return RESULT_ERROR;
		}

		// CREATE BACK BUFFER
		g_ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		if ( FAILED( g_pDDSPrimary->GetAttachedSurface( &g_ddscaps, &g_pDDSBack ) ) )
		{
			return RESULT_ERROR;
		}

		g_pDDSBack->AddRef();

	}
	else //WINDOWED
	{
		//-- Create the primary surface

		// The dwFlags parameter tell DirectDraw which DDSURFACEDESC
		// fields will contain valid values
		g_ddsd.dwFlags = DDSD_CAPS;
		g_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;// | DDSCAPS_SYSTEMMEMORY;

		if( FAILED(g_pDirectDraw->CreateSurface(&g_ddsd, &g_pDDSPrimary, NULL)) )
		{
			return RESULT_ERROR;
		}

		//-- Create the back buffer
		g_ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;

		// Setup our off-screen surface dims
		g_ddsd.dwWidth = nWidth;
		g_ddsd.dwHeight = nHeight;

		g_nWidth = nWidth;
		g_nHeight = nHeight;
		
		// Create an offscreen surface
		g_ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

		if( FAILED(g_pDirectDraw->CreateSurface(&g_ddsd, &g_pDDSBack, NULL)) ) 
		{
			return RESULT_ERROR;
		}
		
		// Create the clipper using the DirectDraw object
		if( FAILED(g_pDirectDraw->CreateClipper(0, &g_pClipper, NULL)))
		{
			return RESULT_ERROR;
		}		

		// Assign your window's HWND to the clipper
		if( FAILED(g_pClipper->SetHWnd(0, g_hWnd)) )
		{
			return RESULT_ERROR;
		}

		// Attach the clipper to the primary surface
		if( FAILED(g_pDDSPrimary->SetClipper(g_pClipper)) )
		{
			return RESULT_ERROR;
		}

		if (g_pDDSBack)
		{
			if( FAILED(g_pDDSBack->Lock( NULL, &g_ddsd, DDLOCK_WAIT, NULL )) )
			{
				return RESULT_ERROR;
			}

			// Store bit depth of surface
			g_iBpp = g_ddsd.ddpfPixelFormat.dwRGBBitCount;

			// Unlock surface
			if( FAILED(g_pDDSBack->Unlock( NULL )) )
			{
				return RESULT_ERROR;
			}
		}

	}

	//Init D3D8
	if((m_pD3D = Direct3DCreate8(D3D_SDK_VERSION))==NULL)
	{
		return RESULT_ERROR;
	}

	D3DDISPLAYMODE d3ddm;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.Windowed = true;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	if(FAILED(m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		(HWND)hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice)))
	{
			return RESULT_ERROR;
	}


	return RESULT_OK;
}

GEN_RESULT IVideoInterface::Term()
{
	if(g_pClipper)
	{
		g_pClipper->Release();
		g_pClipper = NULL;
	}

	if(g_pDDSBack)
	{
		g_pDDSBack->Release();
		g_pDDSBack = NULL;
	}

	if(g_pDDSPrimary)
	{
		g_pDDSPrimary->Release();
		g_pDDSPrimary = NULL;
	}

	if(g_pDirectDraw)
	{
		g_pDirectDraw->Release();
		g_pDirectDraw = NULL;
	}

	Fonts::iterator iter = g_Fonts.begin();
	while(iter != g_Fonts.end())
	{
		if(*iter)
		{
			DeleteObject(*iter);
		}

		++iter;
	}
	g_Fonts.clear();

	//Term D3D8
	//m_pDevice->Release();
	m_pD3D->Release();

	return RESULT_OK;
}

GEN_RESULT IVideoInterface::BeginScene()
{
	//
	// Restore the surfaces if need be.
	//
	// Check the primary surface
	if (g_pDDSPrimary)
	{
		if (g_pDDSPrimary->IsLost() == DDERR_SURFACELOST)
			g_pDDSPrimary->Restore();
	}
	// Check the back buffer
	if (g_pDDSBack)
	{
		if (g_pDDSBack->IsLost() == DDERR_SURFACELOST)
			g_pDDSBack->Restore();
	}

	if(!g_bFullscreen)
	{
		//clear the surface
		DDBLTFX ddbfx;
		RECT    rcDest;

		// Safety net
		if (NULL == g_pDDSPrimary)
		{
			return RESULT_ERROR;
		}

		// Initialize the DDBLTFX structure with the pixel color
		ddbfx.dwSize = sizeof( ddbfx );
		ddbfx.dwFillColor = CreateRGB( 0x01, 0x01, 0x01 , g_iBpp); //(DWORD)0;

		SetRect( &rcDest, 0, 0, g_nWidth, g_nHeight );

		// Blit 1x1 rectangle using solid color op
		if( FAILED(g_pDDSBack->Blt( &rcDest, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ddbfx )) )
		{
			return RESULT_ERROR;
		}
	}

	return RESULT_OK;
}

GEN_RESULT IVideoInterface::EndScene()
{

	//flip it!
	// if we're windowed do the blit, else just Flip
	if (g_bFullscreen)
	{
		HRESULT hr = g_pDDSPrimary->Flip( NULL, DDFLIP_WAIT );
		if(hr != DD_OK)
		{
			char* errorCode = DDErrorString(hr);
			return RESULT_ERROR;
		}
	}
	else
	{
		RECT    rcSrc;  // source blit rectangle
		RECT    rcDest; // destination blit rectangle
		POINT   p;

		// find out where on the primary surface our window lives
		p.x = 0; p.y = 0;
		::ClientToScreen(g_hWnd, &p);
		::GetClientRect(g_hWnd, &rcDest);
		OffsetRect(&rcDest, p.x, p.y);
		SetRect(&rcSrc, 0, 0, g_nWidth, g_nHeight);
		//if(FAILED (g_pDDSPrimary->Blt(&rcDest, g_pDDSBack, &rcSrc, DDBLT_WAIT, NULL)) )

		while(1) {
			HRESULT hr = g_pDDSPrimary->Blt(&rcDest, g_pDDSBack, &rcSrc, DDBLT_WAIT, NULL);
			if(hr != DDERR_WASSTILLDRAWING)
			{
				if(FAILED(hr)) {
					char* errorCode = DDErrorString(hr);
					return RESULT_ERROR;
				} else {
					break;
				}
			}
		}
	}

	return RESULT_OK;
}

GEN_RESULT IVideoInterface::CreateSurface(HSURFACE &hSurface, int x, int y)
{
	LPDIRECTDRAWSURFACE7 _pSurface = NULL;
	DDSURFACEDESC2 desc;
	ZeroMemory( &desc, sizeof( DDSURFACEDESC2 ) );
	desc.dwSize = sizeof( DDSURFACEDESC2 );
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = x;
	desc.dwHeight = y;
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	//desc.dwBackBufferCount = 0;

	if( FAILED(g_pDirectDraw->CreateSurface(&desc, &_pSurface, NULL)) ) 
	{
		return RESULT_ERROR;
	}
	
	DDBLTFX ddbfx;
	RECT rcDest;
	ddbfx.dwSize = sizeof( ddbfx );
	ddbfx.dwFillColor = (DWORD) 0xCCFFFFFF; //ARGB
	SetRect( &rcDest, 0, 0, x, y );

	/*
	if( FAILED(_pSurface->Blt( &rcDest, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ddbfx )) )
	{
		return RESULT_ERROR;
	}
	*/
	HRESULT hr = _pSurface->Blt( &rcDest, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ddbfx );

	if(FAILED(hr))
	{
		char* str = DDErrorString(hr);
		return RESULT_ERROR;
	}

	//set the color key
	//DDCOLORKEY colorKey;
	//colorKey.dwColorSpaceLowValue = colorKey.dwColorSpaceHighValue = ConvertGDIColor (0xFF<<24) | (0x00<<16) | (0x00<<8) | (0x00<<0);
	SetColorKey(_pSurface, RGB(255,0,255));

	//hr = _pSurface->SetColorKey(DDCKEY_SRCBLT, &colorKey);

	//If good:
	hSurface = (HSURFACE)_pSurface;

	return RESULT_OK;
}

//#include <stdio.h>
GEN_RESULT IVideoInterface::CreateSurfaceFromRaw(HSURFACE &hSurface, int x, int y, const char* szFilename)
{
	FileInfo fileInfo;
	unsigned char *pszData = NULL;

	BaseFile* pFile = CResourceManager::GetSingleton().FindFile(szFilename, &fileInfo);
	if(pFile)
	{
		int nDataSize = (x * y) * 3;
		pszData = new unsigned char[nDataSize];

		pFile->Read(pszData, nDataSize);

		pFile->Close();
	}
	else
	{
		return RESULT_ERROR;
	}

	/*
	FILE* fp = NULL;
	fp = fopen(szFilename, "rb");

	int nDataSize = (x * y) * 3;
	unsigned char *szData = new unsigned char[nDataSize];

	if(fp)
	{
		int nSizeRead = (int)fread(szData, nDataSize, 1, fp);
		fclose(fp);
	}
	else
	{
		delete [] szData;
		return RESULT_ERROR;
	}
	*/


	CreateSurface(hSurface, x, y);

	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 pSurface = (LPDIRECTDRAWSURFACE7)hSurface;

	ZeroMemory( &ddsd,sizeof(ddsd) );
	ddsd.dwSize = sizeof(ddsd);

	pSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

	DWORD* pDDSColor = (DWORD*) ddsd.lpSurface;

	for(int i = 0; i < y; ++i)
	{
		for(int j = 0; j < (x*3); j+=3)
		{
			*pDDSColor = (DWORD)(  (0xFF << 24) | ((pszData[j + (i*(x*3))]) << 16) | ((pszData[j + (i*(x*3)) + 1]) << 8) | ((pszData[j + (i*(x*3)) + 2]) << 0));
			++pDDSColor;
		}

		pDDSColor = (DWORD*) ( (BYTE*) ddsd.lpSurface + ( (i+1) * ddsd.lPitch) );
	}

	pSurface->Unlock(NULL);

	delete [] pszData;

	return RESULT_OK;
}

GEN_RESULT IVideoInterface::CreateSurfaceFromFile(HSURFACE &hSurface, int *x, int *y, const char* szFilename)
{
	FileInfo fileInfo;
	unsigned char *pszData = NULL;

	BaseFile* pFile = CResourceManager::GetSingleton().FindFile(szFilename, &fileInfo);
	if(pFile)
	{
		int nDataSize = fileInfo.nFilesize;//(x * y) * 3;
		pszData = new unsigned char[nDataSize];

		pFile->Read(pszData, nDataSize);

		pFile->Close();
	}
	else
	{
		return RESULT_ERROR;
	}

	D3DXIMAGE_INFO srcInfo;
	HRESULT res = D3DXGetImageInfoFromFileInMemory(pszData, fileInfo.nFilesize, &srcInfo);
	if (FAILED(res)) 
	{
#ifdef _DEBUG
		char szError[128];
		sprintf(szError, "CreateSurfaceFromFile failed %s (%d)\n", __FILE__, __LINE__);
		OutputDebugString(szError);
#endif
		return RESULT_ERROR;
	}

	//
	LPDIRECT3DSURFACE8 srcImage = NULL;
	res = m_pDevice->CreateImageSurface(srcInfo.Width,srcInfo.Height,D3DFMT_X8R8G8B8,&srcImage);
	if (FAILED(res)) 
	{
		char szError[128];

		switch(res)
		{
		case D3DERR_INVALIDCALL:
#ifdef _DEBUG
			sprintf(szError, "CreateSurfaceFromFile failed %s (%d)\n", __FILE__, __LINE__);
			OutputDebugString(szError);
#endif
			return RESULT_ERROR;
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
#ifdef _DEBUG
			sprintf(szError, "CreateSurfaceFromFile failed %s (%d)\n", __FILE__, __LINE__);
			OutputDebugString(szError);
#endif
			return RESULT_ERROR;
			break;
		case E_OUTOFMEMORY:
#ifdef _DEBUG
			sprintf(szError, "CreateSurfaceFromFile failed %s (%d)\n", __FILE__, __LINE__);
			OutputDebugString(szError);
#endif
			return RESULT_ERROR;
			break;
		default:
			break;
		}

#ifdef _DEBUG
		sprintf(szError, "CreateSurfaceFromFile failed %s (%d)\n", __FILE__, __LINE__);
		OutputDebugString(szError);
#endif
		return RESULT_ERROR;

	}

	res = D3DXLoadSurfaceFromFileInMemory(srcImage,NULL,NULL,pszData,fileInfo.nFilesize,
		NULL,D3DX_FILTER_NONE,0,&srcInfo);

	if(res == D3DERR_INVALIDCALL)
	{
#ifdef _DEBUG
		char szError[128];
		sprintf(szError, "CreateSurfaceFromFile failed %s (%d)\n", __FILE__, __LINE__);
		OutputDebugString(szError);
#endif
		return RESULT_ERROR;
	}

	if(res == D3DXERR_INVALIDDATA)
	{
#ifdef _DEBUG
		char szError[128];
		sprintf(szError, "CreateSurfaceFromFile failed %s (%d)\n", __FILE__, __LINE__);
		OutputDebugString(szError);
#endif
		return RESULT_ERROR;
	}

	if (FAILED(res)) 
	{
#ifdef _DEBUG
		char szError[128];
		sprintf(szError, "CreateSurfaceFromFile failed %s (%d)\n", __FILE__, __LINE__);
		OutputDebugString(szError);
#endif
		return RESULT_ERROR;
	}

	//DDSURFACEDESC2 ddsd_src;
	//ZeroMemory( &ddsd_src,sizeof(ddsd_src) );
	//ddsd_src.dwSize = sizeof(ddsd_src);

	//srcImage->Lock(NULL, &ddsd_src, DDLOCK_WAIT, NULL);
	//srcImage->LockRect()

	D3DLOCKED_RECT ppLockedRect; 
	srcImage->LockRect(&ppLockedRect,NULL,0); 

	*x = srcInfo.Width;
	*y = srcInfo.Height;

	DWORD* pDDSSrcColor = (DWORD*) ppLockedRect.pBits;//ddsd.lpSurface;


	CreateSurface(hSurface, *x, *y);

	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 pSurface = (LPDIRECTDRAWSURFACE7)hSurface;

	ZeroMemory( &ddsd,sizeof(ddsd) );
	ddsd.dwSize = sizeof(ddsd);

	pSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

	DWORD* pDDSColor = (DWORD*) ddsd.lpSurface;


	for(int i = 0; i < (*y); ++i)
	{
		for(int j = 0; j < ((*x)*4); j+=4)
		{
			*pDDSColor = *pDDSSrcColor;//(DWORD)(  (0xFF << 24) | ((pszData[j + (i*(x*3))]) << 16) | ((pszData[j + (i*(x*3)) + 1]) << 8) | ((pszData[j + (i*(x*3)) + 2]) << 0));
			++pDDSSrcColor;
			++pDDSColor;
		}

		pDDSColor = (DWORD*) ( (BYTE*) ddsd.lpSurface + ( (i+1) * ddsd.lPitch) );
		pDDSSrcColor = (DWORD*) ( (BYTE*) ppLockedRect.pBits + ( (i+1) * ppLockedRect.Pitch) );
	}

	pSurface->Unlock(NULL);

	// Dump the source image info.
	srcImage->UnlockRect();
	srcImage->Release();

	delete [] pszData;

	return RESULT_OK;
}

GEN_RESULT IVideoInterface::GetSurfaceDims(HSURFACE hSurface, int *x, int *y)
{
	ValidateSurface(hSurface);

	LPDIRECTDRAWSURFACE7 pSurface = ((LPDIRECTDRAWSURFACE7)hSurface);

	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(LPDDSURFACEDESC2));
	desc.dwSize = sizeof(DDSURFACEDESC2);
	HRESULT hr = pSurface->GetSurfaceDesc(&desc);
	if(FAILED(hr))
	{
		return RESULT_ERROR;
	}

	*x = desc.dwWidth;
	*y = desc.dwHeight;

	return RESULT_OK;
}

GEN_RESULT IVideoInterface::ValidateSurface(HSURFACE hSurface)
{
	if(NULL == hSurface)
	{
		return RESULT_ERROR;
	}

	if ( ((LPDIRECTDRAWSURFACE7)hSurface)->IsLost() == DDERR_SURFACELOST)		
	{
		((LPDIRECTDRAWSURFACE7)hSurface)->Restore();
	}

	return RESULT_OK;
}

GEN_RESULT IVideoInterface::DrawSurface(HSURFACE hSurface, int x, int y, RectInt2D *pRect)
{
	if(NULL == hSurface)
	{
		return RESULT_ERROR;
	}

	ValidateSurface(hSurface);

	LPDIRECTDRAWSURFACE7 pSurface = ((LPDIRECTDRAWSURFACE7)hSurface);

	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(LPDDSURFACEDESC2));
	desc.dwSize = sizeof(DDSURFACEDESC2);
	//pSurface->Lock();
	HRESULT hr = pSurface->GetSurfaceDesc(&desc);
	//pSurface->Unlock();

	RECT rcSrc;

	if(pRect)
	{
		SetRect( &rcSrc, pRect->x, pRect->y, pRect->x2, pRect->y2);
	}
	else
	{
		SetRect( &rcSrc, 0, 0, desc.dwWidth , desc.dwHeight );
	}


	//RECT rcDest;
	
	if(x < 0)
	{
		x = 0;
	}

	if(y < 0)
	{
		y = 0;
	}

	/*
	if(pRect)
	{
		SetRect(&rcDest, x, y, x+(pRect->x2 - pRect->x), y+(pRect->y2 - pRect->y) );
	}
	else
	{
		SetRect(&rcDest, x, y, x+desc.dwWidth, y+desc.dwHeight);
	}
	*/
	
#if 0				
	{
		char szBuf[256];
		memset(szBuf, 0, 256);
		sprintf(szBuf, "D: %d %d (%d,%d,%d,%d)\n", x, y, rcSrc.left, rcSrc.top, rcSrc.right, rcSrc.bottom);
		OutputDebugString(szBuf);
	}
#endif

	//if( FAILED(g_pDDSBack->Blt( &rcDest, pSurface, &rcSrc, DDBLT_WAIT, NULL )) )
	hr = g_pDDSBack->BltFast( x, y, pSurface, &rcSrc, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY );
	if( FAILED(hr) )
	{
		const char* error = DDErrorString(hr);
		return RESULT_ERROR;
	}

	return RESULT_OK;
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

#include "performance.h"
GEN_RESULT IVideoInterface::DrawSurfaceAlpha(HSURFACE hSurface, int x, int y, float fAlpha, RectInt2D *pRect)
{
#pragma message("FIXME: IVideoInterface::DrawSurfaceAlpha ignoring pRect...")

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

	if(y > (int)descBack.dwHeight)
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

	bool bOdd = false;
	int nSkipped = 0;

	for(int vertical = 0; vertical < nHeight; ++vertical)
	{
		for(int horiz = 0; horiz < nWidth; ++horiz)
		{
		
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

				nRedTmp = nRedDest + ( (nAlpha * (nRedSrc - nRedDest)) >> 8);
				nGreenTmp = nGreenDest + ( (nAlpha * (nGreenSrc - nGreenDest)) >> 8);
				nBlueTmp = nBlueDest + ( (nAlpha * (nBlueSrc - nBlueDest)) >> 8);

				//save to the dest surface
				*pDest = (0xFF000000) | GET_MASK(nRedTmp, REDMASK) | GET_MASK(nGreenTmp, GREENMASK) | GET_MASK(nBlueTmp, BLUEMASK);

				++pDest;
				++pSrc;


		}

		pDest += nDestSkip;

		//if the source is smaller than the destination surface.
		if(nSrcWidth < nDestWidth)
		{	
			pDest += ((descBack.dwWidth - x) - desc.dwWidth) + x;
		}

		pSrc += nSrcSkip;


	}



	counter.Stop();
	float fDelta = counter.TimeInMilliseconds();
	DEBUG_LOG(if(fp) {fprintf(fp, "[%06d] - %f\n", ++g_nLogCounter, fDelta);})

	g_pDDSBack->Unlock(NULL);
	pSurface->Unlock(NULL);

	return RESULT_OK;
}

GEN_RESULT IVideoInterface::ReleaseSurface(HSURFACE hSurface)
{
	if(hSurface)
	{
		((LPDIRECTDRAWSURFACE7)hSurface)->Release();
	}

	return RESULT_OK;
}

GEN_RESULT IVideoInterface::DrawText(const char* szText, int x, int y)
{
	if(g_bFullscreen)
	{
		return RESULT_OK;
	}

	if(NULL == g_hFont)
	{
		return RESULT_ERROR;
	}

	int r = 255;
	int g = 255;
	int b = 255;

	HDC hdc;
	g_pDDSBack->GetDC(&hdc);
	SelectObject(hdc, g_hFont);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(r,g,b) );
	TextOut(hdc, x,y, szText, lstrlen(szText));
	g_pDDSBack->ReleaseDC(hdc);


	return RESULT_OK;
}

GEN_RESULT IVideoInterface::DrawText(HFONT2D hFont, const char* szText, int x, int y, uint32 nARGBColor)
{
	if(g_bFullscreen)
	{
		return RESULT_OK;
	}

	if(NULL == hFont)
	{
		return RESULT_ERROR;
	}

	int r = ((0x00FF0000 & nARGBColor) >> 16);
	int g = ((0x0000FF00 & nARGBColor) >> 8);
	int b = (0x000000FF & nARGBColor);

	HDC hdc;
	g_pDDSBack->GetDC(&hdc);
	SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(r,g,b) );
	TextOut(hdc, x,y, szText, lstrlen(szText));
	g_pDDSBack->ReleaseDC(hdc);


	return RESULT_OK;
}

GEN_RESULT IVideoInterface::DrawLine(HSURFACE hSurface, Vector2d& vStartPos, Vector2d& vEndPos, uint32 uColor)
{
	ValidateSurface(hSurface);

	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 pSurface = (LPDIRECTDRAWSURFACE7)hSurface;

	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(LPDDSURFACEDESC2));
	desc.dwSize = sizeof(DDSURFACEDESC2);
	//pSurface->Lock();
	HRESULT hr = pSurface->GetSurfaceDesc(&desc);

	if(FAILED(hr))
	{
		return RESULT_ERROR;
	}

	if(vEndPos.x > (desc.dwWidth-1))
	{
		return RESULT_OK;
	}

	ZeroMemory( &ddsd,sizeof(ddsd) );
	ddsd.dwSize = sizeof(ddsd);

	pSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

	DWORD* pDDSColor = (DWORD*) ddsd.lpSurface;

	Vector2d _vStartPos = vStartPos;
	Vector2d _vEndPos = vEndPos;
	Vector2d vDir = _vEndPos - _vStartPos;
	vDir.Normalize();
	
	Vector2d vLinePos = vStartPos;
	
	while(!vLinePos.CloseTo(vEndPos))
	{
		//32 bit surface.. so multiply by four...
		pDDSColor = (DWORD*) ((BYTE*) ddsd.lpSurface + (((int)vLinePos.y * ddsd.lPitch) + ((int)vLinePos.x*4)));
		*pDDSColor = uColor;
		vLinePos += vDir;
	}

	pSurface->Unlock(NULL);

	
	return RESULT_OK;
}

GEN_RESULT IVideoInterface::SetSurfacePixel(HSURFACE hSurface, int x, int y, uint32 uColor)
{
	ValidateSurface(hSurface);

	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 pSurface = (LPDIRECTDRAWSURFACE7)hSurface;

	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(LPDDSURFACEDESC2));
	desc.dwSize = sizeof(DDSURFACEDESC2);
	//pSurface->Lock();
	HRESULT hr = pSurface->GetSurfaceDesc(&desc);

	if(FAILED(hr))
	{
		return RESULT_ERROR;
	}

	if(x > (desc.dwWidth-1))
	{
		return RESULT_OK;
	}

	ZeroMemory( &ddsd,sizeof(ddsd) );
	ddsd.dwSize = sizeof(ddsd);

	pSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

	DWORD* pDDSColor = (DWORD*) ddsd.lpSurface;

	//32 bit surface.. so multiply by four...
	pDDSColor = (DWORD*) ((BYTE*) ddsd.lpSurface + ((y * ddsd.lPitch) + x*4));

	*pDDSColor = uColor;

	pSurface->Unlock(NULL);

	return RESULT_OK;
}

HFONT IVideoInterface::CreateFont(const char* szName, EFontStyle eStyle, int nHeight)
{
	// start our font
	LOGFONT logFont;
	ZeroMemory(&logFont, sizeof(LOGFONT));
	strcpy(logFont.lfFaceName, szName);//"Arial");
	logFont.lfHeight = nHeight;
	logFont.lfQuality = ANTIALIASED_QUALITY;

	switch(eStyle)
	{
	case FONTSTYLE_NORMAL:
		{
			logFont.lfWeight = FW_DONTCARE;
		}
		break;
		case FONTSTYLE_BOLD:
		{
			logFont.lfWeight = FW_BOLD;
		}
		break;
		case FONTSTYLE_ITALIC:
		{
			logFont.lfItalic = TRUE;
		}
		break;
		case FONTSTYLE_BOLDITALIC:
		{
			logFont.lfWeight = FW_BOLD;
			logFont.lfItalic = TRUE;
		}
		break;
	default:
		{}
		break;
	}

	HFONT hFont = CreateFontIndirect(&logFont);
	if(hFont)
	{
		g_Fonts.push_back(hFont);
	}

	return hFont;
}

GEN_RESULT IVideoInterface::ReleaseFont(HFONT hFont)
{
	Fonts::iterator iter = find( g_Fonts.begin(), g_Fonts.end(), hFont);
	if(iter != g_Fonts.end())
	{
		if(*iter)
		{
			DeleteObject(*iter);
			g_Fonts.erase(iter);
		}
	}

	return RESULT_OK;
}

GEN_RESULT IVideoInterface::GetStringSize(HFONT hFont, const char* szText, VectorInt2d &vSize)
{
	GEN_RESULT result = RESULT_OK;

	if(g_bFullscreen)
	{
		return RESULT_OK;
	}

	HDC hdc;
	g_pDDSBack->GetDC(&hdc);
	SelectObject(hdc, hFont);

	int nLen = strlen(szText);

	SIZE _size;
	BOOL bRet = GetTextExtentPoint32(hdc, szText, nLen, &_size);
	if(bRet)
	{
		vSize.x = _size.cx;
		vSize.y = _size.cy;
	}
	else
	{
		result = RESULT_ERROR;
	}

	g_pDDSBack->ReleaseDC(hdc);

	return result;

}